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
	
	void Caption::Plot( const Rect& area )
	{
		N::TETextBox( text, area );
	}
	
}

