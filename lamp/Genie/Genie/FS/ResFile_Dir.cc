/*	==============
 *	ResFile_Dir.cc
 *	==============
 */

#include "Genie/FS/ResFile_Dir.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <sys/stat.h>

// mac-file-utils
#include "mac_file/open_rsrc_fork.hh"

// mac-rsrc-utils
#include "mac_rsrc/create_res_file.hh"
#include "mac_rsrc/open_res_file.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/resources.hh"


namespace Genie
{
	
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
		using mac::rsrc::open_res_file;
		
		::ResFileRefNum refNum = open_res_file( file, fsRdPerm );
		
		const bool exists = refNum >= 0;
		
		if ( exists )
		{
			::CloseResFile( refNum );
		}
		else
		{
			const OSErr err = refNum;
			
			if ( err != eofErr )
			{
				Mac::ThrowOSStatus( err );
			}
		}
		
		return exists;
	}
	
	static void resfile_dir_remove( const vfs::node* that )
	{
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		/*
			Don't truncate the resource fork if opening the resfile fails.
			It might contain data that's foreigh to the Resource Manager.
		*/
		
		short n_types = -1;
		
		short resfile = mac::rsrc::open_res_file( fileSpec, fsRdWrPerm );
		
		if ( resfile > 0 )
		{
			n_types = Count1Types();
			
			CloseResFile( resfile );
		}
		
		if ( n_types != 0 )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		short refnum = mac::file::open_rsrc_fork( fileSpec, fsRdWrPerm );
		
		OSErr err = refnum;
		
		if ( refnum >= 0 )
		{
			err = SetEOF( refnum, 0 );
			
			FSClose( refnum );
		}
		
		Mac::ThrowOSStatus( err );
	}
	
	static void empty_rsrc_fork_mkdir( const vfs::node* that, mode_t mode )
	{
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		using mac::rsrc::create_res_file;
		
		Mac::ThrowOSStatus( create_res_file( fileSpec ) );
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
