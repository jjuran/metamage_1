/*
	Dialogs.cc
	----------
*/

#include "Dialogs.hh"

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __DIALOGS__
#include <Dialogs.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

// Standard C
#include <string.h>

// ams-common
#include "QDGlobals.hh"


static short ANumber;
static short ACount = -1;

StringHandle DAStrings[ 4 ] : 0x0AA0;


static
Handle NewHandleOrBust( Size size )
{
	Handle h = NewHandle( size );
	
	if ( h == NULL )
	{
		SysError( 25 );
	}
	
	return h;
}

static
void set_param( short i, const unsigned char* text )
{
	StringHandle& h = DAStrings[ i ];
	
	if ( text == NULL  ||  text[ 0 ] == 0 )
	{
		if ( h != NULL )
		{
			**h = 0;
		}
		
		return;
	}
	
	if ( h == NULL )
	{
		h = (StringHandle) NewHandleOrBust( 256 );
	}
	
	memcpy( *h, text, 1 + text[ 0 ] );
}

struct expansion_storage
{
	Handle text;
	UInt16 size;  // Worst case is 127 * 255 + 1 == 32386
};

static
const expansion_storage& expand_param_text( const unsigned char* text )
{
	static expansion_storage expanded = { NewHandleOrBust( 0 ) };
	
	static UInt16 capacity = 0;
	
	UInt16 expanded_size = 0;
	
	const unsigned char* begin = text + 1;
	const unsigned char* end   = begin + text[ 0 ];
	
	for ( const unsigned char* it = begin;  it < end; )
	{
		unsigned char c = *it++;
		
		if ( c != '^'  ||  it == end )
		{
			++expanded_size;
			continue;
		}
		
		c = *it++ - '0';
		
		if ( c > 3 )
		{
			expanded_size += 2;
			continue;
		}
		
		if ( StringHandle h = DAStrings[ c ] )
		{
			expanded_size += **h;
		}
	}
	
	expanded.size = expanded_size;
	
	if ( capacity < expanded_size )
	{
		capacity = expanded_size;
		
		DisposeHandle( expanded.text );
		
		expanded.text = NewHandleOrBust( capacity );
	}
	
	char* p = *expanded.text;
	
	for ( const unsigned char* q = begin;  q < end; )
	{
		unsigned char c = *q++;
		
		if ( c != '^'  ||  q == end )
		{
			*p++ = c;
			continue;
		}
		
		c = *q++ - '0';
		
		if ( c > 3 )
		{
			*p++ = '^';
			*p++ = c + '0';
			continue;
		}
		
		if ( StringHandle h = DAStrings[ c ] )
		{
			memcpy( p, *h + 1, **h );
			
			p += **h;
		}
	}
	
	return expanded;
}


#pragma mark -
#pragma mark Initialization
#pragma mark -

pascal void InitDialogs_patch( void* proc )
{
}

#pragma mark -
#pragma mark Creating and Disposing of Dialogs
#pragma mark -

pascal DialogRef NewDialog_patch( void*                 storage,
                                  const Rect*           bounds,
                                  const unsigned char*  title,
                                  Boolean               visible,
                                  short                 procID,
                                  WindowRef             behind,
                                  Boolean               closable,
                                  long                  refCon,
                                  Handle                items )
{
	void* original_storage = storage;
	
	if ( storage == NULL )
	{
		storage = NewPtr( sizeof (DialogRecord) );
		
		if ( storage == NULL )
		{
			return NULL;
		}
	}
	
	memset( storage, '\0', sizeof (DialogRecord) );
	
	WindowRef window = NewWindow( storage,
	                              bounds,
	                              title,
	                              visible,
	                              procID,
	                              behind,
	                              closable,
	                              refCon );
	
	if ( window == NULL )
	{
		if ( original_storage == NULL )
		{
			DisposePtr( (Ptr) storage );
		}
		
		return NULL;
	}
	
	WindowPeek w = (WindowPeek) window;
	DialogPeek d = (DialogPeek) window;
	
	w->windowKind = dialogKind;
	
	TextFont( systemFont );
	
	d->aDefItem = 1;
	
	d->items = items;
	
	char* p = *items;
	
	short n_items_1 = *((const UInt16*) p)++;  // item count minus one
	
	do
	{
		void*& placeholder = *((void**) p)++;
		
		const Rect& bounds = *((Rect*) p)++;
		
		UInt8 type = *p++;
		UInt8 len  = *p++;
		
		switch ( type & 0x7F )
		{
			case ctrlItem + btnCtrl:
				placeholder = NewControl( window,
				                          &bounds,
				                          (const uint8_t*) p - 1,
				                          visible,
				                          0,  // value
				                          0,  // min
				                          1,  // max
				                          pushButProc,
				                          0 );
				
				ValidRect( &bounds );
				break;
			
			default:
				break;
		}
		
		p += len + (len & 1);
	}
	while ( --n_items_1 >= 0 );
	
	return window;
}

