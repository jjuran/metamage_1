/*
	resources.cc
	------------
*/

#include "Genie/FS/resources.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// mac-glue-utils
#include "mac_glue/Memory.hh"

// iota
#include "iota/char_types.hh"

// gear
#include "gear/hexadecimal.hh"

// mac-sys-utils
#include "mac_sys/mem_error.hh"

// mac-rsrc-utils
#include "mac_rsrc/open_res_file.hh"

// plus
#include "plus/hexadecimal.hh"
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// MacScribe
#include "quad/quad_name.hh"

// nucleus
#include "nucleus/owned.hh"

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_OSSTATUS_HH
#include "Mac/Toolbox/Types/OSStatus.hh"
#endif

#include "Nitrogen/Resources.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/IO/Handle.hh"
#include "Genie/Utilities/RdWr_OpenResFile_Scope.hh"


namespace Nitrogen
{
	
	namespace n = nucleus;
	
	static
	n::owned< ResFileRefNum > open_res_file( const FSSpec& file, SInt8 perm )
	{
		::ResFileRefNum opened = mac::rsrc::open_res_file( file, perm );
		
		ResError();
		
		ResFileRefNum refNum = ResFileRefNum( opened );
		
		return nucleus::owned< ResFileRefNum >::seize( refNum );
	}
	
}

namespace Genie
{

namespace n = nucleus;
namespace N = Nitrogen;
namespace p7 = poseven;


using MacScribe::make_quad_name;
using MacScribe::parse_quad_name;


struct ResSpec
{
	Mac::ResType  type;
	Mac::ResID    id;
};

class ResLoad_false_scope
{
	private:
		// non-copyable
		ResLoad_false_scope           ( const ResLoad_false_scope& );
		ResLoad_false_scope& operator=( const ResLoad_false_scope& );
	
	public:
		ResLoad_false_scope()
		{
			::SetResLoad( false );
		}
		
