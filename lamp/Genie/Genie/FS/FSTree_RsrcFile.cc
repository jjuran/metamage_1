/*	==================
 *	FSTree_RsrcFile.cc
 *	==================
 */

#include "Genie/FS/FSTree_RsrcFile.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// mac-file-utils
#include "mac_file/open_rsrc_fork.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/StatFile.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Utilities/GetCatInfo.hh"


namespace Genie
{
	
	static mode_t file_mode( const FSSpec& file )
	{
		CInfoPBRec cInfo;
		
		OSErr err = GetCatInfo( cInfo, file );
		
		Mac::ThrowOSStatus( err );
		
		if ( bool locked = cInfo.hFileInfo.ioFlAttrib & kioFlAttribLockedMask )
		{
			return S_IFREG | 0400;
		}
		
		return S_IFREG | 0600;
	}
	
	
	static void rsrcfile_stat( const vfs::node*  that,
	                           struct stat&      sb )
	{
		CInfoPBRec cInfo;
		
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		OSErr err = GetCatInfo( cInfo, fileSpec );
		
		Mac::ThrowOSStatus( err );
		
		Stat_HFS( &sb, cInfo, fileSpec.name, true );
	}
	
	static vfs::filehandle_ptr rsrcfile_open( const vfs::node* that, int flags, mode_t mode )
	{
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		return OpenMacFileHandle( fileSpec,
		                          flags,
		                          &mac::file::open_rsrc_fork,
		                          &GetRsrcForkFSTree );
	}
	
	static const vfs::data_method_set rsrcfile_data_methods =
	{
		&rsrcfile_open
	};
	
	static const vfs::item_method_set rsrcfile_item_methods =
	{
		&rsrcfile_stat,
	};
	
	static const vfs::node_method_set rsrcfile_methods =
	{
		&rsrcfile_item_methods,
		&rsrcfile_data_methods
	};
	
	
	vfs::node_ptr GetRsrcForkFSTree( const FSSpec& file )
	{
		vfs::node_ptr parent = FSTreeFromFSSpec( file );
		
		vfs::node* result = new vfs::node( parent.get(),
		                                   "rsrc",
		                                   file_mode( file ),
		                                   &rsrcfile_methods,
		                                   sizeof (FSSpec) );
		
		*(FSSpec*) result->extra() = file;
		
		return result;
	}
	
}
