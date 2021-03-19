/*
	Lists.cc
	--------
*/

#include "Lists.hh"

// Mac OS
#ifndef __LISTS__
#include <Lists.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"

// ams-ui
#include "utility_region.hh"


#pragma exceptions off


static inline
bool operator==( const Point& a, const Point& b )
{
	return *(long*) &a == *(long*) &b;
}

static
void set_control_bounds( ControlRef control, const Rect& bounds )
{
	HideControl( control );
	
	control[0]->contrlRect = bounds;
	
	ShowControl( control );
}

class List_drawing
{
	private:
		GrafPtr    its_saved_port;
		RgnHandle  its_saved_clip;
		
		// non-copyable
		List_drawing           ( const List_drawing& );
		List_drawing& operator=( const List_drawing& );
		
		void initialize( const ListRec& list, RgnHandle clipRgn );
	
	public:
		List_drawing( const ListRec& list, RgnHandle clipRgn )
		{
			initialize( list, clipRgn );
		}
		
		List_drawing( const ListRec& list )
		{
			initialize( list, rectangular_utility_region( list.rView ) );
		}
		
		~List_drawing();
};

void List_drawing::initialize( const ListRec& list, RgnHandle clipRgn )
{
	GrafPtr& thePort = *get_addrof_thePort();
	
	its_saved_port = thePort;
	
	thePort = list.port;
	
	its_saved_clip = thePort->clipRgn;
	
	thePort->clipRgn = clipRgn;
}

List_drawing::~List_drawing()
{
	GrafPtr& thePort = *get_addrof_thePort();
	
	thePort->clipRgn = its_saved_clip;
	thePort          = its_saved_port;
}

static inline
bool is_selected( const ListRec& list, short i )
{
	return list.cellArray[ i ] < 0;
}

static inline
void mark_selected( ListRec& list, short i )
{
	list.cellArray[ i ] |= 0x8000;
}

static inline
void mark_unselected( ListRec& list, short i )
{
	list.cellArray[ i ] &= ~0x8000;
}

static
void draw_list_cells( const ListRec& list, const Rect& range, RgnHandle clip )
{
	if ( ! clip )
	{
		clip = rectangular_utility_region( list.rView );
	}
	
	const short n_rows = list.dataBounds.bottom - list.dataBounds.top;
	const short n_cols = list.dataBounds.right - list.dataBounds.left;
	
	List_drawing scope( list, clip );
	
	for ( short row = range.top;  row < range.bottom;  ++row )
	{
		const short y = row - range.top;
		
		for ( short col = range.left;  col < range.right;  ++col )
		{
			const short x = col - range.left;
			
			const short top  = list.rView.top  + list.cellSize.v * y;
			const short left = list.rView.left + list.cellSize.h * x;
			
			MoveTo( left + list.indent.h,
			        top  + list.indent.v );
			
			const short i = row * n_cols + col;
			const short p = list.cellArray[ i     ] & 0x7fff;
			const short q = list.cellArray[ i + 1 ] & 0x7fff;
			
			DrawText( *list.cells, p, q - p );
			
			if ( is_selected( list, i ) )
			{
				const Rect rect =
				{
					top,
					left,
					top  + list.cellSize.v,
					left + list.cellSize.h,
				};
				
				InvertRect( &rect );
			}
		}
	}
}

static
pascal void LActivate_call( Boolean activating, ListHandle listH )
{
	ListRec& list = **listH;
	
	list.lActive = activating;
	
	const short part = activating ? kControlNoPart : kControlInactivePart;
	
	if ( list.vScroll )
	{
		HiliteControl( list.vScroll, part );
	}
	
	if ( list.hScroll )
	{
		HiliteControl( list.hScroll, part );
	}
}

