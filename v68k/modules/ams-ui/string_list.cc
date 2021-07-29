/*
	string_list.cc
	--------------
*/

#include "string_list.hh"

// Mac OS
#ifndef __FONTS__
#include <Fonts.h>
#endif

// Standard C
#include <stddef.h>

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "raster_lock.hh"

// ams-ui
#include "utility_region.hh"


struct cell_data
{
	Str63 name;
};

struct string_list_record
{
	Rect       rView;
	short      font_ascent;
	short      line_height;
	short      n_vis_cells;
	short      scrolled_to;
	short      selection;
	short      cell_count;
	cell_data  cellArray[ 1 ];
};

static inline
short min( short a, short b )
{
	return b < a ? b : a;
}

string_list_handle new_string_list( const Rect& view )
{
	Handle h = NewHandleClear( sizeof (string_list_record) );
	
	string_list_handle slh = (string_list_handle) h;
	
	if ( slh )
	{
		string_list_record& list = **slh;
		
		list.rView = view;
		
		FontInfo fontInfo;
		GetFontInfo( &fontInfo );
		
		list.font_ascent = fontInfo.ascent;
		list.line_height = fontInfo.ascent
		                 + fontInfo.descent
		                 + fontInfo.leading;
		
		list.n_vis_cells = (view.bottom - view.top) / list.line_height;
	}
	
	return slh;
}

void dispose_string_list( string_list_handle slh )
{
	DisposeHandle( (Handle) slh );
}

static
short cell_capacity( Size handle_size )
{
	const short header_size = offsetof( string_list_record, cellArray );
	
	return (handle_size - header_size) / sizeof (cell_data);
}

void clear_string_list( string_list_handle slh )
{
	string_list_record& list = **slh;
	
	list.scrolled_to = 0;
	list.selection   = 0;
	list.cell_count  = 0;
}

void append_to_string_list( string_list_handle slh, const unsigned char* str )
{
	Size size = GetHandleSize( (Handle) slh );
	
	short capacity = cell_capacity( size );
	
	string_list_record* list = *slh;
	
	if ( capacity == list->cell_count )
	{
		size += capacity * sizeof (cell_data);
		
		SetHandleSize( (Handle) slh, size );
		
		list = *slh;
	}
	
	cell_data& cell = list->cellArray[ list->cell_count++ ];
	
	fast_memcpy( &cell, str, sizeof cell );
}

void draw_string_list( string_list_handle slh )
{
	string_list_record& list = **slh;
	
	Rect box = list.rView;
	
	GrafPtr& thePort = *get_addrof_thePort();
	
	RgnHandle savedClip = thePort->clipRgn;
	
	thePort->clipRgn = rectangular_utility_region( box );
	
	raster_lock lock;
	
	FrameRect( &box );
	InsetRect( &box, 1, 1 );
	EraseRect( &box );
	
	const short line_height = list.line_height;
	
	const short left = box.left + 3;
	const short base = box.top + list.font_ascent;
	
	if ( const short n = min( list.cell_count, list.n_vis_cells ) )
	{
		for ( short i = 0;  i < n;  ++i )
		{
			MoveTo( left, base + i * line_height );
			
			const short j = i + list.scrolled_to;
			
			DrawString( list.cellArray[ j ].name );
		}
		
		const unsigned short current_index = list.selection - list.scrolled_to;
		
		if ( current_index < list.n_vis_cells )
		{
			box.top += current_index * line_height;
			box.bottom = box.top + line_height;
			
			InvertRect( &box );
		}
	}
	
	thePort->clipRgn = savedClip;
}

void scroll_string_list_to( string_list_handle slh, short offset )
{
	string_list_record& list = **slh;
	
	if ( list.cell_count > list.n_vis_cells )
	{
		list.scrolled_to = offset;
		
		draw_string_list( slh );
	}
}

bool string_list_click( string_list_handle slh, Point pt )
{
	string_list_record& list = **slh;
	
	const short line_height = list.line_height;
	const short scrolled_to = list.scrolled_to;
	
	const short clicked_index = (pt.v - list.rView.top) / line_height;
	const short selected_cell = scrolled_to + clicked_index;
	
	if ( selected_cell < list.cell_count )
	{
		if ( selected_cell != list.selection )
		{
			const short base = list.rView.top + 1;
			
			Rect box;
			box.left  = list.rView.left  + 1;
			box.right = list.rView.right - 1;
			
			const unsigned short previous_index = list.selection - scrolled_to;
			
			if ( previous_index < list.n_vis_cells )
			{
				box.top = base + previous_index * line_height;
				box.bottom = box.top + line_height;
				
				InvertRect( &box );
			}
			
			box.top = base + clicked_index * line_height;
			box.bottom = box.top + line_height;
			
			InvertRect( &box );
			
			list.selection = selected_cell;
		}
	}
	
	return false;
}

const unsigned char* get_string_list_selection( string_list_handle slh )
{
	string_list_record& list = **slh;
	
	const short i = list.selection;
	
	return list.cell_count ? list.cellArray[ i ].name : NULL;
}
