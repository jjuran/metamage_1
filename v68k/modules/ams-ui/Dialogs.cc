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

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"
#include "QDGlobals.hh"

// ams-ui
#include "modal_updating.hh"
#include "scoped_port.hh"


static short ANumber;
static short ACount = -1;

long CaretTime : 0x02F4;

WindowPeek WindowList  : 0x09D6;

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

static
DialogItem* get_nth_item( Handle items, short i )
{
	DialogItem* item = first_dialog_item( items );
	
	while ( --i > 0 )
	{
		item = next( item );
	}
	
	return item;
}

static inline
DialogItem* get_nth_item( DialogRef dialog, short i )
{
	DialogPeek d = (DialogPeek) dialog;
	
	return get_nth_item( d->items, i );
}

static inline
DialogItem* end_of_items( Handle items )
{
	const short n_items_1 = dialog_item_count_minus_one( items );
	
	return get_nth_item( items, n_items_1 + 2 );
}

static inline
const DialogItem* get_editField( DialogPeek d )
{
	const short i = d->editField + 1;
	
	return i ? get_nth_item( d->items, i ) : NULL;
}

static
short next_field( DialogPeek d )
{
	const short n_items_1 = dialog_item_count_minus_one( d->items );
	
	const DialogItem* item = first_dialog_item( d->items );
	
	short first = -1;
	
	short i = 0;
	
	for ( ;  i <= n_items_1;  ++i )
	{
		if ( (item->type & 0x7f) == editText )
		{
			first = i;
			break;
		}
		
		item = next( item );
	}
	
	if ( first < 0 )
	{
		return first;
	}
	
	const short editField = d->editField;
	
	while ( i < editField )
	{
		item = next( item );
		++i;
	}
	
	do
	{
		if ( ++i > n_items_1 )
		{
			return first;
		}
		
		item = next( item );
	}
	while ( (item->type & 0x7f) != editText );
	
	return i;
}

static
short prev_field( DialogPeek d )
{
	const short n_items_1 = dialog_item_count_minus_one( d->items );
	
	const DialogItem* item = first_dialog_item( d->items );
	
	const short editField = d->editField;
	
	short prev = -1;
	
	short i = 0;
	
	while ( i < editField )
	{
		if ( (item->type & 0x7f) == editText )
		{
			prev = i;
		}
		
		item = next( item );
		++i;
	}
	
	if ( prev >= 0 )
	{
		return prev;
	}
	
	short last = editField;
	
	while ( i < n_items_1 )
	{
		item = next( item );
		++i;
		
		if ( (item->type & 0x7f) == editText )
		{
			last = i;
		}
	}
	
	return last;
}

static
DialogItem* recover_dialog_item( DialogPeek d, Handle h )
{
	short n_items_1 = dialog_item_count_minus_one( d->items );
	
	DialogItem* item = first_dialog_item( d->items );
	
	do
	{
		if ( item->handle == h )
		{
			return item;
		}
		
		item = next( item );
	}
	while ( --n_items_1 >= 0 );
	
	return NULL;
}

static
DialogPeek recover_dialog( Handle h )
{
	WindowPeek w = WindowList;
	
	while ( w != NULL )
	{
		if ( w->windowKind == dialogKind  ||  w->windowKind < 0 )
		{
			DialogPeek d = (DialogPeek) w;
			
			if ( recover_dialog_item( d, h ) )
			{
				return d;
			}
		}
		
		w = w->nextWindow;
	}
	
	return NULL;
}


#pragma mark -
#pragma mark Initialization
#pragma mark -

pascal void InitDialogs_patch( void* proc )
{
}

pascal void ErrorSound_patch( void* proc )
{
	ERROR = "ErrorSound is unimplemented";
}

#pragma mark -
#pragma mark Creating and Disposing of Dialogs
#pragma mark -

static
void update_edit_record( TEHandle hTE, const DialogItem* item )
{
	hTE[0]->teLength = GetHandleSize( item->handle );
	hTE[0]->hText    = item->handle;
	
	TESetSelect( 0, 32767, hTE );
}

static
void make_edit_record( DialogPeek d, const DialogItem* item )
{
	TEHandle hTE = TENew( &item->bounds, &item->bounds );
	
	d->textH = hTE;
	
	DisposeHandle( hTE[0]->hText );
	
	update_edit_record( hTE, item );
}

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
	
	DialogItem* edit = NULL;
	DialogItem* item = first_dialog_item( items );
	
	short item_offset = 0;  // item index - 1
	short edit_offset = -1;
	
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
			
			case editText:
				if ( edit_offset < 0 )
				{
					edit_offset = item_offset;
					edit        = item;
				}
				// fall through
			case statText:
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
		
		++item_offset;
	}
	while ( --n_items_1 >= 0 );
	
	d->editField = edit_offset;
	
	if ( edit_offset >= 0 )
	{
		make_edit_record( d, edit );
	}
	
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
	
	if ( TEHandle hTE = d->textH )
	{
		hTE[0]->hText = NewHandle( 0 );
		
		TEDispose( hTE );
	}
	
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

