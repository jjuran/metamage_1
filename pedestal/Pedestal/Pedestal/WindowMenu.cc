/*
	WindowMenu.cc
	-------------
*/

#include "Pedestal/WindowMenu.hh"

// Standard  C++
#include <algorithm>
#include <vector>


namespace Pedestal
{
	
	static std::vector< void* > the_windows;
	static std::vector< void* > the_windows_in_menu;
	
	
	void window_created( WindowRef w )
	{
		typedef std::vector< void* >::const_iterator Iter;
		
		Iter begin = the_windows.begin();
		Iter end   = the_windows.end  ();
		
		if ( std::find( begin, end, w ) == end )
		{
			the_windows.push_back( w );
		}
	}
	
	void window_removed( WindowRef w )
	{
		typedef std::vector< void* >::iterator Iter;
		
		Iter begin = the_windows.begin();
		Iter end   = the_windows.end  ();
		
		Iter it = std::find( begin, end, w );
		
		if ( it != end )
		{
			the_windows.erase( it );
		}
	}
	
}
