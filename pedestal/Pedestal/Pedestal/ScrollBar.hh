/*	============
 *	Scrollbar.hh
 *	============
 */

#ifndef PEDESTAL_SCROLLBAR_HH
#define PEDESTAL_SCROLLBAR_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Pedestal
#include "Pedestal/Control.hh"
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	class Scrollbar : public View
	{
		private:
			ControlRef control;
			Control_Hooks controlHooks;
		
		public:
			Scrollbar( const Rect&       bounds,
			           N::ControlProcID  procID,
			           N::RefCon         refCon,
			           ControlTracker    tracker );
			
			ControlRef Get() const  { return control; }
			
			void Activate( bool activating );
	};
	
	
	inline short ScrollbarThickness()
	{
		return 16;
	}
	
	inline short ScrollbarOverlap()
	{
		return 1;
	}
	
	inline short ScrollbarProfile()
	{
		return ScrollbarThickness() - ScrollbarOverlap();
	}
	
}

#endif

