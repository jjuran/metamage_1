/*
	
	CurrentA4.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_CURRENTA4_HH
#define SILVER_CURRENTA4_HH

// CodeWarrior Mac OS Support
#include <A4Stuff.h>


namespace Silver
{

	class CurrentA4
	{
		private:
			long oldA4;
			
			// Non-copyable
			CurrentA4           ( const CurrentA4& );
			CurrentA4& operator=( const CurrentA4& );
		
		public:
			CurrentA4() : oldA4( ::SetCurrentA4() )  {}
			
			~CurrentA4()  { ::SetA4( oldA4 ); }
	};
	
}

#endif

