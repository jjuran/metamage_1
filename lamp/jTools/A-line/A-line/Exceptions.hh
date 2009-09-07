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
	
	struct missing_project_config
	{
		std::string project_name;
		std::string config_pathname;
		
		missing_project_config( const std::string& name,
		                        const std::string& path )
		: project_name   ( name ),
		  config_pathname( path )
		{
		}
	};
	
	struct circular_dependency
	{
		std::string project_name;
		
		circular_dependency( const std::string& name )
		:
			project_name( name )
		{
		}
	};
	
}

#endif