pascal void CloseDialog_patch( DialogRef dialog )
{
	DialogPeek d = (DialogPeek) dialog;
	
	DisposeHandle( d->items );
	
	CloseWindow( dialog );
}

pascal void DisposeDialog_patch( DialogRef dialog )
{
	CloseDialog( dialog );
	
	DisposePtr( (Ptr) dialog );
}

#pragma mark -
#pragma mark Handling Dialog Events
#pragma mark -

static
bool invoke_defItem( DialogPeek d )
{
	const char* p = *d->items;
	
	const short n_items_1 = *((const UInt16*) p)++;  // item count minus one
	
	if ( d->aDefItem > 1 )
	{
		p += sizeof (void*) + sizeof (Rect) + sizeof (UInt8);
		
		UInt8 len = *p++;
		
		p += len + (len & 1);
	}
	
	void* placeholder = *((void**) p)++;
	
	p += sizeof (Rect);
	
	UInt8 type = *p;
	
	if ( type == ctrlItem + btnCtrl )
	{
		UInt32 dummy;
		
		ControlRef button = (ControlRef) placeholder;
		
		HiliteControl( button, kControlButtonPart );
		Delay( 8, &dummy );
		HiliteControl( button, kControlNoPart );
	}
	
	return ! (type & 0x80);
}

pascal void ModalDialog_patch( ModalFilterUPP filterProc, short* itemHit )
{
	QDGlobals& qd = get_QDGlobals();
	
	WindowRef window = qd.thePort;
	
	WindowPeek w = (WindowPeek) window;
	DialogPeek d = (DialogPeek) window;
	
	const long sleep = 0x7fffffff;
	
	while ( true )
	{
		EventRecord event;
		
		if ( WaitNextEvent( everyEvent, &event, sleep, NULL ) )
		{
			switch ( event.what )
			{
				case updateEvt:
					BeginUpdate( window );
					DrawDialog( window );
					EndUpdate( window );
					break;
				
				case mouseDown:
				{
					if ( ! PtInRgn( event.where, w->contRgn ) )
					{
						SysBeep( 30 );
						continue;
					}
					
					Point pt = event.where;
					GlobalToLocal( &pt );
					
					const char* p = *d->items;
					
					// item count minus one
					const short n_items_1 = *((const UInt16*) p)++;
					
					short item_index = 0;
					
					while ( item_index++ <= n_items_1 )
					{
						const void* pointer = *((const void**) p)++;
						
						const Rect* bounds = (const Rect*) p;
						
						p += sizeof (Rect);
						
						UInt8 type = *p++;
						UInt8 len  = *p++;
						
						if ( PtInRect( pt, bounds ) )
						{
							if ( (type & 0x7c) == ctrlItem )
							{
								ControlRef control = (ControlRef) pointer;
								
								if ( ! TrackControl( control, pt, NULL ) )
								{
									break;
								}
							}
							
							if ( ! (type & 0x80) )
							{
								*itemHit = item_index;
								return;
							}
						}
						
						p += len + (len & 1);
					}
					
					break;
				}
				
				case keyDown:
				{
					switch ( (char) event.message )
					{
						case kEnterCharCode:
						case kReturnCharCode:
							if ( invoke_defItem( d ) )
							{
								*itemHit = d->aDefItem;
								return;
							}
						
						default:
							break;
					}
					
					break;
				}
				
				default:
					break;
			}
		}
	}
}

pascal void DrawDialog_patch( DialogRef dialog )
{
	DrawControls( dialog );
	
	DialogPeek d = (DialogPeek) dialog;
	
	Handle h = d->items;
	
	const char* p = *h;
	
	short n_items_1 = *((const UInt16*) p)++;  // item count minus one
	
	do
	{
		p += 4;  // skip placeholder
		
		const Rect& bounds = *((Rect*) p)++;
		
		UInt8 type = *p++;
		UInt8 len  = *p++;
		
		switch ( type & 0x7F )
		{
			case statText:
			{
				const UInt8* text = (const UInt8*) p - 1;
				
				const expansion_storage& expansion = expand_param_text( text );
				
				TETextBox( *expansion.text, expansion.size, &bounds, 0 );
				break;
			}
			
			case iconItem:
				if ( Handle h = GetResource( 'ICON', *(const ResID*) p ) )
				{
					PlotIcon( &bounds, h );
					ReleaseResource( h );
				}
				else
				{
					FrameRect( &bounds );
				}
				break;
			
			default:
				break;
		}
		
		p += len + (len & 1);
	}
	while ( --n_items_1 >= 0 );
}

