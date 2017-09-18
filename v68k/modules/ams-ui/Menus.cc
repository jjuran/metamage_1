/*
	Menus.cc
	--------
*/

#include "Menus.hh"

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>

// ams-common
#include "QDGlobals.hh"
#include "raster_lock.hh"

// ams-core
#include "MBDF.hh"
#include "MDEF.hh"
#include "Windows.hh"


GrafPtr WMgrPort : 0x09DE;
short MBarHeight : 0x0BAA;


struct MenuList_header
{
	UInt16 extent_bytes;
	SInt16 right_edge;
	SInt16 unused;
};

struct MenuList_entry
{
	MenuRef  menu;
	SInt16   left_edge;
};

static short TheMenu;

static MenuList_header** MenuList;

const short system_font_ascent =  9;
const short menu_title_padding =  9;
const short gap_between_titles = 13;  // space between text of adjacent titles


class WMgrPort_bezel_scope
{
	private:
		GrafPtr oldPort;
		
		// non-copyable
		WMgrPort_bezel_scope           ( WMgrPort_bezel_scope& );
		WMgrPort_bezel_scope& operator=( WMgrPort_bezel_scope& );
	
	public:
		WMgrPort_bezel_scope();
		~WMgrPort_bezel_scope();
};

inline
WMgrPort_bezel_scope::WMgrPort_bezel_scope()
{
	GrafPtr& thePort = *get_addrof_thePort();
	
	oldPort = thePort;
	thePort = WMgrPort;
	
	SetClip( BezelRgn );
}

inline
WMgrPort_bezel_scope::~WMgrPort_bezel_scope()
{
	*get_addrof_thePort() = oldPort;
}


static
MenuList_entry* find_menu_id( short menuID )
{
	if ( menuID == 0 )
	{
		return 0;
	}
	
	const UInt16 extent_bytes = MenuList[0]->extent_bytes;
	
	MenuList_entry* last = (MenuList_entry*) ((char*) *MenuList + extent_bytes);
	MenuList_entry* next = (MenuList_entry*) *MenuList;
	
	while ( ++next <= last )
	{
		if ( next->menu[0]->menuID == menuID )
		{
			return next;
		}
	}
	
	return NULL;
}

static
MenuList_entry* find_clicked_menu_title( Point pt )
{
	if ( pt.h > MenuList[0]->right_edge )
	{
		return NULL;
	}
	
	const UInt16 extent_bytes = MenuList[0]->extent_bytes;
	
	MenuList_entry* last = (MenuList_entry*) ((char*) *MenuList + extent_bytes);
	MenuList_entry* next = (MenuList_entry*) *MenuList;
	MenuList_entry* prev = NULL;
	
	while ( ++next <= last )
	{
		if ( pt.h < next->left_edge )
		{
			break;
		}
		
		prev = next;
	}
	
	return prev;
}

#pragma mark -
#pragma mark Initialization and Allocation
#pragma mark -

pascal void InitMenus_patch()
{
	calculate_menu_bar_height();
	
	draw_menu_bar_from_WMgr_port();
	
	MenuList = (MenuList_header**) NewHandleClear( sizeof (MenuList_header) );
	
	MenuList[0]->right_edge = 10;
}

pascal MenuInfo** GetMenu_patch( short resID )
{
	Handle h = GetResource( 'MENU', resID );
	
	return (MenuRef) h;
}

#pragma mark -
#pragma mark Forming the Menus
#pragma mark -

pascal void AppendMenu_patch( MenuInfo** menu, const unsigned char* format )
{
	WMgrPort_bezel_scope port_swap;
	
	const Size oldSize = GetHandleSize( (Handle) menu );
	
	// TODO:  Parse the format string
	const Size newSize = oldSize + 1 + format[ 0 ] + 4;
	
	SetHandleSize( (Handle) menu, newSize );
	
	unsigned char* p = (unsigned char*) *menu + oldSize - 1;
	
	memcpy( p, format, 1 + format[ 0 ] );
	
	p += 1 + format[ 0 ];
	
	*p++ = 0;  // icon
	*p++ = 0;  // key
	*p++ = 0;  // mark
	*p++ = 0;  // style
	
	*p = 0;  // terminating empty string
	
	MDEF_0( mSizeMsg, menu, NULL, Point(), NULL );
}

