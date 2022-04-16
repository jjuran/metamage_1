/*
	scrolling.hh
	------------
*/

#ifndef SCROLLING_HH
#define SCROLLING_HH

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif


struct scroll_action_rec;

typedef void (*scroll_callback)( const scroll_action_rec& action, short value );

struct scroll_action_rec
{
	scroll_callback scroll_to;
	
	short step;  // should always be 1
	short page;  // the number of visible cells at most, maybe one fewer
};

pascal
void scrollbar_actionProc( ControlRef control, short part );

#endif
