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

// iota
#include "iota/char_types.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "raster_lock.hh"

// ams-core
#include "MBDF.hh"
#include "MDEF.hh"
#include "Windows.hh"


Byte SdVolume    : 0x0260;
GrafPtr WMgrPort : 0x09DE;
short TheMenu    : 0x0A26;
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

static MenuList_header** MenuList;

const short system_font_ascent =  9;
const short menu_title_padding =  9;
const short gap_between_titles = 13;  // space between text of adjacent titles

static const Point zero_Point = { 0 };


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

pascal MenuInfo** NewMenu_patch( short menuID, const unsigned char* title )
{
	Handle h = NewHandleClear( sizeof (MenuInfo) - (255 - title[ 0 ]) + 1 );
	
	MenuRef menu = (MenuRef) h;
	
	menu[0]->menuID      = menuID;
	menu[0]->enableFlags = 0xFFFFFFFF;
	
	fast_memcpy( menu[0]->menuData, title, 1 + title[ 0 ] );
	
	return menu;
}

pascal MenuInfo** GetMenu_patch( short resID )
{
	Handle h = GetResource( 'MENU', resID );
	
	return (MenuRef) h;
}

pascal void DisposeMenu_patch( MenuInfo** menu )
{
	DisposeHandle( (Handle) menu );
}

#pragma mark -
#pragma mark Forming the Menus
#pragma mark -

static
UInt8 actual_item_text_length( const UInt8* format, UInt8 len )
{
	UInt8 text_len = 0;
	
	const UInt8* q = format;
	
	while ( len-- > 0 )
	{
		UInt8 c = *q++;
		
		if ( c == ';' )
		{
			break;
		}
		
		switch ( c )
		{
			case '(':
				break;
			
			case '/':
				if ( len )
				{
					--len, ++q;
				}
				break;
			
			default:
				++text_len;
		}
	}
	
	return text_len;
}

static
short decode_item_format( UInt8 const* format, UInt8 len,
                          UInt8*       p,      UInt8 text_len )
{
	*p++ = text_len;
	
	UInt8* meta = p + text_len;
	
	UInt8 icon  = 0;
	UInt8 key   = 0;
	UInt8 mark  = 0;
	UInt8 style = 0;
	
	const UInt8* q = format;
	
	while ( len-- > 0 )
	{
		UInt8 c = *q++;
		
		if ( c == ';' )
		{
			break;
		}
		
		switch ( c )
		{
			case '(':  break;
			case '/':  if ( len )  key = *q++, --len;  break;
			
			default:
				*p++ = c;
		}
	}
	
	*meta++ = icon;
	*meta++ = key;
	*meta++ = mark;
	*meta++ = style;
	*meta   = '\0';
	
	return q - format;
}

static
short append_one_item( MenuRef menu, const UInt8* format, UInt8 length )
{
	const Size oldSize = GetHandleSize( (Handle) menu );
	
	const UInt8 text_len = actual_item_text_length( format, length );
	
	const Size newSize = oldSize + 1 + text_len + 4;
	
	SetHandleSize( (Handle) menu, newSize );
	
	unsigned char* p = (unsigned char*) *menu + oldSize - 1;
	
	return decode_item_format( format, length, p, text_len );
}

pascal void AppendMenu_patch( MenuInfo** menu, const unsigned char* format )
{
	short count = CountMItems_patch( menu );
	
	WMgrPort_bezel_scope port_swap;
	
	UInt8 length = *format++;
	
	do
	{
		const bool disabled = format[ 0 ] == '(';
		
		short n_bytes_consumed = append_one_item( menu, format, length );
		
		format += n_bytes_consumed;
		length -= n_bytes_consumed;
		
		++count;
		
		if ( disabled )
		{
			DisableItem_patch( menu, count );
		}
	}
	while ( length > 0 );
	
	MDEF_0( mSizeMsg, menu, NULL, zero_Point, NULL );
}

pascal void AddResMenu_patch( MenuInfo** menu, ResType type )
{
	if ( type == 'DRVR' )
	{
		AppendMenu( menu, "\p"
		                  "(\0" "Advanced Mac Substitute" ";"
		                  "(\0" "by Josh Juran" );
	}
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
	
	MDEF_0( mSizeMsg, menu, NULL, zero_Point, NULL );
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
		
		const short left = next->left_edge - 1 + menu_title_padding;
		
		MoveTo( left, pen_v );
		
		DrawString( menu[0]->menuData );
		
		if ( ! (menu[0]->enableFlags & 1) )
		{
			QDGlobals& qd = get_QDGlobals();
			
			PenPat( &qd.gray );
			PenMode( patBic );
			
			Rect titleRect = { 1, left, MBarHeight - 1, qd.thePort->pnLoc.h };
			
			PaintRect( &titleRect );
			
			PenNormal();
		}
	}
}

