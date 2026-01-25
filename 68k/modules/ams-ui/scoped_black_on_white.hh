/*
	scoped_black_on_white.hh
	------------------------
*/

#ifndef SCOPEDBLACKONWHITE_HH
#define SCOPEDBLACKONWHITE_HH

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// iota
#include "iota/class.hh"


class scoped_black_on_white
{
	NON_COPYABLE( scoped_black_on_white )
	NO_NEW_DELETE
	
	private:
		GrafPtr its_port;
		
		long saved_fgColor;
		long saved_bkColor;
	
	public:
		scoped_black_on_white( GrafPtr port );
		
		~scoped_black_on_white()
		{
			its_port->fgColor = saved_fgColor;
			its_port->bkColor = saved_bkColor;
		}
};

#endif
