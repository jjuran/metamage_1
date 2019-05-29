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

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"

// ams-ui
#include "modal_updating.hh"
#include "scoped_port.hh"


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
	
	fast_memcpy( *h, text, 1 + text[ 0 ] );
}

static
Handle expand_param_text( const unsigned char* text )
{
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
	
	Handle expanded = NewHandleOrBust( expanded_size );
	
	char* p = *expanded;
	
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
			p = (char*) fast_mempcpy( p, *h + 1, **h );
		}
	}
	
	return expanded;
}

struct DialogItem
{
	Handle  handle;
	Rect    bounds;
	UInt8   type;
	UInt8   length;
};

struct DialogItemList_header
{
	short       count_minus_1;
	DialogItem  first;
};

static inline
short& dialog_item_count_minus_one( Handle items )
{
	return ((DialogItemList_header*) *items)->count_minus_1;
}

static inline
short dialog_item_count( Handle items )
{
	return dialog_item_count_minus_one( items ) + 1;
}

static inline
DialogItem* first_dialog_item( Handle items )
{
	return &((DialogItemList_header*) *items)->first;
}

static
ResID& item_ResID( DialogItem* di )
{
	return *(ResID*) (di + 1);
}

static
ResID item_ResID( const DialogItem* di )
{
	return *(const ResID*) (di + 1);
}

static
DialogItem* next( DialogItem* di )
{
	return (DialogItem*) ((char*) (di + 1) + di->length + (di->length & 1));
}

static
const DialogItem* next( const DialogItem* di )
{
	return next( const_cast< DialogItem* >( di ) );
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
	
	fast_memset( storage, '\0', sizeof (DialogRecord) );
	
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
	
	d->aDefItem = 1;
	
	d->items = items;
	
	short n_items_1 = dialog_item_count_minus_one( items );
	
	DialogItem* item = first_dialog_item( items );
	
	do
	{
		switch ( item->type & 0x7F )
		{
			case ctrlItem + btnCtrl:
			case ctrlItem + chkCtrl:
			case ctrlItem + radCtrl:
				item->handle = (Handle) NewControl( window,
				                                    &item->bounds,
				                                    &item->length,
				                                    true,
				                                    0,  // value
				                                    0,  // min
				                                    1,  // max
				                                    item->type & 0x03,
				                                    0 );
				break;
			
			case statText:
			case editText:
				item->handle = expand_param_text( &item->length );
				break;
			
			case iconItem:
				item->handle = GetResource( 'ICON', item_ResID( item ) );
				break;
			
			case picItem:
				item->handle = GetResource( 'PICT', item_ResID( item ) );
				break;
			
			default:
				break;
		}
		
		item = next( item );
	}
	while ( --n_items_1 >= 0 );
	
	return window;
}

pascal DialogRef GetNewDialog_patch( short id, void* storage, WindowRef behind )
{
	Handle h = GetResource( 'DLOG', id );
	
	if ( h == NULL )
	{
		return NULL;
	}
	
	const DialogTemplate dlog = *(const DialogTemplate*) *h;
	
	ReleaseResource( h );
	
	h = GetResource( 'DITL', dlog.itemsID );
	
	if ( h == NULL )
	{
		return NULL;
	}
	
	DetachResource( h );
	
	DialogRef dialog = NewDialog( storage,
	                              &dlog.boundsRect,
	                              dlog.title,
	                              dlog.visible,
	                              dlog.procID,
	                              behind,
	                              dlog.goAwayFlag,
	                              dlog.refCon,
	                              h );
	
	return dialog;
}

pascal void CloseDialog_patch( DialogRef dialog )
{
	DialogPeek d = (DialogPeek) dialog;
	
	short n_items_1 = dialog_item_count_minus_one( d->items );
	
	const DialogItem* item = first_dialog_item( d->items );
	
	do
	{
		if ( Handle h = item->handle )
		{
			switch ( item->type & 0x7F )
			{
				case statText:
				case editText:
					DisposeHandle( h );
					break;
				
				case iconItem:
				case picItem:
					ReleaseResource( h );
					break;
				
				default:
					break;
			}
		}
		
		item = next( item );
	}
	while ( --n_items_1 >= 0 );
	
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
	const DialogItem* item = first_dialog_item( d->items );
	
	if ( d->aDefItem > 1 )
	{
		item = next( item );
	}
	
	const UInt8 type = item->type;
	
	if ( type == ctrlItem + btnCtrl )
	{
		UInt32 dummy;
		
		ControlRef button = (ControlRef) item->handle;
		
		HiliteControl( button, kControlButtonPart );
		Delay( 8, &dummy );
		HiliteControl( button, kControlNoPart );
	}
	
	return ! (type & 0x80);
}