pascal void DeleteMenu_patch( short menuID )
{
	if ( MenuList[0]->extent_bytes == 0 )
	{
		DebugStr( "\p" "DeleteMenu with empty menu bar" );
		return;
	}
	
	MenuList_header* header = *MenuList;
	
	const UInt16 extent_bytes = header->extent_bytes;
	
	MenuList_entry* last = (MenuList_entry*) ((char*) header + extent_bytes);
	
	if ( last->menu[0]->menuID == menuID )
	{
		header->right_edge = last->left_edge;
	}
	else if ( MenuList_entry* found = find_menu_id( menuID ) )
	{
		// TODO:  Delete non-last menus
		DebugStr( "\p" "DeleteMenu of non-last menu is unimplemented" );
		return;
	}
	else
	{
		WARNING = "DeleteMenu: menu ID ", menuID, " isn't in menu list";
		return;
	}
	
	header->extent_bytes -= sizeof (MenuList_entry);
}

pascal void ClearMenuBar_patch()
{
	SetHandleSize( (Handle) MenuList, sizeof (MenuList_header) );
	
	MenuList[0]->extent_bytes = 0;
	MenuList[0]->right_edge   = 10;
}

pascal Handle GetNewMBar_patch( ResID menuBarID )
{
	WMgrPort_bezel_scope port_swap;
	
	if ( Handle h = GetResource( 'MBAR', menuBarID ) )
	{
		const short* p = (const short*) *h;
		
		short n = *p++;
		
		const UInt16 extent_bytes = sizeof (MenuList_entry) * n;
		
		const Size size = sizeof (MenuList_header)
		                + sizeof (MenuList_entry) * n;
		
		Handle menuList = NewHandleClear( size );
		
		MenuList_header* header = (MenuList_header*) *menuList;
		
		header->extent_bytes = extent_bytes;
		
		MenuList_entry* next = (MenuList_entry*) header;
		
		short right_edge = 10;
		
		while ( n-- > 0 )
		{
			++next;
			
			short id = *p++;
			
			MenuRef menu = GetMenu_patch( id );
			
			next->menu      = menu;
			next->left_edge = right_edge;
			
			right_edge += gap_between_titles + StringWidth( menu[0]->menuData );
			
			MDEF_0( mSizeMsg, menu, NULL, zero_Point, NULL );
		}
		
		header->right_edge = right_edge;
		
		ReleaseResource( h );
		
		return menuList;
	}
	
	return NULL;
}

pascal Handle GetMenuBar_patch()
{
	Handle result = (Handle) MenuList;
	
	if ( OSErr err = HandToHand( &result ) )
	{
		return NULL;
	}
	
	return result;
}

pascal void SetMenuBar_patch( Handle list )
{
	const Size size = GetHandleSize( list );
	
	SetHandleSize( (Handle) MenuList, size );
	
	fast_memcpy( *MenuList, *list, size );
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
						
						savedBits.bounds.left   = menuRect.left  - 1     & ~0x7;
						savedBits.bounds.right  = menuRect.right + 2 + 7 & ~0x7;
						savedBits.bounds.bottom = menuRect.bottom + 2;
						
						save_bits( savedBits );
						
						draw_menu_frame( menuRect );
						
						MDEF_0( mDrawMsg, menu, &menuRect, zero_Point, NULL );
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
	else if ( opened )
	{
		// If nothing was chosen, unhighlight the menu title.
		
		invert_menu_title( opened, titleRect );
		
		TheMenu = 0;
	}
	
	if ( savedBits.baseAddr != NULL )
	{
		// Restore the bits under the menu.
		restore_bits( savedBits );
	}
	
	return result;
}

pascal long MenuKey_patch( unsigned short key )  // char
{
	const char c = iota::to_upper( key );
	
	const UInt16 extent_bytes = MenuList[0]->extent_bytes;
	
	MenuList_entry* next = (MenuList_entry*) ((char*) *MenuList + extent_bytes);
	MenuList_entry* end  = (MenuList_entry*) *MenuList;
	
	while ( next > end )
	{
		MenuRef menu = next->menu;
		
		menu_item_iterator it( menu );
		
		short n = 0;
		
		while ( const unsigned char* text = it )
		{
			++n;
			
			const unsigned char* p = text + 1 + text[ 0 ] + 1;  // skip icon
			
			if ( *p == c )
			{
				HiliteMenu_patch( menu[0]->menuID );
				
				return menu[0]->menuID << 16 | n;
			}
			
			++it;
		}
		
		--next;
	}
	
	return 0;
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
				fast_memcpy( p + 1, text + 1, newLen );
				
				MDEF_0( mSizeMsg, menu, NULL, zero_Point, NULL );
				return;
			}
			
			size_t size = GetHandleSize( (Handle) menu );
			
			if ( newLen < oldLen )
			{
				unsigned char* q = p + 1 + oldLen;
				
				p = (unsigned char*) fast_mempcpy( p, text, 1 + newLen );
				
				size_t n = (Ptr) *menu + size - (Ptr) q;
				
				fast_memmove( p, q, n );
				
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
				
				fast_memmove( r, q, n );
				
				fast_memcpy( p, text, 1 + newLen );
			}
			
			MDEF_0( mSizeMsg, menu, NULL, zero_Point, NULL );
			
			return;
		}
		
		p += 1 + *p + 4;
	}
}

