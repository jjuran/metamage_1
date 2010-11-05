/*
	FSReader.hh
	-----------
	
	Written 2009 by Joshua Juran.
	
	This code was written entirely by the above contributor, who places it
	in the public domain.
*/


#ifndef FSREADER_HH
#define FSREADER_HH

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif


namespace Nitrogen
{
	
	class FSReader
	{
		private:
			FSFileRefNum its_fRefNum;
		
		public:
			typedef char*        first_argument_type;
			typedef std::size_t  second_argument_type;
			typedef std::size_t  result_type;
			
			FSReader( FSFileRefNum fRefNum ) : its_fRefNum( fRefNum )
			{
			}
			
			std::size_t operator()( char* buffer, std::size_t length ) const
			{
				return FSRead( its_fRefNum, length, buffer, ThrowEOF_Never() );
			}
	};
	
}

#endif

