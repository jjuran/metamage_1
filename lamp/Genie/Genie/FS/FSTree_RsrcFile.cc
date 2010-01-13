/*	==================
 *	FSTree_RsrcFile.cc
 *	==================
 */

#include "Genie/FS/FSTree_RsrcFile.hh"

// POSIX
#include <fcntl.h>

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSSpecForkUser.hh"
#include "Genie/FS/StatFile.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	class FSTree_RsrcFile : public FSTree
	{
		private:
			FSSpec  itsFileSpec;
			bool    itIsOnServer;
		
		public:
			FSTree_RsrcFile( const FSSpec& file, bool onServer )
			:
				FSTree( FSTreeFromFSSpec( file, onServer ), "rsrc" ),
				itsFileSpec( file ),
				itIsOnServer( onServer )
			{
			}
			
			void Stat( struct ::stat& sb ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				flags |= itIsOnServer ? O_MAC_ASYNC : 0;
				
				return OpenMacFileHandle( itsFileSpec,
				                          flags,
				                          &Genie::FSpOpenRF,
				                          &New_RsrcForkHandle );
			}
	};
	
	
	FSTreePtr GetRsrcForkFSTree( const FSSpec& file, bool onServer )
	{
		return seize_ptr( new FSTree_RsrcFile( file, onServer ) );
	}
	
	
	void FSTree_RsrcFile::Stat( struct ::stat& sb ) const
	{
		StatFile( itIsOnServer, itsFileSpec, &sb, true );
	}
	
}

