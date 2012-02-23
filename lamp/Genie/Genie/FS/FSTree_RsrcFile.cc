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
	
	static mode_t file_mode( const FSSpec& file, bool async )
	{
		CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Throws >( cInfo, async, file );
		
		if ( bool locked = cInfo.hFileInfo.ioFlAttrib & kioFlAttribLockedMask )
		{
			return S_IFREG | 0400;
		}
		
		return S_IFREG | 0600;
	}
	
	class FSTree_RsrcFile : public FSTree
	{
		private:
			FSSpec  itsFileSpec;
			bool    itIsOnServer;
		
		public:
			FSTree_RsrcFile( const FSSpec& file, bool onServer );
			
			void Stat( struct ::stat& sb ) const;
			
			IOPtr Open( OpenFlags flags, mode_t mode ) const;
	};
	
	
	static void rsrcfile_stat( const FSTree*   node,
	                           struct ::stat&  sb )
	{
		const FSTree_RsrcFile* file = static_cast< const FSTree_RsrcFile* >( node );
		
		file->Stat( sb );
	}
	
	static IOPtr rsrcfile_open( const FSTree* node, int flags, mode_t mode )
	{
		const FSTree_RsrcFile* file = static_cast< const FSTree_RsrcFile* >( node );
		
		return file->Open( flags, mode );
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
	
	
	FSTree_RsrcFile::FSTree_RsrcFile( const FSSpec& file, bool onServer )
	:
		FSTree( FSTreeFromFSSpec( file, onServer ),
		        "rsrc",
		        file_mode( file, onServer ),
		        &rsrcfile_methods ),
		itsFileSpec( file ),
		itIsOnServer( onServer )
	{
	}
	
	
	FSTreePtr GetRsrcForkFSTree( const FSSpec& file, bool onServer )
	{
		return new FSTree_RsrcFile( file, onServer );
	}
	
	
	void FSTree_RsrcFile::Stat( struct ::stat& sb ) const
	{
		CInfoPBRec cInfo = { 0 };
		
		FSpGetCatInfo< FNF_Throws >( cInfo,
		                             itIsOnServer,
		                             itsFileSpec );
		
		Stat_HFS( itIsOnServer, &sb, cInfo, itsFileSpec.name, true );
	}
	
	IOPtr FSTree_RsrcFile::Open( OpenFlags flags, mode_t mode ) const
	{
		flags |= itIsOnServer ? O_MAC_ASYNC : 0;
		
		return OpenMacFileHandle( itsFileSpec,
		                          flags,
		                          &Genie::FSpOpenRF,
		                          &New_RsrcForkHandle );
	}
	
}

