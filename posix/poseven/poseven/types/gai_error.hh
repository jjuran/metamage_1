/*
	gai_error.hh
	------------
*/

#ifndef POSEVEN_TYPES_GAI_ERROR_HH
#define POSEVEN_TYPES_GAI_ERROR_HH


namespace poseven
{
	
	class gai_error
	{
		private:
			int its_number;
		
		public:
			typedef int error_number;
			
			gai_error(       ) : its_number( 0 )  {}
			gai_error( int n ) : its_number( n )  {}
			
			int get() const               { return its_number; }
			operator int() const          { return its_number; }
	};
	
	inline
	void throw_gai_error( int error )
	{
		if ( error != 0 )
		{
			throw gai_error( error );
		}
	}
	
}

#endif
