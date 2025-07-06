/*
	res_file.cc
	-----------
*/

// Standard C
#include <stdlib.h>
#include <string.h>

// Standard C++
#include <algorithm>

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"

// mac-relix-utils
#include "mac_relix/FSRef_from_path.hh"
#include "mac_relix/FSSpec_from_path.hh"

// plus
#include "plus/string.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// poseven
#include "poseven/types/errno_t.hh"

// cpres
#include "res_file.hh"


namespace n = nucleus;
namespace N = Nitrogen;
namespace p7 = poseven;

using mac::sys::Error;


class ForkNames
{
	private:
		HFSUniStr255 name[ 2 ];
	
	public:
		ForkNames()
		{
			/*
				Notice that we check for FSOpenResourceFile() here, not
				FSGetResourceForkName().  In Mac OS 9.0 (or earlier with
				CarbonLib), the latter is available and the former is not.
				In that case, we'll skip the FSGetResourceForkName() call
				even though it's available, because we only actually use
				the resource fork name when FSOpenResourceFile() exists.
			*/
			
			if ( has_FSOpenResourceFile() )
			{
				if ( OSErr err = ::FSGetResourceForkName( &name[ rsrcFork ] ) )
				{
					abort();
				}
			}
		}
		
		const HFSUniStr255* get() const  { return name; }
};

static ForkNames forkNames;

static const HFSUniStr255& getForkName( ForkType fork )
{
	return forkNames.get()[ fork ];
}

static inline const UniChar* get_string_data( const HFSUniStr255& str )
{
	return str.unicode;
}

static inline UniCharCount get_string_size( const HFSUniStr255& str )
{
	return str.length;
}

template < bool unicode >
struct file_manager_traits
{
	typedef FSSpec File;
	typedef FSSpec Node;
};

template <>
struct file_manager_traits< true >
{
	typedef N::FSRefNameSpec FSRefNameSpec;
	
	typedef FSRef          File;
	typedef FSRefNameSpec  Node;
};

static
void throw_error( Error err )
{
	int errnum = is_errno( err ) ? errno_from_muxed( err )
	                             : errno_from_mac_error( err );
	
	if ( errnum > 0 )
	{
		p7::throw_errno( errnum );
	}
	
	Mac::ThrowOSStatus( err );
}

static
void resolve_new_path( const char* path, FSSpec& node )
{
	using mac::relix::FSSpec_from_optional_path;
	
	Error err = FSSpec_from_optional_path( path, node );
	
	/*
		In MacRelix, this will always be an errno, not an OSErr.
	*/
	
#ifdef __RELIX__
	
	p7::throw_errno( errno_from_muxed( err ) );
	
#else
	
	throw_error( err );
	
#endif
}

static inline
void resolve_path( const char* path, FSSpec& file )
{
	resolve_new_path( path, file );
}

static
void resolve_path( const char* path, FSRef& file )
{
	using mac::relix::FSRef_from_path;
	
	Error err = FSRef_from_path( path, file );
	
	throw_error( err );
}

static
void resolve_new_path( const char* path, N::FSRefNameSpec& node )
{
	enum
	{
		utf8 = kCFStringEncodingUTF8,
	};
	
	const char* slash = strrchr( path, '/' );
	
	const char* filename = slash ? slash + 1 : path;
	
	FSRef& parent = node.parent;
	
	resolve_path( slash ? plus::string( path, slash ).c_str() : ".", parent );
	
	CFStringRef string = CFStringCreateWithCString( NULL, filename, utf8 );
	
	if ( ! string )
	{
		Mac::ThrowOSStatus( memFullErr );
	}
	
	CFIndex n = CFStringGetLength( string );
	
	node.name.length = n;
	
	CFStringGetCharacters( string, CFRangeMake( 0, n ), node.name.unicode );
	
	CFRelease( string );
}


static inline
const FSSpec& create_res_file( const FSSpec& file, ForkType fork, FSSpec& spec )
{
	::FSpCreateResFile( &file, 'RSED', 'rsrc', smRoman );
	
	return file;
}

static
const FSRef& create_res_file( const Mac::FSRefNameSpec& file, ForkType fork, FSRef& ref )
{
	const HFSUniStr255& forkName = getForkName( fork );
	
	N::FSCreateResourceFile( file, forkName );
	
	return ref = N::FSMakeFSRefUnicode( file, kTextEncodingUnknown );
}

static n::owned< N::ResFileRefNum > open_res_file( const FSSpec&   filespec,
                                                   ForkType        fork,
                                                   N::FSIOPermssn  perm )
{
	return N::FSpOpenResFile( filespec, perm );
}

static n::owned< N::ResFileRefNum > open_res_file( const FSRef&    file,
                                                   ForkType        fork,
                                                   N::FSIOPermssn  perm )
{
	const HFSUniStr255& forkName = getForkName( fork );
	
	return N::FSOpenResourceFile( file, forkName, perm );
}


template < bool unicode >
static n::owned< N::ResFileRefNum >
open_res_file_template( const char* path, ForkType fork, N::FSIOPermssn perm )
{
	typedef file_manager_traits< unicode > Traits;
	
	typename Traits::File destFile;
	
	resolve_path( path, destFile );
	
	return open_res_file( destFile, fork, perm );
}

template < bool unicode >
static n::owned< N::ResFileRefNum >
open_new_res_file_template( const char* path, ForkType fork )
{
	typedef file_manager_traits< unicode > Traits;
	
	typedef typename Traits::File File;
	typedef typename Traits::Node Node;
	
	/*
		With FSRefs, destFile is a reference to destFileStorage.
		
		With FSSpecs, Node and File are the same type and destFile
		refers to destNode (with destFileStorage remaining unused).
	*/
	
	Node destNode;
	File destFileStorage;
	
	resolve_new_path( path, destNode );
	
	const File& destFile = create_res_file( destNode, fork, destFileStorage );
	
	return open_res_file( destFile, fork, N::fsRdWrPerm );
}

n::owned< N::ResFileRefNum >
open_res_file( const char* path, ForkType fork )
{
	enum { no_unicode, unicode };
	
	const N::FSIOPermssn ro = N::fsRdPerm;
	
	if ( has_FSOpenResourceFile() )
	{
		return open_res_file_template< unicode >( path, fork, ro );
	}
	
	return open_res_file_template< no_unicode >( path, fork, ro );
}

n::owned< N::ResFileRefNum >
open_res_file( const char* path, ForkType fork, bool exists )
{
	enum { no_unicode, unicode };
	
	const N::FSIOPermssn rw = N::fsRdWrPerm;
	
	if ( exists )
	{
		if ( has_FSOpenResourceFile() )
		{
			return open_res_file_template< unicode >( path, fork, rw );
		}
		
		return open_res_file_template< no_unicode >( path, fork, rw );
	}
	
	if ( has_FSOpenResourceFile() )
	{
		return open_new_res_file_template< unicode >( path, fork );
	}
	
	return open_new_res_file_template< no_unicode >( path, fork );
}

void set_BNDL_bit( const char* path, bool value )
{
	FSSpec file;
	
	resolve_path( path, file );
	
	FInfo info = N::FSpGetFInfo( file );
	
	if ( value == ! (info.fdFlags & kHasBundle) )
	{
		info.fdFlags ^= kHasBundle;
		
		N::FSpSetFInfo( file, info );
	}
}
