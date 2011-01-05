/*	============
 *	Scrollbar.hh
 *	============
 */

#ifndef PEDESTAL_SCROLLBAR_HH
#define PEDESTAL_SCROLLBAR_HH

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif


namespace Pedestal
{
	
	class Scrollbar
	{
		private:
			ControlRef itsControl;
		
		public:
			Scrollbar() : itsControl()
			{
			}
			
			ControlRef Get() const  { return itsControl; }
			
			void Create( const Rect& bounds, const void* refCon );
			
			void UpdateBounds( const Rect& bounds );
			
			void Clear();
			
			void SetValue( short value );
			
			void Adjust( int clientLength, int offset, int apertureLength );
			
			void Activate( bool activating );
	};
	
}

#endif

