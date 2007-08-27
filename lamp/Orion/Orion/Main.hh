/*	=======
 *	Main.hh
 *	=======
 */

#ifndef ORION_MAIN_HH
#define ORION_MAIN_HH

#include "iota/argv.hh"


namespace Orion
{
	
	class ExitStatus
	{
		private:
			int status;
		
		public:
			ExitStatus( int status ) : status( status )  {}
			
			int Get() const  { return status; }
			
			operator int() const  { return Get(); }
	};
	
	inline void ThrowExitStatus( int status )  { throw ExitStatus( status ); }
	
	using iota::argv_t;
	
	int Main( int argc, argv_t argv );
	
}

#endif