#pragma mark -
#pragma mark Invoking Alerts
#pragma mark -

/*
	Apple's Human Interface Guidelines state that there should be 13 vertical
	and 23 horizontal white pixels between, respectively the top and left edges
	of the window frame and the dialog/alert icon.  Since the frame itself has
	a 3-pixel margin already, that reduces the figures to 10 and 20.
*/

static const Rect alert_icon_bounds = { 10, 20, 10 + 32, 20 + 32 };

static
void DITL_append_icon( Handle items, ResID icon_id )
{
	char* p = *items;
	
	short n_items_1 = *((const short*) p)++;  // item count minus one
	
	do
	{
		p += sizeof (void*) + sizeof (Rect) + sizeof (UInt8);
		
		UInt8 len = *p++;
		
		p += len + (len & 1);
	}
	while ( --n_items_1 >= 0 );
	
	const short added_length = sizeof (void*)
	                         + sizeof (Rect)
	                         + sizeof (UInt8)
	                         + sizeof (UInt8)
	                         + sizeof (ResID);  // 16 bytes
	
	const Size old_size = p - *items;
	
	SetHandleSize( items, old_size + added_length );
	
	p = *items + old_size;
	
	*((void**) p)++ = NULL;
	*((Rect* ) p)++ = alert_icon_bounds;
	
	*p++ = iconItem + itemDisable;
	*p++ = sizeof (ResID);
	
	*(short*) p = icon_id;
	
	++*((short*) *items);
}

static
short basic_Alert( short alertID, ModalFilterUPP filterProc, ResID icon_id )
{
	Handle h = GetResource( 'ALRT', alertID );
	
	if ( h == NULL )
	{
		return ResError();
	}
	
	AlertTHndl alert = (AlertTHndl) h;
	
	const Rect bounds   = alert[0]->boundsRect;
	const short itemsID = alert[0]->itemsID;
	const short stages  = alert[0]->stages;
	
	ReleaseResource( h );
	
	h = GetResource( 'DITL', itemsID );
	
	if ( h == NULL )
	{
		return ResError();
	}
	
	DetachResource( h );
	
	if ( icon_id >= 0 )
	{
		DITL_append_icon( h, icon_id );
	}
	
	DialogRef dialog = NewDialog( NULL,
	                              &bounds,
	                              "\p",   // dBoxProc doesn't draw a title
	                              true,   // visible
	                              dBoxProc,
	                              (WindowRef) -1,
	                              false,  // not closable
	                              0,      // refCon
	                              h );
	
	if ( dialog == NULL )
	{
		return MemError();
	}
	
	if ( alertID != ANumber )
	{
		ANumber = alertID;
		ACount  = 0;
	}
	else if ( ACount < 3 )
	{
		++ACount;
	}
	
	DialogPeek d = (DialogPeek) dialog;
	
	d->aDefItem = ((stages >> 4 * ACount + 3) & 0x1) + 1;
	
	short itemHit = 0;
	
	ModalDialog( NULL, &itemHit );
	
	DisposeDialog( dialog );
	
	return itemHit;
}

pascal short Alert_patch( short alertID, ModalFilterUPP filterProc )
{
	return basic_Alert( alertID, filterProc, -1 );
}

pascal short StopAlert_patch( short alertID, ModalFilterUPP filterProc )
{
	return basic_Alert( alertID, filterProc, kStopIcon );
}

pascal short NoteAlert_patch( short alertID, ModalFilterUPP filterProc )
{
	return basic_Alert( alertID, filterProc, kNoteIcon );
}

pascal short CautionAlert_patch( short alertID, ModalFilterUPP filterProc )
{
	return basic_Alert( alertID, filterProc, kCautionIcon );
}

#pragma mark -
#pragma mark Manipulating Items in Dialogs and Alerts
#pragma mark -

pascal void ParamText_patch( const unsigned char*  p1,
                             const unsigned char*  p2,
                             const unsigned char*  p3,
                             const unsigned char*  p4 )
{
	set_param( 0, p1 );
	set_param( 1, p2 );
	set_param( 2, p3 );
	set_param( 3, p4 );
}