pascal void AddResMenu_patch( MenuInfo** menu, ResType type )
{
}

#pragma mark -
#pragma mark Forming the Menu Bar
#pragma mark -

pascal void InsertMenu_patch( MenuInfo** menu, short beforeID )
{
	// TODO:  Respect beforeID
	
	WMgrPort_bezel_scope port_swap;
	
	const UInt16 extent_bytes = MenuList[0]->extent_bytes;
	
	const Size new_size = sizeof (MenuList_header)
	                    + extent_bytes
	                    + sizeof (MenuList_entry);
	
	SetHandleSize( (Handle) MenuList, new_size );
	
	MenuList_header* header = *MenuList;
	
	MenuList_entry* last = (MenuList_entry*) ((char*) *MenuList + extent_bytes);
	MenuList_entry* next = last + 1;
	
	next->menu = menu;
	next->left_edge = header->right_edge;
	
	header->right_edge += gap_between_titles + StringWidth( menu[0]->menuData );
	
	header->extent_bytes += sizeof (MenuList_entry);
	
	MDEF_0( mSizeMsg, menu, NULL, Point(), NULL );
}

pascal void DrawMenuBar_patch()
{
	WMgrPort_bezel_scope port_swap;
	
	draw_menu_bar_from_WMgr_port();
	
	const short pen_v = (MBarHeight + system_font_ascent) / 2;
	
	const UInt16 extent_bytes = MenuList[0]->extent_bytes;
	
	MenuList_entry* last = (MenuList_entry*) ((char*) *MenuList + extent_bytes);
	MenuList_entry* next = (MenuList_entry*) *MenuList;
	
	while ( ++next <= last )
	{
		MenuRef menu = next->menu;
		
		MoveTo( next->left_edge - 1 + menu_title_padding, pen_v );
		
		DrawString( menu[0]->menuData );
	}
}

#pragma mark -
#pragma mark Choosing From a Menu
#pragma mark -

static
void invert_menu_title( const MenuList_entry* entry, Rect& rect )
{
	// rect top and bottom are already set to 1 and 19, respectively.
	
	short textWidth = StringWidth( entry->menu[0]->menuData );
	
	rect.left  = entry->left_edge - 1;
	rect.right = rect.left + 2 * menu_title_padding + textWidth;
	
	InvertRect( &rect );
}

static inline
void invert_menu_title( short menuID, Rect& rect )
{
	// rect top and bottom are already set to 1 and 19, respectively.
	
	if ( MenuList_entry* entry = find_menu_id( menuID ) )
	{
		invert_menu_title( entry, rect );
	}
}

pascal void HiliteMenu_patch( short menuID )
{
	if ( menuID == TheMenu )
	{
		return;
	}
	
	WMgrPort_bezel_scope port_swap;
	
	Rect rect;
	
	rect.top    = 1;
	rect.bottom = MBarHeight - 1;
	
	invert_menu_title( TheMenu, rect );
	invert_menu_title( menuID,  rect );
	
	TheMenu = menuID;
}

static Handle SavedHandle;

static
void save_bits( BitMap& savedBits )
{
	QDGlobals& qd = get_QDGlobals();
	
	const Rect& bounds = savedBits.bounds;
	
	const short width  = bounds.right - bounds.left;
	const short height = bounds.bottom - bounds.top;
	
	const short rowBytes = width + 15 >> 4 << 1;
	
	savedBits.rowBytes = rowBytes;
	
	if ( SavedHandle == NULL )
	{
		SavedHandle = NewHandle( height * rowBytes );
	}
	else if ( GetHandleSize( SavedHandle ) < height * rowBytes )
	{
		SetHandleSize( SavedHandle, height * rowBytes );
	}
	
//	HLock( SavedHandle );
	
	savedBits.baseAddr = *SavedHandle;
	
	CopyBits( &qd.screenBits, &savedBits, &bounds, &bounds, srcCopy, NULL );
	
//	HUnlock( SavedHandle );
}

