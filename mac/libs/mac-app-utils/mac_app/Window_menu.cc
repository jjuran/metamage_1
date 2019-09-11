/*
	Window_menu.cc
	--------------
*/

#include "mac_app/Window_menu.hh"

// Standard  C++
#include <algorithm>
#include <functional>
#include <vector>

// mac-config
#include "mac_config/desk-accessories.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"


namespace mac {
namespace app {
	
	static std::vector< void* > the_windows_in_menu;
	
	
	static bool window_title_less( const void* a_, const void* b_ )
	{
		const unsigned char* a;
		const unsigned char* b;
		
	#if ! OPAQUE_TOOLBOX_STRUCTS
		
		a = *((WindowPeek) a_)->titleHandle;
		b = *((WindowPeek) b_)->titleHandle;
		
	#else
		
		Str255 one;
		Str255 two;
		
		a = one;
		b = two;
		
		GetWTitle( (WindowRef) a_, one );
		GetWTitle( (WindowRef) b_, two );
		
	#endif
		
		const uint8_t a_len = *a++;
		const uint8_t b_len = *b++;
		
		const unsigned char* a_end = a + a_len;
		const unsigned char* b_end = b + b_len;
		
		return std::lexicographical_compare( a, a_end, b, b_end );
	}
	
	static
	void enumerate_windows()
	{
		WindowRef window = FrontWindow();
		
		for ( ;  window != NULL;  window = GetNextWindow( window ) )
		{
			if ( CONFIG_DESK_ACCESSORIES  &&  GetWindowKind( window ) < 0 )
			{
				continue;
			}
			
		#if OPAQUE_TOOLBOX_STRUCTS
			
			Str255 title;
			GetWTitle( window, title );
			
		#else
			
			ConstStr255Param title = *((WindowPeek) window)->titleHandle;
			
		#endif
			
			if ( title[ 0 ] == 0 )
			{
				continue;
			}
			
			the_windows_in_menu.push_back( window );
		}
	}
	
	void populate_Window_menu( MenuRef menu )
	{
		using mac::ui::delete_all_menu_items;
		
		delete_all_menu_items( menu );
		
		the_windows_in_menu.clear();
		
		enumerate_windows();
		
		const int n = (int) the_windows_in_menu.size();
		
		if ( n == 0 )
		{
			return;
		}
		
		std::stable_sort( the_windows_in_menu.begin(),
		                  the_windows_in_menu.end(),
		                  std::ptr_fun( window_title_less ) );
		
		WindowRef front = FrontWindow();
		
		Str255 title;
		
		for ( int i = 0;  i < n;  ++i )
		{
			WindowRef w = (WindowRef) the_windows_in_menu[ i ];
			
			GetWTitle( w, title );
			
			AppendMenu( menu, "\p " );
			
			SetMenuItemText( menu, i + 1, title );
			
			if ( w == front )
			{
				CheckMenuItem( menu, i + 1, true );
			}
		}
	}
	
	WindowRef get_nth_window( int i )
	{
		if ( (unsigned) i >= the_windows_in_menu.size() )
		{
			return NULL;
		}
		
		return (WindowRef) the_windows_in_menu[ i ];
	}
	
}
}
