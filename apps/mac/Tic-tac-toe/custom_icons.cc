/*
	custom_icons.cc
	---------------
*/

#include "custom_icons.hh"

// iota
#include "iota/bits.hh"


#define _ 0
#define X 1


enum
{
	icon_height = 5,
	row_height  = icon_height + 3,  // divider plus two blank scanlines
	grid_offset = 8,
};

static const IconRow icon_XOs[] =
{
	BIG32( _,_,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,_,X,_,X,_,_,X,_,_,X,_,X,_,_,X,_,_,X,_,X,_,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,_,X,_,X,_,_,X,_,_,X,_,X,_,_,X,_,_,X,_,X,_,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,_,_,_ ),
	
	BIG32( _,_,_,X,_,_,X,X,X,_,_,X,_,_,X,X,X,_,_,X,_,_,X,X,X,_,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,X,_,_,_,X,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,_,X,X,X,_,_,X,_,_,X,X,X,_,_,X,_,_,X,X,X,_,_,X,_,_,_,_ ),
};

static const IconRow icon_masks[] =
{
	BIG32( _,_,_,X,_,X,X,X,X,X,_,X,_,_,_,_,_,_,_,X,_,_,_,_,_,_,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,_,_,_,_,_,_,X,_,X,X,X,X,X,_,X,_,_,_,_,_,_,_,X,_,_,_,_ ),
	BIG32( _,_,_,X,_,_,_,_,_,_,_,X,_,_,_,_,_,_,_,X,_,X,X,X,X,X,_,X,_,_,_,_ ),
};

int inscribe_move_into_icon( unsigned index, IconRow* icon_rows, int state )
{
	int x = index % 3;
	int y = index / 3 * row_height + grid_offset;
	
	IconRow mask = icon_masks[ x ];
	
	for ( int i = 0;  i < icon_height;  ++i )
	{
		IconRow mini = icon_XOs[ state + i ];
		
		icon_rows[ y + i ] |= mini & mask;
	}
	
	return icon_height - state;
}
