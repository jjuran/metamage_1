/*	===========
 *	AboutBox.cc
 *	===========
 */

#include "Pedestal/AboutBox.hh"

// Nitrogen
#include "Nitrogen/Icons.h"
#include "Nitrogen/QuickDraw.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	AboutBox::AboutBox( WindowClosure& closure )
	: 
		Base( NewWindowContext( N::OffsetRect( N::SetRect( 0, 0, 64, 64 ),
		                                       300,
		                                       200 ),
		                        "\pPedestal" ),
		      closure,
		      std::make_pair( N::ResID( 128 ),
		                      N::Make< RGBColor >( 0xDDDD ) ) )
	{
	}
	
}

