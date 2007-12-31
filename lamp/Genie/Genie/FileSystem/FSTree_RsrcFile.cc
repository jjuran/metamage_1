/*	==================
 *	FSTree_RsrcFile.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_RsrcFile.hh"

// Genie
#include "Genie/FileSystem/FSSpecForkUser.hh"
#include "Genie/FileSystem/StatFile.hh"


namespace Genie
{
	
	class FSTree_RsrcFile : public FSTree
	{
		private:
			FSSpec itsFileSpec;
		
		public:
			FSTree_RsrcFile( const FSSpec& file ) : itsFileSpec( file )  {}
			
			std::string Name() const  { return "rsrc"; }
			
			FSTreePtr Parent() const  { return FSTreeFromFSSpec( itsFileSpec ); }
			
			void Stat( struct ::stat& sb ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return ResourceForkUser().OpenFileHandle( itsFileSpec, flags );
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

