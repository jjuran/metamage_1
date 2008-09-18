/*	=============
 *	Exceptions.hh
 *	=============
 */

#ifndef ALINE_EXCEPTIONS_HH
#define ALINE_EXCEPTIONS_HH

// Standard C++
#include <string>


namespace tool
{
	
	struct NoSuchUsedProject
	{
		NoSuchUsedProject( const std::string& projName, const std::string& used )
		:
			projName( projName ), 
			used    ( used )
		{}
		
		std::string projName;
		std::string used;
	};
	
}

#endif

