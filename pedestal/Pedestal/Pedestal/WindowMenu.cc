/*
	WindowMenu.cc
	-------------
*/

#include "Pedestal/WindowMenu.hh"

// Standard  C++
#include <algorithm>
#include <functional>
#include <vector>

// mac-ui-utils
#include "mac_ui/menus.hh"


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
	
	static bool window_title_less( const void* a_, const void* b_ )
	{
		Str255 one;
		Str255 two;
		
		GetWTitle( (WindowRef) a_, one );
		GetWTitle( (WindowRef) b_, two );
		
		const unsigned char* a = one + 1;
		const unsigned char* b = two + 1;
		
		const unsigned char* a_end = a + one[ 0 ];
		const unsigned char* b_end = b + two[ 0 ];
		
		return std::lexicographical_compare( a, a_end, b, b_end );
	}
	
	void populate_Window_menu( MenuRef menu )
	{
		using mac::ui::delete_all_menu_items;
		
		delete_all_menu_items( menu );
		
		the_windows_in_menu = the_windows;
		
		std::stable_sort( the_windows_in_menu.begin(),
		                  the_windows_in_menu.end(),
		                  std::ptr_fun( window_title_less ) );
		
		const std::size_t n = the_windows.size();
		
		Str255 title;
		
		for ( int i = 0;  i < n;  ++i )
		{
			WindowRef w = (WindowRef) the_windows_in_menu[ i ];
			
			GetWTitle( w, title );
			
			AppendMenu( menu, "\p " );
			
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
