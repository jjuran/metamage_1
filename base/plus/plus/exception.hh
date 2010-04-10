/*
	exception.hh
	------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PLUS_EXCEPTION_HH
#define PLUS_EXCEPTION_HH

// Standard C++
#include <exception>


namespace plus
{
	
	class exception : public std::exception
	{
		private:
			char buffer[ 256 ];
		
		public:
			exception( const char* s, unsigned length );
			exception( const char* s );
			
			const char* what() const throw()  { return buffer; }
	};
	
}

#endif

