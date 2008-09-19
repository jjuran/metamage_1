/*	=============
 *	PushButton.hh
 *	=============
 */

#ifndef PEDESTAL_PUSHBUTTON_HH
#define PEDESTAL_PUSHBUTTON_HH

// Nitrogen
#include "Nitrogen/RefCon.h"

// Pedestal
#include "Pedestal/Control.hh"


namespace Pedestal
{
	
	typedef void ( *ButtonAction )( Nitrogen::RefCon refCon );
	
	class PushButton : public Control
	{
		private:
			ButtonAction action;
			Nitrogen::RefCon refCon;
		
		public:
			struct Initializer
			{
				Initializer( ConstStr255Param title, ButtonAction action, Nitrogen::RefCon refCon ) 
				  :	title( title ), action( action ), refCon( refCon )  {}
				
				ConstStr255Param title;
				ButtonAction action;
				Nitrogen::RefCon refCon;
			};
			
			PushButton( const Rect& bounds, const Initializer& init );
			
			static void TrackControl( ControlRef control, ControlPartCode part, Point point );
	};
	
}

#endif

