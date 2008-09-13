/*	=============
 *	Exceptions.hh
 *	=============
 */

#pragma once

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