pascal void GetItem_patch( MenuInfo** menu, short item, Str255 result )
{
	menu_item_iterator it( menu );
	
	while ( const unsigned char* text = it )
	{
		if ( --item == 0 )
		{
			fast_memcpy( result, text, 1 + text[ 0 ] );
			return;
		}
		
		++it;
	}
	
	result[ 0 ] = '\0';
}

pascal void DisableItem_patch( MenuInfo** menu, short item )
{
	if ( item <= 31 )
	{
		menu[0]->enableFlags &= ~(1 << item);
	}
}

pascal void EnableItem_patch( MenuInfo** menu, short item )
{
	if ( item <= 31 )
	{
		menu[0]->enableFlags |= 1 << item;
	}
}

pascal void CheckItem_patch( MenuInfo** menu, short item, char checked )
{
	char mark = checked ? kCheckCharCode  // 0x12
	                    : kNullCharCode;  // 0x00
	
	SetItemMark_patch( menu, item, mark );
}

pascal void SetItemMark_patch( MenuInfo** menu, short item, char mark )
{
	menu_item_iterator it( menu );
	
	while ( unsigned char* p = it )
	{
		if ( --item == 0 )
		{
			p += 1 + p[ 0 ] + 2;
			
			*p = mark;
			
			return;
		}
		
		++it;
	}
}

#pragma mark -
#pragma mark Miscellaneous Routines
#pragma mark -

pascal short CountMItems_patch( MenuInfo** menu )
{
	short n = 0;
	
	menu_item_iterator it( menu );
	
	while ( it )
	{
		++it;
		++n;
	}
	
	return n;
}

pascal MenuInfo** GetMHandle_patch( short menuID )
{
	MenuList_entry* it = find_menu_id( menuID );
	
	return it ? it->menu : NULL;
}

pascal void FlashMenuBar_patch( short menuID )
{
	WMgrPort_bezel_scope port_swap;
	
	Rect menu_bar = BezelRgn[0]->rgnBBox;
	
	menu_bar.bottom = MBarHeight;
	
	InvertRect( &menu_bar );
}

static
short insert_one_item( MenuRef menu, const UInt8* format, UInt8 length, short i )
{
	if ( i <= 31 )
	{
		const long stay_mask = (1 << i) - 1;
		const long move_mask = ~stay_mask;
		
		const long flags = menu[0]->enableFlags;
		
		long stay_flags = flags & stay_mask;
		long move_flags = flags & move_mask;
		
		move_flags <<= 1;
		
		menu[0]->enableFlags = move_flags | stay_flags;
	}
	
	const UInt8 text_len = actual_item_text_length( format, length );
	
	const Size increase = 1 + text_len + 4;
	const Size old_size = GetHandleSize( (Handle) menu );
	const Size new_size = old_size + increase;
	
	SetHandleSize( (Handle) menu, new_size );
	
	menu_item_iterator it( menu );
	
	while ( it  &&  --i > 0 )
	{
		++it;
	}
	
	const size_t offset = it.get() - (unsigned char*) *menu;
	
	unsigned char* p = (unsigned char*) *menu + offset;
	unsigned char* q = p + increase;
	
	while ( it )
	{
		++it;
	}
	
	const unsigned char* end = it.get() + 1;
	
	fast_memmove( q, p, end - p );
	
	return decode_item_format( format, length, p, text_len );
}

pascal void InsMenuItem_patch( MenuInfo**            menu,
                               const unsigned char*  format,
                               short                 after )
{
	WMgrPort_bezel_scope port_swap;
	
	UInt8 length = *format++;
	
	const short n = CountMItems_patch( menu );
	
	if ( after > n )
	{
		after = n;
	}
	
	const short i = after + 1;
	
	do
	{
		const bool disabled = format[ 0 ] == '(';
		
		short n_bytes_consumed = insert_one_item( menu, format, length, i );
		
		format += n_bytes_consumed;
		length -= n_bytes_consumed;
		
		if ( ! disabled )
		{
			EnableItem_patch( menu, i );
		}
	}
	while ( length > 0 );
	
	MDEF_0( mSizeMsg, menu, NULL, zero_Point, NULL );
}

SysBeep_ProcPtr old_SysBeep;

pascal void SysBeep_patch( short duration )
{
	if ( SdVolume & 0x7 )
	{
		old_SysBeep( duration );
		return;
	}
	
	FlashMenuBar_patch( 0 );
	
	UInt32 dummy;
	Delay( 8, &dummy );
	
	FlashMenuBar_patch( 0 );
}