static
void activate_editField( DialogRef dialog, bool activating )
{
	DialogPeek d = (DialogPeek) dialog;
	
	if ( TEHandle hTE = d->textH )
	{
		if ( activating )
		{
			if ( const DialogItem* item = get_editField( d ) )
			{
				hTE[0]->viewRect = item->bounds;
				hTE[0]->destRect = item->bounds;
				
				TEActivate( hTE );
			}
		}
		else
		{
			TEDeactivate( hTE );
		}
	}
}

static pascal
Boolean basic_filterProc( DialogRef dialog, EventRecord* event, short* itemHit )
{
	DialogPeek d = (DialogPeek) dialog;
	
	if ( event->what == keyDown )
	{
		switch ( (char) event->message )
		{
			case kEnterCharCode:
			case kReturnCharCode:
				if ( invoke_defItem( d ) )
				{
					*itemHit = d->aDefItem;
					return true;
				}
			
			default:
				break;
		}
	}
	
	return false;
}

static inline
bool blinks( TEHandle hTE )
{
	return hTE  &&  hTE[0]->active  &&  hTE[0]->selStart == hTE[0]->selEnd;
}

pascal void ModalDialog_patch( ModalFilterUPP filterProc, short* itemHit )
{
	modal_update_scope updating( filterProc == NULL );
	
	if ( filterProc == NULL )
	{
		filterProc = &basic_filterProc;
	}
	
	WindowRef window = FrontWindow();
	
	scoped_port thePort = window;
	
	WindowPeek w = (WindowPeek) window;
	DialogPeek d = (DialogPeek) window;
	
	while ( true )
	{
		const long sleep = blinks( d->textH ) ? CaretTime : 0xFFFFFFFF;
		
		EventRecord event;
		
		if ( WaitNextEvent( everyEvent, &event, sleep, NULL ) )
		{
			if ( event.what == mouseDown )
			{
				if ( ! PtInRgn( event.where, w->contRgn ) )
				{
					SysBeep( 6 );
					continue;
				}
			}
		}
		
		if ( filterProc( window, &event, itemHit ) )
		{
			return;
		}
		
		if ( DialogSelect( &event, &window, itemHit ) )
		{
			return;
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

static inline
DialogRef dialog_from_event( const EventRecord* event )
{
	WindowPeek w = (WindowPeek) event->message;
	
	return w->windowKind == dialogKind ? &w->port : NULL;
}

static
void advance_field( DialogRef dialog, bool reverse )
{
	DialogPeek d = (DialogPeek) dialog;
	
	const short target = reverse ? prev_field( d )
	                             : next_field( d );
	
	if ( target >= 0  &&  target != d->editField )
	{
		SelectDialogItemText( dialog, target + 1, 0, 32767 );
	}
}

pascal Boolean DialogSelect_patch( const EventRecord*  event,
                                   DialogRef*          dialogHit,
                                   short*              itemHit )
{
	switch ( event->what )
	{
		case updateEvt:
			if ( DialogRef dialog = dialog_from_event( event ) )
			{
				BeginUpdate( dialog );
				DrawDialog ( dialog );
				EndUpdate  ( dialog );
			}
			
			return false;
		
		case activateEvt:
			if ( DialogRef dialog = dialog_from_event( event ) )
			{
				activate_editField( dialog, event->modifiers & activeFlag );
			}
			
			return false;
		
		default:
			break;
	}
	
	const DialogRef dialog = FrontWindow();
	
	*dialogHit = dialog;
	
	DialogPeek d = (DialogPeek) dialog;
	
	if ( event->what == mouseDown )
	{
		scoped_port thePort = dialog;
		
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
				if ( (type & 0x7c) == editText )
				{
					bool extend = event->modifiers & shiftKey;
					
					if ( d->editField + 1 != item_index )
					{
						SelectDialogItemText( dialog, item_index, 0, 32767 );
						
						extend = false;
					}
					
					TEClick( pt, extend, d->textH );
				}
				
				if ( (type & 0x7c) == ctrlItem )
				{
					ControlRef control = (ControlRef) item->handle;
					
					if ( ! TrackControl( control, pt, NULL ) )
					{
						return false;
					}
				}
				
				if ( type & 0x80 )
				{
					return false;  // disabled
				}
				
				*itemHit = item_index;
				
				return true;
			}
			
			item = next( item );
		}
	}
	else if ( event->what == keyDown  ||  event->what == autoKey )
	{
		if ( TEHandle hTE = d->textH )
		{
			const char c = (char) event->message;
			
			if ( c == kTabCharCode )
			{
				advance_field( dialog, event->modifiers & shiftKey );
				return false;
			}
			
			TEKey( c, hTE );
			
			const DialogItem* item = get_editField( d );
			
			*itemHit = d->editField + 1;
			
			return ! (item->type & 0x80);
		}
	}
	else if ( TEHandle hTE = d->textH )
	{
		TEIdle( hTE );
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
				
				if ( d->editField + 1 == item_index )
				{
					TEUpdate( &bounds, d->textH );
					break;
				}
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
	DialogItem* item = end_of_items( items );
	
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
	DialogItem* item = end_of_items( items );
	
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
		
		const short dh = (screen.right - screen.left - bounds.right) / 2u;
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
	
	const DialogItem* item = get_nth_item( dialog, i );
	
	*type = item->type;
	*h    = item->handle;
	*box  = item->bounds;
	
	TEHandle hTE = d->textH;
	
	if ( hTE  &&  hTE[0]->hText == item->handle )
	{
		// Trim the handle so the caller knows the text length.
		
		SetHandleSize( hTE[0]->hText, hTE[0]->teLength );
	}
}

pascal void SetDItem_patch( DialogRef    dialog,
                            short        i,
                            short        type,
                            Handle       h,
                            const Rect*  box )
{
	DialogPeek d = (DialogPeek) dialog;
	
	DialogItem* item = get_nth_item( dialog, i );
	
	if ( d->editField == i - 1  &&  (type & 0x7f) != editText )
	{
		TEHandle hTE = d->textH;
		
		SetHandleSize( hTE[0]->hText, hTE[0]->teLength );
		
		d->editField = -1;
	}
	
	item->handle = h;
	item->bounds = *box;
	item->type   = type;
	
	if ( (type & 0x7f) == editText  &&  d->editField < 0 )
	{
		d->editField = i - 1;
		
		if ( d->textH == NULL )
		{
			scoped_port thePort = dialog;
			
			make_edit_record( d, item );
		}
		else
		{
			update_edit_record( d->textH, item );
		}
	}
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
	
	SetHandleSize( h, len );
	
	fast_memcpy( *h, text + 1, len );
	
	DialogPeek d = recover_dialog( h );
	
	if ( d == NULL )
	{
		ERROR = "SetIText can't recover DialogRef to set text: ", CSTR( text );
		return;
	}
	
	TEHandle hTE = d->textH;
	
	if ( hTE  &&  hTE[0]->hText == h )
	{
		hTE[0]->teLength = len;
		
		TESetSelect( 0, len, hTE );
		
		return;
	}
	
	scoped_port thePort = (DialogRef) d;
	
	DialogItem* item = recover_dialog_item( d, h );
	
	TETextBox( text + 1, len, &item->bounds, 0 );
}

pascal void SelIText_patch( GrafPort*  dialog,
                            short      item,
                            short      start,
                            short      end )
{
	DialogPeek d = (DialogPeek) dialog;
	
	TEHandle hTE = d->textH;
	
	if ( hTE == NULL )
	{
		return;
	}
	
	const bool active = hTE[0]->active;
	
	if ( active )
	{
		TEDeactivate( hTE );
	}
	
	if ( d->editField != item - 1 )
	{
		d->editField = item - 1;
		
		const DialogItem* edit = get_editField( d );
		
		hTE[0]->viewRect = edit->bounds;
		hTE[0]->destRect = edit->bounds;
		
		SetHandleSize( hTE[0]->hText, hTE[0]->teLength );
		
		hTE[0]->teLength = GetHandleSize( edit->handle );
		hTE[0]->hText    = edit->handle;
	}
	
	TESetSelect( start, end, hTE );
	
	if ( active )
	{
		TEActivate( hTE );
	}
}

pascal void HideDItem_patch( GrafPort* dialog, short i )
{
	DialogPeek d = (DialogPeek) dialog;
	
	DialogItem* item = get_nth_item( dialog, i );
	
	if ( item->bounds.left >= 8192 )
	{
		return;
	}
	
	if ( (item->type & 0x7c) == ctrlItem )
	{
		HideControl( (ControlRef) item->handle );
	}
	else
	{
		if ( (item->type & 0x7f) == editText )
		{
			WARNING = "hiding an editText item without checking if it's active";
		}
		
		scoped_port thePort = dialog;
		
		EraseRect( &item->bounds );
		InvalRect( &item->bounds );
	}
	
	item->bounds.left  += 16384;
	item->bounds.right += 16384;
}

pascal void ShowDItem_patch( GrafPort* dialog, short i )
{
	DialogPeek d = (DialogPeek) dialog;
	
	DialogItem* item = get_nth_item( dialog, i );
	
	if ( item->bounds.left < 8192 )
	{
		return;
	}
	
	if ( (item->type & 0x7c) == ctrlItem )
	{
		ShowControl( (ControlRef) item->handle );
	}
	else
	{
		if ( (item->type & 0x7f) == editText  &&  d->editField < 0 )
		{
			WARNING = "showing a lone editText item without making it active";
		}
		
		scoped_port thePort = dialog;
		
		InvalRect( &item->bounds );
	}
	
	item->bounds.left  -= 16384;
	item->bounds.right -= 16384;
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
