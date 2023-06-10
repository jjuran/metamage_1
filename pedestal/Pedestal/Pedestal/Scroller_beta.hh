/*	================
 *	Scroller_beta.hh
 *	================
 */

#ifndef PEDESTAL_SCROLLERBETA_HH
#define PEDESTAL_SCROLLERBETA_HH

// Pedestal
#ifndef PEDESTAL_SUPERVIEW_HH
#include "Pedestal/Superview.hh"
#endif


namespace Pedestal
{
	
	class Scrollbar;
	
	
	class ScrollerAPI
	{
		public:
			virtual ~ScrollerAPI()
			{
			}
			
			virtual short ViewWidth () const = 0;
			virtual short ViewHeight() const = 0;
			
			virtual int ClientWidth () const = 0;
			virtual int ClientHeight() const = 0;
			
			virtual int GetHOffset() const = 0;
			virtual int GetVOffset() const = 0;
			
			virtual void SetHOffset( int h ) = 0;
			virtual void SetVOffset( int v ) = 0;
	};
	
	bool Scroller_KeyDown( ScrollerAPI& scroller, const EventRecord& event );
	
	class ScrollFrame : public Superview
	{
		public:
			virtual void UpdateScrollbars()  {}
			
			virtual const Rect& Bounds() const = 0;
			
			virtual Scrollbar& GetHorizontal() = 0;
			virtual Scrollbar& GetVertical  () = 0;
			
			Rect ApertureFromBounds( const Rect& bounds );
			
			void Activate( bool activating );
			
			bool HitTest( const EventRecord& event );
			
			bool MouseDown( const EventRecord& event );
			
			bool KeyDown( const EventRecord& event );
			
			bool UserCommand( CommandCode code );
	};
	
}

#endif
