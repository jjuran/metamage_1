/*	=============
 *	FileHandle.hh
 *	=============
 */

#ifndef GENIE_FILEHANDLE_HH
#define GENIE_FILEHANDLE_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/IORef.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	IORef OpenFile( N::Owned< N::FSFileRefNum > refNum );
	
	IORef OpenFile( const FSSpec& file, N::FSIOPermissions perm );
	
	class FileHandle
	{
		private:
			N::Owned< N::FSFileRefNum > fRefNum;
		
		public:
			FileHandle( N::Owned< N::FSFileRefNum > refNum )
			:
				fRefNum( refNum )
			{}
			
			FileHandle( const FSSpec& file, N::FSIOPermissions perm );
			
			int Read( char* data, std::size_t byteCount );
			
			int Write( const char* data, std::size_t byteCount );
	};
	
}

#endif

