/*
	custom_icons.hh
	---------------
*/

#ifndef TICTACTOE_CUSTOMICONS_HH
#define TICTACTOE_CUSTOMICONS_HH

#if __LP64__
typedef unsigned int IconRow;
#else
typedef unsigned long IconRow;
#endif

int inscribe_move_into_icon( unsigned index, IconRow* icon_rows, int state );

#endif
