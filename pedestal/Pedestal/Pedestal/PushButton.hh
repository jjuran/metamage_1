/*	=============
 *	PushButton.hh
 *	=============
 */

#ifndef PEDESTAL_PUSHBUTTON_HH
#define PEDESTAL_PUSHBUTTON_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Nitrogen
#include "Nitrogen/RefCon.h"

// Pedestal
#include "Pedestal/Control.hh"


namespace Pedestal {
	
	typedef void ( *ButtonAction )( N::RefCon refCon );
	
	class PushButton : public Control
	{
		private:
			ButtonAction action;
			N::RefCon refCon;
		
		public:
			struct Initializer
			{
				Initializer( ConstStr255Param title, ButtonAction action, N::RefCon refCon ) 
				  :	title( title ), action( action ), refCon( refCon )  {}
				
				ConstStr255Param title;
				ButtonAction action;
				N::RefCon refCon;
			};
			
			PushButton( const Rect& bounds, const Initializer& init );
			
			static void TrackControl( ControlRef control, ControlPartCode part, Point point );
	};
	
}

#endif

