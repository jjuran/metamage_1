/*	=======
 *	Main.hh
 *	=======
 */

#ifndef ORION_MAIN_HH
#define ORION_MAIN_HH


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
	
	int Main( int argc, const char *const argv[] );
	
}

#endif

