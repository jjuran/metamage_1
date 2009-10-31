/*
	FSReader.hh
	-----------
	
	Written 2009 by Joshua Juran.
	
	This code was written entirely by the above contributor, who places it
	in the public domain.
*/


#ifndef FSREADER_HH
#define FSREADER_HH

// Standard C++
#include <functional>

// Nitrogen
#include "Nitrogen/Files.h"


namespace Nitrogen
{
	
	class FSReader : public std::binary_function< char*, size_t, Size >
	{
		private:
			FSFileRefNum its_fRefNum;
		
		public:
			FSReader( FSFileRefNum fRefNum ) : its_fRefNum( fRefNum )
			{
			}
			
			Size operator()( char* buffer, size_t length ) const
			{
				return FSRead( its_fRefNum, length, buffer, ThrowEOF_Never() );
			}
	};
	
}

#endif

