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
		N::TETextBox( Text(), bounds );
	}
	
}

