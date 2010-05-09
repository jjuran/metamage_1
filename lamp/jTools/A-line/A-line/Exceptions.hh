/*	=============
 *	Exceptions.hh
 *	=============
 */

#ifndef ALINE_EXCEPTIONS_HH
#define ALINE_EXCEPTIONS_HH

// plus
#include "plus/string.hh"


namespace tool
{
	
	struct NoSuchUsedProject
	{
		NoSuchUsedProject( const plus::string& projName, const plus::string& used )
		:
			projName( projName ), 
			used    ( used )
		{}
		
		plus::string  projName;
		plus::string  used;
	};
	
	struct missing_project_config
	{
		plus::string  project_name;
		plus::string  config_pathname;
		
		missing_project_config( const plus::string&  name,
		                        const plus::string&  path )
		: project_name   ( name ),
		  config_pathname( path )
		{
		}
	};
	
	struct circular_dependency
	{
		plus::string project_name;
		
		circular_dependency( const plus::string& name )
		:
			project_name( name )
		{
		}
	};
	
}

#endif

