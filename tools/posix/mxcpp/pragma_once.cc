/*
	pragma_once.cc
	--------------
*/

#include "pragma_once.hh"

// mxcpp
#include "exception.hh"
#include "include.hh"
#include "pragma.hh"


namespace tool
{
	
	static bool globally_including_once = false;
	
	
	bool including_once()
	{
		return globally_including_once;
	}
	
	void pragma_once( const plus::string& param )
	{
		if ( param.empty() )
		{
			mark_current_source_once_included();
		}
		else if ( param == "on" )
		{
			globally_including_once = true;
		}
		else if ( param == "off" )
		{
			globally_including_once = false;
		}
		else
		{
			throw exception( param );
		}
	}
	
}
