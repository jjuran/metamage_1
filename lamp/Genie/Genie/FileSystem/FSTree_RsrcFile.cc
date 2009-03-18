/*	==================
 *	FSTree_RsrcFile.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_RsrcFile.hh"

// Genie
#include "Genie/FileSystem/FSSpec.hh"
#include "Genie/FileSystem/FSSpecForkUser.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	class FSTree_RsrcFile : public FSTree
	{
		private:
			FSSpec itsFileSpec;
		
		public:
			FSTree_RsrcFile( const FSSpec& file ) : FSTree( FSTreeFromFSSpec( file ), "rsrc" ),
			                                        itsFileSpec( file )
			{
			}
			
			void Stat( struct ::stat& sb ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return OpenMacFileHandle( itsFileSpec,
				                          flags,
				                          &Genie::FSpOpenRF,
				                          &New_RsrcForkHandle );
			}
	};
	
	
	FSTreePtr GetRsrcForkFSTree( const FSSpec& file )
	{
		return FSTreePtr( new FSTree_RsrcFile( file ) );
	}
	
	
	void FSTree_RsrcFile::Stat( struct ::stat& sb ) const
	{
		StatFile( itsFileSpec, &sb, true );
	}
	
}