static
void restore_bits( BitMap& savedBits )
{
	QDGlobals& qd = get_QDGlobals();
	
	const Rect& bounds = savedBits.bounds;
	
//	HLock( SavedHandle );
	
	CopyBits( &savedBits, &qd.screenBits, &bounds, &bounds, srcCopy, NULL );
	
//	HUnlock( SavedHandle );
	
	savedBits.baseAddr = NULL;
}

static
void draw_menu_frame( const Rect& menuRect )
{
	Rect frame = menuRect;
	
	EraseRect( &frame );
	
	InsetRect( &frame, -1, -1 );
	FrameRect( &frame );
	
	Rect right_edge =
	{
		frame.top + 3, frame.right,
		frame.bottom,  frame.right + 1,
	};
	
	Rect bottom_edge =
	{
		frame.bottom,     frame.left  + 3,
		frame.bottom + 1, frame.right + 1,
	};
	
	PaintRect( &right_edge  );
	PaintRect( &bottom_edge );
}

static
void flash_menu_item( MenuRef menu, const Rect& r, Point pt, short item, int n )
{
	long delay = 3;
	
	for ( int i = 0;  i < n;  ++i )
	{
		pt.v = -pt.v;
		
		delay = ~delay & 0x7;  // 4 ticks on, 3 ticks off
		
		UInt32 whatever;
		Delay( delay, &whatever );
		
		MDEF_0( mChooseMsg, menu, &r, pt, &item );
	}
}

pascal long MenuSelect_patch( Point pt )
{
	WMgrPort_bezel_scope port_swap;
	
	BitMap savedBits;
	savedBits.baseAddr   = NULL;
	savedBits.bounds.top = MBarHeight;
	
	Rect menuRect;
	menuRect.top = MBarHeight;
	
	Rect titleRect;
	titleRect.top    = 1;
	titleRect.bottom = MBarHeight - 1;
	
	MenuList_entry* opened = NULL;
	
	short whichItem = 0;
	
	RgnHandle mouseRgn = NewRgn();
	
	SetRectRgn( mouseRgn, pt.h, pt.v, pt.h + 1, pt.v + 1 );
	
	while ( true )
	{
		if ( pt.v < MBarHeight )
		{
			// The mouse cursor is in the menu bar.
			
			if ( opened  &&  whichItem != 0 )
			{
				// Unhighlight the highlighted menu item.
				MDEF_0( mChooseMsg, opened->menu, &menuRect, pt, &whichItem );
			}
			
			// Hit test the menu titles.
			MenuList_entry* it = find_clicked_menu_title( pt );
			
			if ( it != opened )
			{
				// The current title has changed.
				
				raster_lock lock;
				
				if ( opened )
				{
					// Restore the open menu's bits and unhighlight the title.
					
					if ( savedBits.baseAddr != NULL )
					{
						restore_bits( savedBits );
					}
					
					invert_menu_title( opened, titleRect );
				}
				
				TheMenu = 0;
				
				opened = it;
				
				if ( opened )
				{
					// Highlight the open menu's title.
					invert_menu_title( opened, titleRect );
					
					MenuRef menu = opened->menu;
					
					TheMenu = menu[0]->menuID;
					
					menuRect.left = opened->left_edge;
					menuRect.right = menuRect.left + menu[0]->menuWidth;
					menuRect.bottom = menuRect.top + menu[0]->menuHeight;
					
					if ( menu[0]->menuHeight != 0 )
					{
						// Save the bits under the menu and draw it.
						
						savedBits.bounds.left   = menuRect.left - 1;
						savedBits.bounds.right  = menuRect.right + 2;
						savedBits.bounds.bottom = menuRect.bottom + 2;
						
						save_bits( savedBits );
						
						draw_menu_frame( menuRect );
						
						MDEF_0( mDrawMsg, menu, &menuRect, Point(), NULL );
					}
				}
			}
		}
		else if ( opened )
		{
			// The mouse cursor is below the menu bar, and a menu is open.
			// Hit-test its menu items and update which one is highlighted.
			
			MDEF_0( mChooseMsg, opened->menu, &menuRect, pt, &whichItem );
		}
		
		// Wait for mouse movement or mouse-up.
		
		const long sleep = 0x7FFFFFFF;
		
		EventRecord event;
		
		const bool eventful = WaitNextEvent( mUpMask, &event, sleep, mouseRgn );
		
		// Events for DAs won't occur here, so we don't need to check.
		
		if ( event.what == mouseUp )
		{
			break;
		}
		
		pt = event.where;
		
		SetRectRgn( mouseRgn, pt.h, pt.v, pt.h + 1, pt.v + 1 );
	}
	
	DisposeRgn( mouseRgn );
	
	long result = 0;
	
	if ( whichItem )
	{
		// Blink the chosen menu item and pack the result.
		
		flash_menu_item( opened->menu, menuRect, pt, whichItem, 3 * 2 );
		
		result = TheMenu << 16 | whichItem;
	}
	
	if ( savedBits.baseAddr != NULL )
	{
		// Restore the bits under the menu.
		restore_bits( savedBits );
	}
	
	return result;
}