static
pascal short LAddRow_call( short row_count, short row, ListHandle listH )
{
	if ( row_count <= 0 )
	{
		return -1;
	}
	
	const ListBounds& dataBounds = listH[0]->dataBounds;
	
	row -= dataBounds.top;
	
	const short n_rows = dataBounds.bottom - dataBounds.top;
	const short n_cols = dataBounds.right - dataBounds.left;
	
	if ( row > n_rows )
	{
		row = n_rows;
	}
	
	const int n_cells = (n_rows + row_count) * n_cols;
	
	const Size previous_size = GetHandleSize( (Handle) listH );
	const Size required_size = sizeof (ListRec) + n_cells * sizeof (short);
	
	if ( required_size > previous_size )
	{
		SetHandleSize( (Handle) listH, required_size );  // FIXME:  Error-check
	}
	
	ListRec& list = **listH;
	
	const short first = n_cols * row;
	const short count = n_cols * row_count;
	const short limit = first + count;
	
	fast_memmove( &list.cellArray[ limit ],
	              &list.cellArray[ first ],
	              (list.maxIndex + 1 - first) * sizeof (short) );
	
	const short offset = list.cellArray[ first ] & 0x7fff;
	
	for ( short i = first + 1;  i < limit;  ++i )
	{
		list.cellArray[ i ] = offset;
	}
	
	list.dataBounds.bottom += row_count;
	list.maxIndex          += count;
	
	return row + dataBounds.top;
}

static
pascal Boolean LClick_call( Point pt, EventModifiers mods, ListHandle listH )
{
	ListRec& list = **listH;
	
	const Rect& view = list.rView;
	
	if ( ! PtInRect( pt, &view ) )
	{
		return false;
	}
	
	const Rect& dataBounds = list.dataBounds;
	
	const UInt32 now = TickCount();
	const UInt32 prev_clock = list.clikTime;
	const Cell   prev_click = list.lastClick;
	
	const Cell clicked =
	{
		list.visible.top  + (pt.v - view.top  + 0u) / list.cellSize.v,
		list.visible.left + (pt.h - view.left + 0u) / list.cellSize.h,
	};
	
	const bool double_click = clicked == prev_click  &&  now - prev_clock < 8;
	
	list.clikTime  = now;
	list.clikLoc   = pt;
	list.lastClick = clicked;
	
	const short n_rows = dataBounds.bottom - dataBounds.top;
	const short n_cols = dataBounds.right - dataBounds.left;
	
	const short clicked_index = clicked.v * n_cols + clicked.h;
	
	List_drawing scope( list );
	
	if ( clicked_index < list.maxIndex )
	{
		if ( ! is_selected( list, clicked_index ) )
		{
			const short top  = view.top  + clicked.v * list.cellSize.v;
			const short left = view.left + clicked.h * list.cellSize.h;
			
			const Rect rect =
			{
				top,
				left,
				top  + list.cellSize.v,
				left + list.cellSize.h,
			};
			
			InvertRect( &rect );
		}
		
		// Mark the clicked cell unselected so we don't invert it below.
		
		mark_unselected( list, clicked_index );
	}
	
	for ( short row = dataBounds.top;  row < dataBounds.bottom;  ++row )
	{
		const short y = row - dataBounds.top;
		
		const short top = view.top + list.cellSize.v * y;
		
		for ( short col = dataBounds.left;  col < dataBounds.right;  ++col )
		{
			const short i = row * n_cols + col;
			
			if ( ! is_selected( list, i ) )
			{
				continue;
			}
			
			mark_unselected( list, i );
			
			const short x = col - dataBounds.left;
			
			const short left = view.left + list.cellSize.h * x;
			
			const Rect rect =
			{
				top,
				left,
				top  + list.cellSize.v,
				left + list.cellSize.h,
			};
			
			InvertRect( &rect );
		}
	}
	
	if ( clicked_index < list.maxIndex )
	{
		mark_selected( list, clicked_index );
	}
	
	return double_click;
}

