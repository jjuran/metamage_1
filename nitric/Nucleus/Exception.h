// Nucleus/Exception.h
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_EXCEPTION_H
#define NUCLEUS_EXCEPTION_H

// Standard C++
#include <exception>
#include <string>


namespace Nucleus
{
	
	class Exception : public std::exception
	{
		private:
			std::string itsDescription;
		
		public:
			Exception()  {}
			
			Exception( const std::string& description ) : itsDescription( description )
			{
				(void) itsDescription.c_str();  // make sure c_str() is safe to call
			}
			
			~Exception() throw()  {}
			
			const char* what() const throw()  { return itsDescription.c_str(); }
	};
	
}

#endif