#pragma mark -
#pragma mark Controlling the Appearance of Items
#pragma mark -

pascal void SetItem_patch( MenuInfo** menu, short item, ConstStr255Param text )
{
	WMgrPort_bezel_scope port_swap;
	
	const UInt8 newLen = text[ 0 ];
	
	unsigned char* p = menu[0]->menuData;
	
	p += 1 + *p;
	
	while ( *p )
	{
		if ( --item == 0 )
		{
			const UInt8 oldLen = p[ 0 ];
			
			if ( newLen == oldLen )
			{
				memcpy( p + 1, text + 1, newLen );
				
				MDEF_0( mSizeMsg, menu, NULL, Point(), NULL );
				return;
			}
			
			size_t size = GetHandleSize( (Handle) menu );
			
			if ( newLen < oldLen )
			{
				unsigned char* q = p + 1 + oldLen;
				
				memcpy( p, text, 1 + newLen );
				
				p += 1 + newLen;
				
				size_t n = (Ptr) *menu + size - (Ptr) q;
				
				memmove( p, q, n );
				
				SetHandleSize( (Handle) menu, size - (oldLen - newLen) );
			}
			else
			{
				const short offset = p - (unsigned char*) *menu;
				
				SetHandleSize( (Handle) menu, size + (newLen - oldLen) );
				
				p = (unsigned char*) *menu + offset;
				
				unsigned char* q = p + 1 + oldLen;
				unsigned char* r = p + 1 + newLen;
				
				size_t n = (Ptr) *menu + size - (Ptr) q;
				
				memmove( r, q, n );
				
				memcpy( p, text, 1 + newLen );
			}
			
			MDEF_0( mSizeMsg, menu, NULL, Point(), NULL );
			
			return;
		}
		
		p += 1 + *p + 4;
	}
}

pascal void GetItem_patch( MenuInfo** menu, short item, Str255 result )
{
	menu_item_const_iterator it( menu );
	
	while ( const unsigned char* text = it )
	{
		if ( --item == 0 )
		{
			memcpy( result, text, 1 + text[ 0 ] );
			return;
		}
		
		++it;
	}
	
	result[ 0 ] = '\0';
}

pascal void CheckItem_patch( MenuInfo** menu, short item, char checked )
{
}

#pragma mark -
#pragma mark Miscellaneous Routines
#pragma mark -

pascal void FlashMenuBar_patch( short menuID )
{
	WMgrPort_bezel_scope port_swap;
	
	Rect menu_bar = BezelRgn[0]->rgnBBox;
	
	menu_bar.bottom = MBarHeight;
	
	InvertRect( &menu_bar );
}

pascal short CountMItems_patch( MenuInfo** menu )
{
	short n = 0;
	
	menu_item_const_iterator it( menu );
	
	while ( it )
	{
		++it;
		++n;
	}
	
	return n;
}