static
pascal void LDelRow_call( short row_count, short row, ListHandle listH )
{
	ListRec& list = **listH;
	
	if ( row_count <= 0 )
	{
		list.dataBounds.bottom = list.dataBounds.top;
		
		list.maxIndex = 0;
		list.cellArray[ 0 ] = 0;
		
		SetHandleSize( list.cells, 0 );
		
		return;
	}
	
	row -= list.dataBounds.top;
	
	const short n_rows = list.dataBounds.bottom - list.dataBounds.top;
	const short n_cols = list.dataBounds.right - list.dataBounds.left;
	
	if ( row >= n_rows )
	{
		return;
	}
	
	if ( row_count > n_rows - row )
	{
		row_count = n_rows - row;
	}
	
	const short first = n_cols * row;
	const short count = n_cols * row_count;
	const short limit = first + count;
	
	const short begin = list.cellArray[ first ] & 0x7fff;
	const short end   = list.cellArray[ limit ] & 0x7fff;
	
	const long n_bytes = end - begin;
	
	if ( n_bytes )
	{
		Munger( list.cells, begin, NULL, n_bytes, "", 0 );
		
		for ( short i = limit;  i <= list.maxIndex;  ++i )
		{
			list.cellArray[ i ] -= n_bytes;
		}
	}
	
	list.dataBounds.bottom -= row_count;
	list.maxIndex          -= count;
	
	const long offset = sizeof (ListRec) + (first - 1) * sizeof (short);
	
	Munger( (Handle) listH, offset, NULL, count * sizeof (short), "", 0 );
}

static
pascal void LDispose_call( ListHandle listH )
{
	ListRec& list = **listH;
	
	if ( list.hScroll )
	{
		DisposeControl( list.hScroll );
	}
	
	if ( list.vScroll )
	{
		DisposeControl( list.vScroll );
	}
	
	DisposeHandle( list.cells );
	DisposeHandle( (Handle) listH );
}

static
void get_vscroll_bounds( const Rect& view, Rect* bounds )
{
	const short edge = view.right;
	
	bounds->top    = view.top - 1;
	bounds->left   = edge;
	bounds->bottom = view.bottom + 1;
	bounds->right  = edge + 16;
}

static
void get_hscroll_bounds( const Rect& view, Rect* bounds )
{
	const short edge = view.bottom;
	
	bounds->top    = edge;
	bounds->left   = view.left - 1;
	bounds->bottom = edge + 16;
	bounds->right  = view.right + 1;
}

static
void calc_visible( ListHandle listH )
{
	ListRec& list = **listH;
	
	const Rect& view = list.rView;
	
	const short view_height = view.bottom - view.top;
	const short view_width  = view.right - view.left;
	
	const short visible_height = (view_height - 1) / list.cellSize.v + 1;
	const short visible_width  = (view_width  - 1) / list.cellSize.h + 1;
	
	// No scrolling yet
	
	list.visible.bottom = visible_height;
	list.visible.right  = visible_width;
}

static
pascal ListHandle LNew_call( const Rect*        view,
                             const ListBounds*  dataBounds,
                             Point              cellSize,
                             short              procID,
                             WindowRef          window,
                             Boolean            draw,
                             Boolean            hasGrowBox,
                             Boolean            hScroll,
                             Boolean            vScroll )
{
	const short n_rows = dataBounds->bottom - dataBounds->top;
	const short n_cols = dataBounds->right - dataBounds->left;
	
	const int n_cells = n_rows * n_cols;
	
	Handle h = NewHandleClear( sizeof (ListRec) + n_cells * sizeof (short) );
	
	if ( ! h )
	{
		return NULL;
	}
	
	ListHandle listH = (ListHandle) h;
	
	ListRec& list = **listH;
	
	list.cells = NewHandle( 0 );
	
	if ( ! list.cells )
	{
		goto fail;
	}
	
	Rect bounds;
	
	if ( vScroll )
	{
		get_vscroll_bounds( *view, &bounds );
		
		list.vScroll = NewControl( window, &bounds, "\p", 1, 0, 0, 0, 16, 0 );
		
		if ( ! list.vScroll )
		{
			goto fail;
		}
	}
	
	if ( hScroll )
	{
		get_hscroll_bounds( *view, &bounds );
		
		list.hScroll = NewControl( window, &bounds, "\p", 1, 0, 0, 0, 16, 0 );
		
		if ( ! list.hScroll )
		{
			goto fail;
		}
	}
	
	if ( cellSize.h == 0 )
	{
		cellSize.h = view->right - view->left;
		
		if ( n_cols > 1 )
		{
			cellSize.h /= n_cols;
		}
	}
	
	
	if ( cellSize.v == 0 )
	{
		FontInfo fontInfo;
		GetFontInfo( &fontInfo );
		
		const short line_height = fontInfo.ascent
		                        + fontInfo.descent
		                        + fontInfo.leading;
		
		cellSize.v = line_height;
		
		list.indent.v = fontInfo.ascent;
	}
	
	list.indent.h = 4;
	
	list.rView      = *view;
	list.port       = window;
	list.cellSize   = cellSize;
	list.dataBounds = *dataBounds;
	list.maxIndex   = n_cells;
	
	calc_visible( listH );
	
	return listH;
	
fail:
	LDispose_call( listH );
	
	return NULL;
}

