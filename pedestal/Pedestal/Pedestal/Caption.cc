/*	==========
 *	Caption.cc
 *	==========
 */

#include "Pedestal/Caption.hh"

// Nucleus
#include "Nucleus/Saved.h"

// Nitrogen
#include "Nitrogen/TextEdit.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	void Caption::Draw( const Rect& bounds )
	{
		NN::Saved< N::Clip_Value > savedClip;
		
		Rect newBounds = bounds;
		
		if ( !Wrapped() )
		{
			newBounds.right = 30000;
			
			N::ClipRect( bounds );
		}
		
		N::TETextBox( Text(), newBounds );
	}
	
}

