/*	==============
 *	ResFile_Dir.cc
 *	==============
 */

#include "Genie/FS/ResFile_Dir.hh"

// Standard C++
#include <algorithm>

// POSIX
#include <sys/stat.h>

// mac-sys-utils
#include "mac_sys/has/FSSpec_calls.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Resources.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/resources.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static void empty_rsrc_fork_mkdir( const vfs::node* that, mode_t mode );
	
	static const vfs::dir_method_set empty_rsrc_fork_dir_methods =
	{
		NULL,
		NULL,
		&empty_rsrc_fork_mkdir
	};
	
	static const vfs::node_method_set empty_rsrc_fork_methods =
	{
		NULL,
		NULL,
		NULL,
		&empty_rsrc_fork_dir_methods
	};
	
	
	static void resfile_dir_remove( const vfs::node* that );
	
	static vfs::node_ptr resfile_dir_lookup( const vfs::node*     that,
	                                         const plus::string&  name,
	                                         const vfs::node*     parent );
	
	static void resfile_dir_listdir( const vfs::node*    that,
	                                 vfs::dir_contents&  cache );
	
	static const vfs::item_method_set resfile_dir_item_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&resfile_dir_remove,
	};
	
	static const vfs::dir_method_set resfile_dir_dir_methods =
	{
		&resfile_dir_lookup,
		&resfile_dir_listdir
	};
	
	static const vfs::node_method_set resfile_dir_methods =
	{
		&resfile_dir_item_methods,
		NULL,
		NULL,
		&resfile_dir_dir_methods
	};
	
	
	static bool ResFile_dir_exists( const FSSpec& file )
	{
		if ( ! mac::sys::has_FSSpec_calls() )
		{
			p7::throw_errno( ENOSYS );
		}
		
		::ResFileRefNum refNum = ::FSpOpenResFile( &file, fsRdPerm );
		
		const bool exists = refNum >= 0;
		
		if ( exists )
		{
			::CloseResFile( refNum );
		}
		else
		{
			const OSErr err = ::ResError();
			
			if ( err != eofErr )
			{
				Mac::ThrowOSStatus( err );
			}
		}
		
		return exists;
	}
	
	static inline bool contains( const char* s, size_t length, char c )
	{
		const char* end = s + length;
		
		return std::find( s, end, c ) != end;
	}
	
	static inline bool is_rsrc_file( const CInfoPBRec&  cInfo,
	                                 ConstStr255Param   name )
	{
		return cInfo.hFileInfo.ioFlLgLen == 0  &&  !contains( (char*) &name[ 1 ],
		                                                      1 + name[0],
		                                                      '.' );
	}
	
	static void resfile_dir_remove( const vfs::node* that )
	{
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		n::owned< Mac::FSFileRefNum > resource_fork = N::HOpenRF( fileSpec, Mac::fsRdWrPerm );
		
		if ( N::GetEOF( resource_fork ) > 286 )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		N::SetEOF( resource_fork, 0 );
	}
	
	static void empty_rsrc_fork_mkdir( const vfs::node* that, mode_t mode )
	{
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		CInfoPBRec cInfo = {{ 0 }};
		
		const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo, false, fileSpec );
		
		::OSType creator;
		::OSType type;
		
		if ( !exists || is_rsrc_file( cInfo, fileSpec.name ) )
		{
			creator = 'RSED';
			type    = 'rsrc';
		}
		else
		{
			const FInfo& fInfo = cInfo.hFileInfo.ioFlFndrInfo;
			
			creator = fInfo.fdCreator;
			type    = fInfo.fdType;
		}
		
		N::FSpCreateResFile( fileSpec,
		                     Mac::FSCreator( creator ),
		                     Mac::FSType   ( type    ),
		                     Mac::smSystemScript );
	}
	
	static vfs::node_ptr resfile_dir_lookup( const vfs::node*     that,
	                                         const plus::string&  name,
	                                         const vfs::node*     parent )
	{
		if ( !exists( *that ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		return Get_RsrcFile_FSTree( parent, name, fileSpec );
	}
	
	static void resfile_dir_listdir( const vfs::node*    that,
	                                 vfs::dir_contents&  cache )
	{
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		iterate_resources( fileSpec, cache );
	}
	
	
	vfs::node_ptr Get_ResFileDir_FSTree( const vfs::node*     parent,
	                                     const plus::string&  name,
	                                     const FSSpec&        file )
	{
		const bool exists = ResFile_dir_exists( file );
		
		const mode_t mode = exists ? S_IFDIR | 0755 : 0;
		
		const vfs::node_method_set* methods = exists ? &resfile_dir_methods
		                                             : &empty_rsrc_fork_methods;
		
		vfs::node* result = new vfs::node( parent, name, mode, methods, sizeof (FSSpec) );
		
		*(FSSpec*) result->extra() = file;
		
		return result;
	}
	
}
