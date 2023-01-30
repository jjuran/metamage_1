/*	================
 *	Scroller_beta.cc
 *	================
 */

#include "Pedestal/Scroller_beta.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/scoped_clipRect.hh"

// Pedestal
#include "Pedestal/Scrollbar.hh"


namespace Pedestal
{
	
	static inline
	int
	min( int a, int b )
	{
		return b < a ? b : a;
	}
	
	static inline
	int
	max( int a, int b )
	{
		return a > b ? a : b;
	}
	
	static inline
	bool PtInRect( Point pt, const Rect& rect )
	{
		return ::PtInRect( pt, &rect );
	}
	
	static int NewVOffset( const ScrollerAPI& scroller, char c )
	{
		if ( c == kHomeCharCode )
		{
			return 0;
		}
		
		const short viewHeight = scroller.ViewHeight();
		
		const int max_voffset = max( scroller.ClientHeight() - viewHeight, 0 );
		
		if ( c == kEndCharCode )
		{
			return max_voffset;
		}
		
		const int voffset = scroller.GetVOffset();
		
		const short page_distance = viewHeight - 1;
		
		switch ( c )
		{
			case kPageUpCharCode:
				return max( voffset - page_distance, 0 );
			
			case kPageDownCharCode:
				return min( voffset + page_distance, max_voffset );
			
			default:
				break;
		}
		
		return -1;
	}
	
	bool Scroller_KeyDown( ScrollerAPI& scroller, const EventRecord& event )
	{
		const short keyCode_and_char = event.message;
		
		// Only consider programmers' keys, not control characters
		if ( keyCode_and_char >= 0x7000 )
		{
			const char keyChar = event.message;
			
			const int voffset = NewVOffset( scroller, keyChar );
			
			if ( voffset >= 0 )
			{
				scroller.SetVOffset( voffset );
				
				return true;
			}
		}
		
		return false;
	}
	
	
	void ScrollFrame::ClickInLoop()
	{
		using namespace mac::qd;
		
		static RgnHandle tmp = NewRgn();
		
		scoped_clipRect clipRect( get_portRect( thePort() ), tmp );
		
		UpdateScrollbars();
	}
	
	void ScrollFrame::Activate( bool activating )
	{
		GetVertical  ().Activate( activating );
		GetHorizontal().Activate( activating );
		
		Subview().Activate( activating );
	}
	
	bool ScrollFrame::HitTest( const EventRecord& event )
	{
		Point where = event.where;
		
		GlobalToLocal( &where );
		
		return PtInRect( where, ApertureFromBounds( Bounds() ) );
	}
	
	bool ScrollFrame::MouseDown( const EventRecord& event )
	{
		if ( HitTest( event ) )
		{
			TEClickLoop_Scope scope( this );
			
			Subview().MouseDown( event );
			
			UpdateScrollbars();
			
			return true;
		}
		
		return false;
	}
	
	bool ScrollFrame::KeyDown( const EventRecord& event )
	{
		const bool result = Subview().KeyDown( event );
		
		if ( result )
		{
			UpdateScrollbars();
		}
		
		return result;
	}
	
	bool ScrollFrame::UserCommand( CommandCode code )
	{
		const bool result = Subview().UserCommand( code );
		
		if ( result )
		{
			UpdateScrollbars();
		}
		
		return result;
	}
	
	Rect ScrollFrame::ApertureFromBounds( const Rect& bounds )
	{
		Rect aperture = bounds;
		
		const short kScrollbarThickness = 16;
		
		const short kOverlap = 1;
		
		const short kFootprint = kScrollbarThickness - kOverlap;
		
		if ( GetVertical().Get() )
		{
			aperture.right -= kFootprint;
		}
		
		if ( GetHorizontal().Get() )
		{
			aperture.bottom -= kFootprint;
		}
		
		return aperture;
	}
	
}
