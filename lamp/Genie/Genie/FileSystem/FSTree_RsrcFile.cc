/*	==================
 *	FSTree_RsrcFile.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_RsrcFile.hh"

// POSIX
#include "fcntl.h"

// Nucleus
//#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	class FSTree_RsrcFile : public FSTree
	{
		private:
			FSSpec fileSpec;
		
		public:
			FSTree_RsrcFile( const FSSpec& file ) : fileSpec( file )  {}
			
			std::string Name() const  { return "rsrc"; }
			
			void Stat( struct ::stat& sb ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	FSTreePtr GetRsrcForkFSTree( const FSSpec& file )
	{
		return FSTreePtr( new FSTree_RsrcFile( file ) );
	}
	
	
	static boost::shared_ptr< RegularFileHandle > OpenFile( NN::Owned< N::FSFileRefNum > refNum )
	{
		return boost::shared_ptr< RegularFileHandle >( new RegularFileHandle( refNum ) );
	}
	
	static boost::shared_ptr< IOHandle > OpenFSSpec( const FSSpec& fileSpec, OpenFlags flags, bool rsrcFork )
	{
		N::FSIOPermissions rdPerm = N::FSIOPermissions( flags + 1  &  FREAD  );
		N::FSIOPermissions wrPerm = N::FSIOPermissions( flags + 1  &  FWRITE );
		
		bool nonblocking = flags & O_NONBLOCK;
		bool appending   = flags & O_APPEND;
		// ...
		bool creating    = flags & O_CREAT;
		bool truncating  = flags & O_TRUNC;
		bool excluding   = flags & O_EXCL;
		// ...
		bool resFork     = flags & O_ResFork;
		bool resMap      = flags & O_ResMap;
		
		NN::Owned< N::FSFileRefNum > fileH = rsrcFork ? N::FSpOpenRF( fileSpec, rdPerm | wrPerm )
		                                              : N::FSpOpenDF( fileSpec, rdPerm | wrPerm );
		
		if ( truncating )
		{
			N::SetEOF( fileH, 0 );
		}
		else if ( appending )
		{
			N::SetFPos( fileH, fsFromLEOF, 0 );
		}
		
		return OpenFile( fileH );
	}
	
	
	void FSTree_RsrcFile::Stat( struct ::stat& sb ) const
	{
		StatFile( fileSpec, &sb, true );
	}
	
	boost::shared_ptr< IOHandle > FSTree_RsrcFile::Open( OpenFlags flags ) const
	{
		const bool rsrcFork = true;
		
		return OpenFSSpec( fileSpec, flags, rsrcFork );
	}
	
}