		~ResLoad_false_scope()
		{
			::SetResLoad( true );
		}
};


void iterate_resources( const FSSpec& file, vfs::dir_contents& cache )
{
	n::owned< N::ResFileRefNum > resFile = N::open_res_file( file, fsRdPerm );
	
	const short n_types = N::Count1Types();
	
	for ( short i = 1;  i <= n_types;  ++i )
	{
		const N::ResType type = N::Get1IndType( i );
		
		const short n_rsrcs = N::Count1Resources( type );
		
		for ( short j = 1;  j <= n_rsrcs;  ++j )
		{
			const N::Handle r = N::Get1IndResource( type, j );
			
			const mac::types::ResInfo info = N::GetResInfo( r );
			
			plus::var_string name = plus::encode_16_bit_hex( info.id );
			
			name += '.';
			
			name += make_quad_name( type );
			
			const vfs::dir_entry node( info.id, name );
			
			cache.push_back( node );
		}
	}
}

static
ResSpec GetResSpec_from_name( const plus::string& name )
{
	const plus::string& mac_name = plus::mac_from_utf8( name );
	
	const char* begin = mac_name.data();
	const char* end   = mac_name.size() + begin;
	
	// In the event of a short name, is_xdigit( '\0' ) will return false
	
	const bool has_id =      iota::is_xdigit( begin[ 0 ] )
	                     &&  iota::is_xdigit( begin[ 1 ] )
	                     &&  iota::is_xdigit( begin[ 2 ] )
	                     &&  iota::is_xdigit( begin[ 3 ] );
	
	if ( ! has_id )
	{
		p7::throw_errno( ENOENT );
	}
	
	const short id = gear::decode_16_bit_hex( begin );
	
	begin += 4;
	
	if ( *begin++ != '.' )
	{
		p7::throw_errno( ENOENT );
	}
	
	const ::OSType type = parse_quad_name( begin, end - begin );
	
	const ResSpec result = { Mac::ResType( type ), Mac::ResID( id ) };
	
	return result;
}


static
void mac_name_get( plus::var_string& result, const vfs::node* that, bool binary )
{
	const vfs::node* res_file = that->owner();
	
	const FSSpec& fileSpec = *(FSSpec*) res_file->extra();
	
	n::owned< N::ResFileRefNum > resFile = N::open_res_file( fileSpec, fsRdPerm );
	
	const ResSpec resSpec = GetResSpec_from_name( that->name() );
	
	ResLoad_false_scope ResLoad_false;
	
	const N::Handle r = N::Get1Resource( resSpec.type, resSpec.id );
	
	const mac::types::ResInfo resInfo = N::GetResInfo( r );
	
	::ReleaseResource( r );
	
	result.assign( resInfo.name );
}

static
void mac_name_set( const vfs::node* that, const char* begin, const char* end, bool binary )
{
	const size_t length = end - begin;
	
	if ( length > 255 )
	{
		p7::throw_errno( ENAMETOOLONG );
	}
	
	Str255 name;
	
	name[ 0 ] = length;
	
	mempcpy( name + 1, begin, length );
	
	const vfs::node* res_file = that->owner();
	
	const FSSpec& fileSpec = *(FSSpec*) res_file->extra();
	
	RdWr_OpenResFile_Scope openResFile( fileSpec );
	
	const ResSpec resSpec = GetResSpec_from_name( that->name() );
	
	ResLoad_false_scope ResLoad_false;
	
	const N::Handle r = N::Get1Resource( resSpec.type, resSpec.id );
	
	N::SetResInfo( r, resSpec.id, name );
	
	::ReleaseResource( r );
}

static
void utf8_name_get( plus::var_string& result, const vfs::node* that, bool binary )
{
	mac_name_get( result, that, binary );
	
	result = plus::utf8_from_mac( result );
}

static
void utf8_name_set( const vfs::node* that, const char* begin, const char* end, bool binary )
{
	plus::string mac_text = plus::mac_from_utf8( begin, end - begin );
	
	mac_name_set( that, mac_text.begin(), mac_text.end(), binary );
}

static const vfs::property_params mac_name_params =
{
	vfs::no_fixed_size,
	&mac_name_get,
	&mac_name_set,
};

static const vfs::property_params utf8_name_params =
{
	vfs::no_fixed_size,
	&utf8_name_get,
	&utf8_name_set,
};


struct rsrc_extra : Mac_Handle_extra
{
	FSSpec filespec;
};

static
N::Handle GetOrAddResource( const ResSpec& resSpec )
{
	try
	{
		return N::Get1Resource( resSpec.type, resSpec.id );
	}
	catch ( const Mac::OSStatus& err )
	{
		if ( err != resNotFound )
		{
			throw;
		}
		
		return N::AddResource( N::NewHandle( 0 ), resSpec.type, resSpec.id, NULL );
	}
}

static
void flush_resource( vfs::filehandle* that )
{
	rsrc_extra& extra = *(rsrc_extra*) that->extra();
	
	vfs::node_ptr file = get_file( *that );
	
	const ResSpec resSpec = GetResSpec_from_name( file->name() );
	
	RdWr_OpenResFile_Scope openResFile( extra.filespec );
	
	const N::Handle r = GetOrAddResource( resSpec );
	
	const size_t size = mac::glue::GetHandleSize( extra.handle );
	
	SetHandleSize( r, size );
	
	Mac::ThrowOSStatus( mac::sys::mem_error() );
	
	mempcpy( *r.Get(), *extra.handle, size );
	
	N::ChangedResource( r );
	N::WriteResource  ( r );
}

static
void rsrc_cleanup( vfs::filehandle* that )
{
	rsrc_extra& extra = *(rsrc_extra*) that->extra();
	
	try
	{
		flush_resource( that );
	}
	catch ( ... )
	{
	}
	
	::DisposeHandle( extra.handle );
}

static
vfs::filehandle* new_rsrc_handle( const vfs::node&        file,
                                  int                     flags,
                                  n::owned< N::Handle >&  h,
                                  const FSSpec&           resFile )
{
	vfs::filehandle* result = new vfs::filehandle( &file,
	                                               flags,
	                                               &Mac_Handle_methods,
	                                               sizeof (rsrc_extra),
	                                               &rsrc_cleanup );
	
	rsrc_extra& extra = *(rsrc_extra*) result->extra();
	
	extra.handle   = h.release();
	extra.filespec = resFile;
	
	return result;
}


static vfs::filehandle_ptr unrsrc_file_open( const vfs::node* that, int flags, mode_t mode );

static const vfs::data_method_set unrsrc_file_data_methods =
{
	&unrsrc_file_open
};

static const vfs::node_method_set unrsrc_file_methods =
{
	NULL,
	&unrsrc_file_data_methods
};


static void rsrc_file_remove( const vfs::node* that );

static void rsrc_file_rename( const vfs::node* that, const vfs::node* destination );

static vfs::filehandle_ptr rsrc_file_open( const vfs::node* that, int flags, mode_t mode );

static off_t rsrc_file_geteof( const vfs::node* that );

static
vfs::node_ptr rsrc_file_lookup( const vfs::node*     that,
                                const plus::string&  name,
                                const vfs::node*     parent )
{
	bool binary = false;
	bool mac    = false;
	
	const char* p = name.c_str();
	
	if ( *p == '.' )
	{
		++p;
		
		if ( *p == '~' )
		{
			binary = true;
			
			++p;
		}
		
		if ( memcmp( p, "mac-", sizeof "mac-" - 1) == 0 )
		{
			mac = true;
			
			p += sizeof "mac-" - 1;
		}
		else if ( ! binary )
		{
			p = "";  // Don't match on ".name"
		}
	}
	
	if ( memcmp( p, "name", sizeof "name" ) != 0 )
	{
		p7::throw_errno( ENOENT );
	}
	
	const vfs::property_params& params = mac ? mac_name_params
	                                         : utf8_name_params;
	
	return vfs::new_property( that, name, &params );
}

static const vfs::item_method_set rsrc_file_item_methods =
{
	NULL,
	NULL,
	NULL,
	NULL,
	&rsrc_file_remove,
	&rsrc_file_rename,
};

static const vfs::data_method_set rsrc_file_data_methods =
{
	&rsrc_file_open,
	&rsrc_file_geteof
};

static const vfs::dir_method_set rsrc_file_dirmethods =
{
	&rsrc_file_lookup
};

static const vfs::node_method_set rsrc_file_methods =
{
	&rsrc_file_item_methods,
	&rsrc_file_data_methods,
	NULL,
	&rsrc_file_dirmethods
};


static
bool has_resource( const FSSpec& file, const ResSpec& resSpec )
{
	n::owned< N::ResFileRefNum > resFile = N::open_res_file( file, fsRdPerm );
	
	return ::Get1Resource( resSpec.type, resSpec.id ) != NULL;
}

static
void rsrc_file_remove( const vfs::node* that )
{
	const FSSpec& fileSpec = *(FSSpec*) that->extra();
	
	RdWr_OpenResFile_Scope openResFile( fileSpec );
	
	const ResSpec resSpec = GetResSpec_from_name( that->name() );
	
	const N::Handle r = N::Get1Resource( resSpec.type, resSpec.id );
	
	(void) N::RemoveResource( r );
}

static
void rsrc_file_rename( const vfs::node* that, const vfs::node* destination )
{
	if ( destination->owner() != that->owner() )
	{
		p7::throw_errno( EXDEV );
	}
	
	const plus::string& new_name = destination->name();
	
	const ResSpec old_resSpec = GetResSpec_from_name( that->name() );
	
	const ResSpec new_resSpec = GetResSpec_from_name( new_name );
	
	if ( old_resSpec.type != new_resSpec.type )
	{
		p7::throw_errno( EXDEV );
	}
	
	const FSSpec& fileSpec = *(FSSpec*) that->extra();
	
	RdWr_OpenResFile_Scope openResFile( fileSpec );
	
	::SetResLoad( false );
	
	if ( const Handle r = ::Get1Resource( new_resSpec.type, new_resSpec.id ) )
	{
		::RemoveResource( r );
	}
	
	::SetResLoad( true );
	
	const N::Handle r = N::Get1Resource( old_resSpec.type, old_resSpec.id );
	
	const mac::types::ResInfo resInfo = N::GetResInfo( r );
	
	N::SetResInfo( r, new_resSpec.id, resInfo.name );
}

static
off_t rsrc_file_geteof( const vfs::node* that )
{
	const FSSpec& fileSpec = *(FSSpec*) that->extra();
	
	n::owned< N::ResFileRefNum > resFile = N::open_res_file( fileSpec, fsRdPerm );
	
	const ResSpec resSpec = GetResSpec_from_name( that->name() );
	
	const N::Handle r = N::Get1Resource( resSpec.type, resSpec.id );
	
	return mac::glue::GetHandleSize( r );
}

static
vfs::filehandle_ptr unrsrc_file_open( const vfs::node* that, int flags, mode_t mode )
{
	const bool writing = flags + (1 - O_RDONLY) & 2;
	
	const bool creating = flags & O_CREAT;
	
	if ( ! creating )
	{
		p7::throw_errno( ENOENT );
	}
	
	const FSSpec& fileSpec = *(FSSpec*) that->extra();
	
	{
		RdWr_OpenResFile_Scope openResFile( fileSpec );
		
		const ResSpec resSpec = GetResSpec_from_name( that->name() );
		
		(void) N::AddResource( N::NewHandle( 0 ), resSpec.type, resSpec.id, NULL );
	}
	
	n::owned< N::Handle > h = N::NewHandle( 0 );
	
	// that refers to an unrsrc; make a new one that's a live rsrc
	
	vfs::node_ptr new_node = Get_RsrcFile_FSTree( that->owner(),
	                                              that->name(),
	                                              fileSpec );
	
	that = new_node.get();
	
	vfs::filehandle* result = writing ? new_rsrc_handle  ( *that, flags, h, fileSpec )
	                                  : new_Handle_handle( *that, flags, h );
	
	return result;
}

static
vfs::filehandle_ptr rsrc_file_open( const vfs::node* that, int flags, mode_t mode )
{
	const bool writing = flags + (1 - O_RDONLY) & 2;
	
	const FSSpec& fileSpec = *(FSSpec*) that->extra();
	
	n::owned< N::ResFileRefNum > resFile = N::open_res_file( fileSpec, fsRdPerm );
	
	const ResSpec resSpec = GetResSpec_from_name( that->name() );
	
	const N::Handle r = N::Get1Resource( resSpec.type, resSpec.id );
	
	n::owned< N::Handle > h = N::DetachResource( r );
	
	vfs::filehandle* result = writing ? new_rsrc_handle  ( *that, flags, h, fileSpec )
	                                  : new_Handle_handle( *that, flags, h );
	
	return result;
}

vfs::node_ptr Get_RsrcFile_FSTree( const vfs::node*     parent,
                                   const plus::string&  name,
                                   const FSSpec&        file )
{
	const ResSpec resSpec = GetResSpec_from_name( name );
	
	const bool exists = has_resource( file, resSpec );
	
	// FIXME:  Check perms
	const mode_t mode = exists ? S_IFREG | 0444 : 0;
	
	const vfs::node_method_set* methods = exists ? &rsrc_file_methods
	                                             : &unrsrc_file_methods;
	
	vfs::node* result = new vfs::node( parent, name, mode, methods, sizeof (FSSpec) );
	
	*(FSSpec*) result->extra() = file;
	
	return result;
}

}
