/*	============
 *	Scrollbar.hh
 *	============
 */

#ifndef PEDESTAL_SCROLLBAR_HH
#define PEDESTAL_SCROLLBAR_HH

// Nitrogen
#include "Nitrogen/Controls.hh"


namespace Pedestal
{
	
	class Scrollbar
	{
		private:
			Nitrogen::ControlRef itsControl;
		
		public:
			Scrollbar() : itsControl()
			{
			}
			
			Nitrogen::ControlRef Get() const  { return itsControl; }
			
			void Create( const Rect& bounds, Nitrogen::RefCon refCon );
			
			void UpdateBounds( const Rect& bounds );
			
			void Clear();
			
			void SetValue( short value );
			
			void Adjust( int clientLength, int offset, int apertureLength );
			
			void Activate( bool activating );
	};
	
}

#endif

