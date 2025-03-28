/*
	LDEF-0.cc
	---------
*/

// Mac OS
#ifndef __LISTS__
#include <Lists.h>
#endif


static
pascal void LDEF_0( short        message,
                    Boolean      select,
                    const Rect*  rect,
                    Cell         cell,
                    short        offset,
                    short        length,
                    ListHandle   listH );

asm int main()
{
	JMP      LDEF_0
}

pascal void LDEF_0( short        message,
                    Boolean      select,
                    const Rect*  rect,
                    Cell         cell,
                    short        offset,
                    short        length,
                    ListHandle   listH )
{
	ListRec& list = **listH;
	
	switch ( message )
	{
		case lInitMsg:
		case lCloseMsg:
			break;
		
		case lDrawMsg:
			MoveTo( rect->left + list.indent.h, rect->top + list.indent.v );
			
			DrawText( *list.cells, offset, length );
			
			if ( ! select )
			{
				break;
			}
			
			// fall through
		
		case lHiliteMsg:
			InvertRect( rect );
			break;
	}
}
