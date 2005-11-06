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
	
	class FileHandle : public IOStream
	{
		private:
			N::Owned< N::FSFileRefNum > fRefNum;
		
		public:
			FileHandle( N::Owned< N::FSFileRefNum > refNum )
			:
				fRefNum( refNum )
			{}
			
			FileHandle( const FSSpec& file, N::FSIOPermissions perm );
			
			N::FSFileRefNum GetRefNum() const  { return fRefNum; }
			
			unsigned int SysPoll() const  { return kPollRead | kPollWrite | kPollExcept; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
	};
	
}

#endif

