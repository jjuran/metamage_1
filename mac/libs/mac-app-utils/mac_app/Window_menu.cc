/*
	Window_menu.cc
	--------------
*/

#include "mac_app/Window_menu.hh"

// Mac OS
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// Standard  C++
#include <algorithm>
#include <functional>

// mac-ui-utils
#include "mac_ui/menus.hh"


#pragma exceptions off


namespace mac {
namespace app {

static inline
size_t byte_distance( const void* begin, const void* end )
{
	return (const char*) end - (const char*) begin;
}

template < class T >
class Handle_based_vector
{
	private:
		Handle its_handle;
		size_t its_size;
		
		Handle_based_vector           ( const Handle_based_vector& );
		Handle_based_vector& operator=( const Handle_based_vector& );
	
	public:
		typedef       T*       iterator;
		typedef const T* const_iterator;
		
		Handle_based_vector() : its_handle( NewHandle( 0 ) ), its_size()
		{
		}
		
		/*
			The destructor is commented out because we don't need it in this
			case.  The handle will persist for the life of the application.
		
		~Handle_based_vector()
		{
			DisposeHandle( its_handle );
		}
		*/
		
		size_t size() const  { return its_size; }
		
		const_iterator begin() const
		{
			return (const T*) *its_handle;
		}
		
		const_iterator end() const
		{
			return begin() + size();
		}
		
		T operator[]( size_t i ) const
		{
			return i < its_size ? begin()[ i ] : NULL;
		}
		
		iterator begin()
		{
			return (T*) *its_handle;
		}
		
		iterator end()
		{
			return begin() + size();
		}
		
		void insert( iterator it, T x )
		{
			long d = byte_distance( begin(), it );
			
			Munger( its_handle, d, NULL, 0, &x, sizeof x );
			
			++its_size;
		}
		
		void erase( iterator it )
		{
			long d = byte_distance( begin(), it );
			
			Munger( its_handle, d, NULL, sizeof (T), "", 0 );
			
			--its_size;
		}
};

typedef Handle_based_vector< void* > vector_of_windows;

static vector_of_windows the_windows_in_menu;


static
bool has_empty_title( WindowRef window )
{
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	const unsigned char* title = *((WindowPeek) window)->titleHandle;
	
#else
	
	Str255 title;
	
	GetWTitle( window, title );
	
#endif
	
	return title[ 0 ] == 0;
}

static
bool window_title_less( const void* a_, const void* b_ )
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

void Window_menu_insert( WindowRef window )
{
	if ( has_empty_title( window ) )
	{
		return;
	}
	
	typedef vector_of_windows::iterator Iter;
	
	Iter it = std::lower_bound( the_windows_in_menu.begin(),
	                            the_windows_in_menu.end(),
	                            window,
	                            std::ptr_fun( window_title_less ) );
	
	the_windows_in_menu.insert( it, window );
}

void Window_menu_remove( WindowRef window )
{
	typedef vector_of_windows::iterator Iter;
	
	Iter end = the_windows_in_menu.end();
	Iter it  = the_windows_in_menu.begin();
	
	while ( it != end )
	{
		if ( *it == window )
		{
			the_windows_in_menu.erase( it );
			break;
		}
		
		++it;
	}
}

void populate_Window_menu( MenuRef menu )
{
	using mac::ui::delete_all_menu_items;
	
	delete_all_menu_items( menu );
	
	const int n = (int) the_windows_in_menu.size();
	
	if ( n == 0 )
	{
		return;
	}
	
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
