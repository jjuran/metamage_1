/*	==========
 *	Control.hh
 *	==========
 */

#ifndef PEDESTAL_CONTROL_HH
#define PEDESTAL_CONTROL_HH

#ifndef __CONTROLS__
#include <Controls.h>
#endif


namespace Pedestal
{
	
	void SetValueStretch( ControlRef control, short value );
	
	class Control
	{
		protected:
			ControlRef macControl;
		
		public:
			// Pane management
			//void SetMinimumClip(short min);
			//void SetMaximumClip(short max);
			//void SetValuePin(short val);
	};
	
}

#endif

