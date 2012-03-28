/*	==================
 *	FSTree_RsrcFile.cc
 *	==================
 */

#include "Genie/FS/FSTree_RsrcFile.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSSpecForkUser.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/StatFile.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	static mode_t file_mode( const FSSpec& file )
	{
		CInfoPBRec cInfo;
		
		const bool async = false;
		
		FSpGetCatInfo< FNF_Throws >( cInfo, async, file );
		
		if ( bool locked = cInfo.hFileInfo.ioFlAttrib & kioFlAttribLockedMask )
		{
			return S_IFREG | 0400;
		}
		
		return S_IFREG | 0600;
	}
	
	
	static void rsrcfile_stat( const FSTree*   node,
	                           struct ::stat&  sb )
	{
		CInfoPBRec cInfo = { 0 };
		
		const bool async = false;
		
		const FSSpec& fileSpec = *(FSSpec*) node->extra();
		
		FSpGetCatInfo< FNF_Throws >( cInfo,
		                             async,
		                             fileSpec );
		
		Stat_HFS( async, &sb, cInfo, fileSpec.name, true );
	}
	
	static IOPtr rsrcfile_open( const FSTree* node, int flags, mode_t mode )
	{
		const FSSpec& fileSpec = *(FSSpec*) node->extra();
		
		return OpenMacFileHandle( fileSpec,
		                          flags,
		                          &Genie::FSpOpenRF,
		                          &New_RsrcForkHandle );
	}
	
	static const data_method_set rsrcfile_data_methods =
	{
		&rsrcfile_open
	};
	
	static node_method_set rsrcfile_methods =
	{
		&rsrcfile_stat,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&rsrcfile_data_methods
	};
	
	
	FSTreePtr GetRsrcForkFSTree( const FSSpec& file )
	{
		FSTreePtr parent = FSTreeFromFSSpec( file );
		
		FSTree* result = new FSTree( parent,
		                             "rsrc",
		                             file_mode( file ),
		                             &rsrcfile_methods,
		                             sizeof (FSSpec) );
		
		*(FSSpec*) result->extra() = file;
		
		return result;
	}
	
}

