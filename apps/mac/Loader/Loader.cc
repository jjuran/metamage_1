/*
	Loader.cc
	---------
*/

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// more-libc
#include "more/string.h"

// mac-sys-utils
#include "mac_sys/beep.hh"
#include "mac_sys/has/virtualization.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"
#include "mac_qd/wide_drag_area.hh"

// mac-app-utils
#include "mac_app/documents.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"
#include "mac_app/state.hh"

// Loader
#include "files.hh"
#include "icon.hh"


using mac::qd::get_portRect;
using mac::qd::main_display_bounds;
using mac::qd::wide_drag_area;


Str15 FinderName : 0x02E0;

Handle AppParmHandle : 0x0AEC;

struct LaunchParams
{
	void* appName;
	short reserved2;
	// ...
};

#if TARGET_CPU_68K

static inline
short asm Launch( void* pb : __A0 )
{
	DC.W     0xA9F2  // _Launch
}

#endif

static Str255 app_name;
static Handle app_icon;

static WindowRef main_window;

static
void make_main_window()
{
	const short alignment = 16;
	
	Rect bounds = main_display_bounds();
	
	bounds.top    += 40;  // approximate menu bar height + title bar height
	bounds.bottom -=  8;  // approximate window frame thickness
	
	const short usableHeight = bounds.bottom - bounds.top;
	
	short height = usableHeight;
	
	height -= height /  4u;  // leave some space
	height -= height % 16u;  // make it an exact multiple
	
	const short margin = usableHeight - height;
	
	bounds.top   += margin / 2u;
	bounds.bottom = bounds.top + height;
	
	short screen_width = bounds.right - bounds.left;
	short window_width = screen_width * 5 / 8u;
	
	window_width -= window_width % 16u;
	
	bounds.left  = (screen_width - window_width) / 2u;
	bounds.right = bounds.left + window_width;
	
	main_window = NewWindow( NULL,
	                         &bounds,
	                         "\p" "Advanced Mac Substitute",
	                         true,
	                         noGrowDocProc,
	                         (WindowRef) -1,
	                         true,
	                         0 );
	
	SetPortWindowPort( main_window );
	
	TextFont( 3 );  // Geneva
	TextSize( 9 );
}

static
void draw_window( WindowRef window )
{
	SetPortWindowPort( window );
	
	const Rect& portRect = get_portRect( window );
	
	const short v_center = (portRect.bottom - portRect.top) / 2u;
	const short h_center = (portRect.right - portRect.left) / 2u;
	
	Rect icon_rect =
	{
		v_center - 32, h_center - 16,
		v_center,      h_center + 16,
	};
	
	if ( app_icon )
	{
		PlotIcon( &icon_rect, app_icon );
	}
	
	const short width = StringWidth( app_name );
	
	MoveTo( h_center - width / 2u, v_center + 10 );
	
	DrawString( app_name );
}

static
void menu_item_chosen( long choice )
{
	short menu = choice >> 16;
	short item = choice;
	
	switch ( menu )
	{
		case 1:  // (Apple)
			mac::sys::beep();
			break;
		
		case 2:  // File
			switch ( item )
			{
				case 1:  // New
				case 2:  // Open
					break;
				
				case 3:  // Close
				case 4:  // -
				case 5:  // Quit
					mac::app::quitting = true;
					break;
				
				default:
					break;
			}
		
		case 3:  // Edit
			break;
		
		default:
			break;
	}
	
	HiliteMenu( 0 );
}

static
long HFS_file_opener( short vRefNum, long dirID, const Byte* name )
{
	BlockMoveData( name, app_name, 1 + name[ 0 ] );
	
	return noErr;
}

int main()
{
	using mac::app::open_documents_with;
	using mac::app::quitting;
	
	const bool in_AMS = mac::sys::has_v68k();
	
	if ( in_AMS )
	{
		/*
			If a debugger makes us ExitToShell(),
			exit instead of relaunching.
		*/
		
		FinderName[ 0 ] = '\0';
	}
	
	mac::app::init_toolbox();
	mac::app::install_menus();
	
	/*
		Crawl AppParmHandle to see if we should display
		a specific application.  As we go, we clear the
		file type of each entry (via ClrAppFiles()), but
		the stale entry still confuses System's Twilight.
		So after we do that, clear the count as well.
	*/
	
	mac::app::open_AppFiles_with( &HFS_file_opener );
	
	*(long*) *AppParmHandle = 0;
	
	if ( ! *app_name )
	{
		find_appl( app_name );
	}
	
	if ( *app_name )
	{
		app_icon = get_icon( app_name );
	}
	
	make_main_window();
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, NULL ) )
		{
			WindowRef window;
			
			switch ( event.what )
			{
				case mouseDown:
					switch ( FindWindow( event.where, &window ) )
					{
						case inMenuBar:
							menu_item_chosen( MenuSelect( event.where ) );
							break;
						
						case inDrag:
							DragWindow( window, event.where, wide_drag_area() );
							break;
						
						case inGoAway:
							if ( TrackGoAway( window, event.where ) )
							{
								quitting = true;
							}
							break;
						
						default:
							break;
					}
					break;
				
				case keyDown:
					if ( event.modifiers & cmdKey )
					{
						menu_item_chosen( MenuKey( event.message ) );
						break;
					}
					
					switch ( (Byte) event.message )
					{
						case kReturnCharCode:
							if ( in_AMS  &&  *app_name )
							{
								LaunchParams params = { app_name };
								
							#if TARGET_CPU_68K
								
								mempcpy( FinderName, "\p" "Loader", 7 );
								
								Launch( &params );
								
							#endif
							}
							break;
						
						default:
							break;
					}
					
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
					BeginUpdate( window );
					draw_window( window );
					EndUpdate  ( window );
					break;
				
				default:
					break;
			}
		}
	}
	
	return 0;
}
