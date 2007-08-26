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
	
#if defined( __GNUC__ ) || defined( __MACH__ )
	
	typedef char       *const argv_t[];
	
#else
	
	typedef char const *const argv_t[];
	
#endif
	
	int Main( int argc, argv_t argv );
	
}

#endif