pascal void ModalDialog_patch( ModalFilterUPP filterProc, short* itemHit )
{
	modal_update_scope updating( filterProc == NULL );
	
	QDGlobals& qd = get_QDGlobals();
	
	WindowRef window = FrontWindow();
	
	scoped_port thePort = window;
	
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
					window = (WindowRef) event.message;
					
					BeginUpdate( window );
					DrawDialog( window );
					EndUpdate( window );
					break;
				
				case mouseDown:
				{
					if ( ! PtInRgn( event.where, w->contRgn ) )
					{
						SysBeep( 6 );
						continue;
					}
					
					Point pt = event.where;
					GlobalToLocal( &pt );
					
					// item count minus one
					short n_items_1 = dialog_item_count_minus_one( d->items );
					
					const DialogItem* item = first_dialog_item( d->items );
					
					short item_index = 0;
					
					while ( item_index++ <= n_items_1 )
					{
						const UInt8 type = item->type;
						
						if ( PtInRect( pt, &item->bounds ) )
						{
							if ( (type & 0x7c) == ctrlItem )
							{
								ControlRef control = (ControlRef) item->handle;
								
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
						
						item = next( item );
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

pascal Boolean IsDialogEvent_patch( const EventRecord* event )
{
	const short what = event->what;
	
	if ( what == activateEvt  ||  what == updateEvt )
	{
		return ((WindowPeek) event->message)->windowKind == dialogKind;
	}
	
	WindowPeek front = (WindowPeek) FrontWindow();
	
	if ( front  &&  front->windowKind == dialogKind )
	{
		if ( what == mouseDown )
		{
			return PtInRgn( event->where, front->contRgn );
		}
		
		return true;
	}
	
	return false;
}

pascal Boolean DialogSelect_patch( const EventRecord*  event,
                                   DialogRef*          dialogHit,
                                   short*              itemHit )
{
	DialogRef dialog;
	GrafPtr savedPort;
	
	switch ( event->what )
	{
		case mouseDown:
		case keyDown:
		case autoKey:
			break;
		
		case updateEvt:
			dialog = (DialogRef) event->message;
			
			GetPort( &savedPort );
			SetPort( dialog );
			
			BeginUpdate( dialog );
			DrawDialog ( dialog );
			EndUpdate  ( dialog );
			
			SetPort( savedPort );
			return false;
		
		case activateEvt:
			// TODO:  Activate TE items
			return false;
		
		default:
			return false;
	}
	
	dialog = FrontWindow();
	
	DialogPeek d = (DialogPeek) dialog;
	
	// TODO:  keyDown/autoKey
	
	if ( event->what == mouseDown )
	{
		Point pt = event->where;
		GlobalToLocal( &pt );
		
		// item count minus one
		short n_items_1 = dialog_item_count_minus_one( d->items );
		
		const DialogItem* item = first_dialog_item( d->items );
		
		short item_index = 0;
		
		while ( item_index++ <= n_items_1 )
		{
			const UInt8 type = item->type;
			
			if ( PtInRect( pt, &item->bounds ) )
			{
				if ( type & 0x80 )
				{
					return false;  // disabled
				}
				
				if ( (type & 0x7c) == ctrlItem )
				{
					ControlRef control = (ControlRef) item->handle;
					
					if ( ! TrackControl( control, pt, NULL ) )
					{
						return false;
					}
				}
				
				*itemHit = item_index;
				
				return true;
			}
			
			item = next( item );
		}
	}
	
	return false;
}

pascal void DrawDialog_patch( DialogRef dialog )
{
	scoped_port thePort = dialog;
	
	DrawControls( dialog );
	
	DialogPeek d = (DialogPeek) dialog;
	
	short n_items_1 = dialog_item_count_minus_one( d->items );
	
	const DialogItem* item = first_dialog_item( d->items );
	
	short item_index = 1;
	
	do
	{
		const Rect& bounds = item->bounds;
		
		switch ( item->type & 0x7F )
		{
			case userItem:
				if ( UserItemUPP userProc = (UserItemUPP) item->handle )
				{
					userProc( dialog, item_index );
				}
				break;
			
			case editText:
			{
				Rect box = bounds;
				
				InsetRect( &box, -3, -3 );
				FrameRect( &box );
			}
			// fall through
			
			case statText:
			{
				Handle h = item->handle;
				
				TETextBox( *h, GetHandleSize( h ), &bounds, 0 );
				break;
			}
			
			case iconItem:
				if ( Handle h = item->handle )
				{
					PlotIcon( &bounds, h );
				}
				else
				{
					FrameRect( &bounds );
				}
				break;
			
			case picItem:
				if ( PicHandle picture = (PicHandle) item->handle )
				{
					DrawPicture( picture, &bounds );
				}
				else
				{
					QDGlobals& qd = get_QDGlobals();
					
					FillRect( &bounds, &qd.ltGray );
					FrameRect( &bounds );
				}
				break;
			
			default:
				break;
		}
		
		item = next( item );
		
		++item_index;
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
	short n_items_1 = dialog_item_count_minus_one( items );
	
	DialogItem* item = first_dialog_item( items );
	
	do
	{
		item = next( item );
	}
	while ( --n_items_1 >= 0 );
	
	const short added_length = sizeof (DialogItem)
	                         + sizeof (ResID);  // 16 bytes
	
	const Size old_size = (char*) item - *items;
	
	SetHandleSize( items, old_size + added_length );
	
	item = (DialogItem*) (*items + old_size);
	
	item->handle = NULL;
	item->bounds = alert_icon_bounds;
	item->type   = iconItem + itemDisable;
	item->length = sizeof (ResID);
	
	item_ResID( item ) = icon_id;
	
	++dialog_item_count_minus_one( items );
}

static
void DITL_append_userItem( Handle items, UserItemUPP proc, const Rect& bounds )
{
	short n_items_1 = dialog_item_count_minus_one( items );
	
	DialogItem* item = first_dialog_item( items );
	
	do
	{
		item = next( item );
	}
	while ( --n_items_1 >= 0 );
	
	const short added_length = sizeof (DialogItem);  // 14 bytes
	
	const Size old_size = (char*) item - *items;
	
	SetHandleSize( items, old_size + added_length );
	
	item = (DialogItem*) (*items + old_size);
	
	item->handle = (Handle) proc;
	item->bounds = bounds;
	item->type   = userItem + itemDisable;
	item->length = 0;
	
	++dialog_item_count_minus_one( items );
}

static
pascal void default_button_outline( DialogRef dialog, short index )
{
	DialogPeek d = (DialogPeek) dialog;
	
	const DialogItem* item = first_dialog_item( d->items );
	
	if ( d->aDefItem > 1 )
	{
		item = next( item );
	}
	
	if ( item->type == ctrlItem + btnCtrl )
	{
		Rect bounds = item->bounds;
		
		InsetRect( &bounds, -4, -4 );
		
		PenSize( 3, 3 );
		FrameRoundRect( &bounds, 16, 16 );
		PenNormal();
	}
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
	
	Rect bounds = alert[0]->boundsRect;
	
	const short itemsID = alert[0]->itemsID;
	const short stages  = alert[0]->stages;
	
	ReleaseResource( h );
	
	if ( *(long*) &bounds.top == 0 )
	{
		QDGlobals& qd = get_QDGlobals();
		
		const Rect& screen = qd.screenBits.bounds;
		
		const short dh = (screen.right - screen.left - bounds.right) / 2;
		const short dv = (screen.bottom - screen.top - bounds.bottom) / 3;
		
		OffsetRect( &bounds, dh, dv );
	}
	
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
	
	DITL_append_userItem( h, &default_button_outline, dialog->portRect );
	
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
	
	if ( WindowRef window = FrontWindow() )
	{
		WindowPeek w = (WindowPeek) window;
		
		if ( w->windowKind == dialogKind )
		{
			DialogPeek d = (DialogPeek) window;
			
			short n_items_1 = dialog_item_count_minus_one( d->items );
			
			DialogItem* item = first_dialog_item( d->items );
			
			do
			{
				switch ( item->type & 0x7F )
				{
					case statText:
					case editText:
						DisposeHandle( item->handle );
						
						item->handle = expand_param_text( &item->length );
						break;
					
					default:
						break;
				}
				
				item = next( item );
			}
			while ( --n_items_1 >= 0 );
		}
	}
}

pascal void GetDItem_patch( DialogRef  dialog,
                            short      i,
                            short*     type,
                            Handle*    h,
                            Rect*      box )
{
	DialogPeek d = (DialogPeek) dialog;
	
	const DialogItem* item = first_dialog_item( d->items );
	
	while ( --i > 0 )
	{
		item = next( item );
	}
	
	*type = item->type;
	*h    = item->handle;
	*box  = item->bounds;
}

pascal void SetDItem_patch( DialogRef    dialog,
                            short        i,
                            short        type,
                            Handle       h,
                            const Rect*  box )
{
	DialogPeek d = (DialogPeek) dialog;
	
	short n_items_1 = dialog_item_count_minus_one( d->items );
	
	DialogItem* item = first_dialog_item( d->items );
	
	while ( --i > 0 )
	{
		item = next( item );
	}
	
	item->handle = h;
	item->bounds = *box;
	item->type   = type;
}

pascal void GetIText_patch( Handle h, Str255 text )
{
	Size size = GetHandleSize( h );
	
	if ( size > 255 )
	{
		size = 255;
	}
	
	text[ 0 ] = size;
	
	fast_memcpy( text + 1, *h, size );
}

pascal void SetIText_patch( Handle h, const unsigned char* text )
{
	const Size len = text[ 0 ];
	
	if ( GetHandleSize( h ) < len )
	{
		SetHandleSize( h, len );
	}
	
	fast_memcpy( *h, text + 1, len );
	
	// TODO:  Invalidate the text rect
}

pascal void SelIText_patch( GrafPort*  dialog,
                            short      item,
                            short      start,
                            short      end )
{
}

pascal short FindDItem_patch( DialogRef dialog, Point pt )
{
	DialogPeek d = (DialogPeek) dialog;
	
	// item count minus one
	short n_items_1 = dialog_item_count_minus_one( d->items );
	
	const DialogItem* item = first_dialog_item( d->items );
	
	short item_index = 0;
	
	do
	{
		const Rect& r = item->bounds;
		
		if ( PtInRect( pt, &item->bounds ) )
		{
			return item_index;
		}
		
		item = next( item );
	}
	while ( ++item_index <= n_items_1 );
	
	return -1;
}
