/*	===========
 *	Io/Files.hh
 *	===========
 */

#ifndef IO_FILES_HH
#define IO_FILES_HH

// Nitrogen
#include "Nitrogen/Files.h"


namespace Io
{
	
	namespace N = Nitrogen;
	
	inline int Read ( N::FSFileRefNum fRefNum,       char* data, std::size_t byteCount )
	{
		return N::FSRead( fRefNum, byteCount, data );
	}
	
	inline int Write( N::FSFileRefNum fRefNum, const char* data, std::size_t byteCount )
	{
		return N::FSWrite( fRefNum, byteCount, data );
	}
	
	inline int Read ( N::FSForkRefNum fRefNum,       char* data, std::size_t byteCount )
	{
		return N::FSReadFork( fRefNum, byteCount, data );
	}
	
	inline int Write( N::FSForkRefNum fRefNum, const char* data, std::size_t byteCount )
	{
		return N::FSWriteFork( fRefNum, byteCount, data );
	}
	
	
	template < class FileHandle >
	inline int Write( FileHandle fileH, const std::string& s )
	{
		return Write( fileH, s.data(), s.size() );
	}
	
	
	template < class FileHandle >
	struct File_Details
	{
		static void SetBlocking( FileHandle fRefNum, bool blocking )
		{
			
		}
		
		static int Read( FileHandle   fRefNum,
		                 char*        data,
		                 std::size_t  byteCount )
		{
			return Io::Read( fRefNum, data, byteCount );
		}
		
		static int Write( FileHandle   fRefNum,
		                  const char*  data,
		                  std::size_t  byteCount )
		{
			return Io::Write( fRefNum, data, byteCount );
		}
	};
	
	typedef File_Details< N::FSFileRefNum > FileRefNum_Details;
	typedef File_Details< N::FSForkRefNum > ForkRefNum_Details;
	
}

#endif

