/*	================
 *	Scroller_beta.hh
 *	================
 */

#ifndef PEDESTAL_SCROLLERBETA_HH
#define PEDESTAL_SCROLLERBETA_HH

// Pedestal
#include "Pedestal/Superview.hh"


namespace Pedestal
{
	
	class ScrollerAPI
	{
		public:
			virtual short ViewWidth () const = 0;
			virtual short ViewHeight() const = 0;
			
			virtual int ClientWidth () const = 0;
			virtual int ClientHeight() const = 0;
			
			virtual int GetHOffset() const = 0;
			virtual int GetVOffset() const = 0;
			
			virtual void SetHOffset( int h ) = 0;
			virtual void SetVOffset( int v ) = 0;
	};
	
	class Scroller : public Superview, public ScrollerAPI
	{
		public:
			bool KeyDown( const EventRecord& event );
			
			virtual void Scroll( int dh, int dv ) = 0;
	};
	
	class ScrollFrame : public Superview
	{
		public:
			virtual void ApertureHook( Rect& aperture )  {}
			
			void Draw( const Rect& bounds );
	};
	
}

#endif