static
pascal void LSetCell_call( const void* p, short n, Cell cell, ListHandle listH )
{
	ListRec& list = **listH;
	
	const short n_rows = list.dataBounds.bottom - list.dataBounds.top;
	const short n_cols = list.dataBounds.right - list.dataBounds.left;
	
	const short i = cell.v * n_cols + cell.h;
	
	if ( ! PtInRect( cell, &list.dataBounds ) )
	{
		return;
	}
	
	const short o = list.cellArray[ i     ] & 0x7fff;
	const short q = list.cellArray[ i + 1 ] & 0x7fff;
	
	Munger( list.cells, o, NULL, q - o, p, n );
	
	const short d = n - (q - o);
	
	for ( short c = i;  c < list.maxIndex;  ++c )
	{
		list.cellArray[ c + 1 ] += d;
	}
	
	List_drawing scope( list );
	
	if ( PtInRect( cell, &list.visible ) )
	{
		const short y = cell.v - list.visible.top;
		const short x = cell.h - list.visible.left;
		
		MoveTo( list.rView.left + list.cellSize.h * x + list.indent.h,
		        list.rView.top  + list.cellSize.v * y + list.indent.v );
		
		DrawText( p, 0, n );
	}
}

static
pascal void LSize_call( short width, short height, ListHandle listH )
{
	ListRec& list = **listH;
	
	Rect rect = list.rView;  // previous view rect
	
	InvalRect( &rect );
	
	list.rView.right = list.rView.left + width;
	list.rView.bottom = list.rView.top + height;
	
	SectRect( &rect, &list.rView, &rect );  // valid intersection
	
	ValidRect( &rect );
	
	calc_visible( listH );
	
	if ( ControlRef control = list.vScroll )
	{
		get_vscroll_bounds( list.rView, &rect );
		set_control_bounds( control, rect );
		
		ValidRect( &rect );
	}
	
	if ( ControlRef control = list.hScroll )
	{
		get_hscroll_bounds( list.rView, &rect );
		set_control_bounds( control, rect );
		
		ValidRect( &rect );
	}
}

static
pascal void LUpdate_call( RgnHandle rgn, ListHandle listH )
{
	ListRec& list = **listH;
	
	if ( list.vScroll )
	{
		Draw1Control( list.vScroll );
	}
	
	if ( list.hScroll )
	{
		Draw1Control( list.hScroll );
	}
	
	Rect drawable = list.visible;
	
	SectRect( &drawable, &list.dataBounds, &drawable );
	
	EraseRect( &list.rView );
	
	draw_list_cells( list, drawable, NULL );
}

static
void unimplemented_call( short selector : __D0 )
{
	FATAL = "unimplemented List Manager call ", selector;
	
	asm { ILLEGAL }
}

static void* dispatch_table[] =
{
	(void*) LActivate_call,
	(void*) unimplemented_call,
	(void*) LAddRow_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) LClick_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) LDelRow_call,
	(void*) LDispose_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) LNew_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) unimplemented_call,
	(void*) LSetCell_call,
	(void*) unimplemented_call,
	(void*) LSize_call,
	(void*) LUpdate_call,
};

enum { dispatch_table_size = sizeof dispatch_table };

asm void Pack0_patch( short selector )
{
	MOVEA.L  (SP)+,A0
	MOVE.W   (SP)+,D0
	MOVE.L   A0,-(SP)
	
	CHK.W    #(dispatch_table_size - 4),D0
	
	LEA      dispatch_table,A0
	MOVEA.L  (A0,D0),A0
	
	JMP      (A0)
}
