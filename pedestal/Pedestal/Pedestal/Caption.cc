/*	==========
 *	Caption.cc
 *	==========
 */

#include "Pedestal/Caption.hh"

// Nitrogen
#include "Nitrogen/TextEdit.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void Caption::Draw( const Rect& bounds )
	{
		Rect newBounds = bounds;
		
		if ( !Wrapped() )
		{
			newBounds.right = 30000;
		}
		
		N::TETextBox( Text(), newBounds );
	}
	
}

