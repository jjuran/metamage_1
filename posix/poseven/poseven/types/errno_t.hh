// errno_t.hh
// ----------

// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_ERRNO_T_HH
#define POSEVEN_TYPES_ERRNO_T_HH

// Standard C
#include <errno.h>


namespace poseven
{
	
	class errno_t
	{
		private:
			int itsNumber;
		
		public:
			typedef int ErrorNumber;
			
			errno_t(       ) : itsNumber( 0 )  {}
			errno_t( int n ) : itsNumber( n )  {}
			
			static errno_t Make( int n )    { return errno_t( n ); }
			
			int Get() const               { return itsNumber; }
			operator int() const          { return itsNumber; }
	};
	
	void throw_errno_internal( errno_t number );
	
	inline void throw_errno( int error )
	{
		if ( error != 0 )
		{
			throw_errno_internal( error );
		}
	}
	
	inline int throw_posix_result( int result )
	{
		if ( result == -1 )
		{
			throw_errno_internal( errno );
		}
		
		return result;
	}
	
}

#endif

