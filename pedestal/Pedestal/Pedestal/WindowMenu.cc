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
	
	static void delete_all_menu_items( MenuRef menu )
	{
		uint16_t n = CountMenuItems( menu );
		
		while ( n > 0 )
		{
			DeleteMenuItem( menu, n );
			
			--n;
		}
	}
	
	void populate_Window_menu( MenuRef menu )
	{
		delete_all_menu_items( menu );
		
		the_windows_in_menu = the_windows;
		
		const std::size_t n = the_windows.size();
		
		Str255 title;
		
		for ( int i = 0;  i < n;  ++i )
		{
			WindowRef w = (WindowRef) the_windows_in_menu[ i ];
			
			GetWTitle( w, title );
			
			InsertMenuItem( menu, "\p ", i );
			
			if ( title[ 0 ] > 0 )
			{
				SetMenuItemText( menu, i + 1, title );
			}
			
			if ( w == FrontWindow() )
			{
				CheckMenuItem( menu, i + 1, true );
			}
		}
	}
	
	WindowRef get_nth_window( int i )
	{
		return (WindowRef) the_windows_in_menu.at( i );
	}
	
}
