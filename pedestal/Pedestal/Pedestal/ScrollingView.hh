/*	================
 *	ScrollingView.hh
 *	================
 */

#ifndef PEDESTAL_SCROLLINGVIEW_HH
#define PEDESTAL_SCROLLINGVIEW_HH

// Universal Interfaces
#include <Events.h>

// Nucleus
#include "Nucleus/Saved.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"

// Pedestal
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/ScrollableBase.hh"


namespace Pedestal
{
	
	class ScopedOrigin
	{
		private:
			Nucleus::Saved< Nitrogen::Clip_Value > savedClip;  // automatically save and restore the clip region
			
			void Setup( short h, short v, RgnHandle copyOfClipRegion );
		
		public:
			// Temporarily adjust the origin
			ScopedOrigin( short h, short v )
			{
				Setup( h, v, Nitrogen::GetClip() );
			}
			
			ScopedOrigin( Point origin )
			{
				Setup( origin.h, origin.v, Nitrogen::GetClip() );
			}
			
			// Temporarily adjust the origin and set a new clip region
			ScopedOrigin( short h, short v, RgnHandle newClip )
			{
				Setup( h, v, Nitrogen::CopyRgn( newClip ) );
			}
			
			// Restore (assumed) zero origin (and the original clip region)
			~ScopedOrigin()  { Nitrogen::SetOrigin( 0, 0 ); }
	};
	
	template < class SubViewType >
	class ScrollingView : public ScrollableBase
	{
		private:
			Rect bounds;
			Point scrollPosition;
			SubViewType mySubView;
		
		public:
			ScrollingView( const Rect& bounds )
			: 
				bounds( bounds ), 
				scrollPosition( Nitrogen::SetPt( 0, 0 ) ), 
				mySubView( bounds, init )
			{}
			
			SubViewType const& SubView() const  { return mySubView; }
			SubViewType      & SubView()        { return mySubView; }
			
			const Rect& Bounds() const  { return bounds; }
			
			void Resize( short width, short height )
			{
				bounds.right = bounds.left + width;
				bounds.bottom = bounds.top + height;
			}
			
			Point ViewableRange() const
			{
				return NitrogenExtras::RectSize( Bounds() );
			}
			
			Point ScrollableRange() const
			{
				return NitrogenExtras::RectSize( SubView().Bounds() );
			}
			
			Point ScrollPosition() const  { return scrollPosition; }
			
			void Scroll( short dh, short dv, bool updateNow = false );
			
			void Idle( const EventRecord& event )
			{
				ScopedOrigin scopedOrigin( ScrollPosition() );
				
				SubView().Idle( event );
			}
			
			void MouseDown( const EventRecord& event )
			{
				SubView().MouseDown( event );
			}
			
			bool KeyDown( const EventRecord& event )
			{
				return SubView().KeyDown( event );
			}
			
			void Draw( const Rect& bounds )
			{
				// FIXME:  This (perhaps unnecessarily) erases the subview content.
				//Nitrogen::EraseRect( bounds );
				
				Rect subviewBounds = SubView().Bounds();
				
				Rect bottom = bounds;
				bottom.top = subviewBounds.bottom - scrollPosition.v;
				
				Rect right = bounds;
				right.left = subviewBounds.right - scrollPosition.h;
				
				Nitrogen::EraseRect( bottom );
				Nitrogen::EraseRect( right  );
				
				ScopedOrigin scopedOrigin( ScrollPosition() );
				
				SubView().Draw( subviewBounds );
			}
			
			void Activate( bool activating )
			{
				SubView().Activate( activating );
			}
			
			bool SetCursor( Point location, RgnHandle mouseRgn )
			{
				if ( Nitrogen::PtInRect( location, SubView().Bounds() ) )
				{
					return SubView().SetCursor( location, mouseRgn );
				}
				
				return false;
			}
			
			bool UserCommand( MenuItemCode code )
			{
				return SubView().UserCommand( code );
			}
			
	};
	
	template < class SubViewType >
	inline Rect Bounds( const ScrollingView< SubViewType >& scroll )
	{
		return scroll.Bounds();
	}
	
	template < class SubViewType >
	inline void Resize( ScrollingView< SubViewType >& scroll, short width, short height )
	{
		scroll.Resize( width, height );
		
		Nitrogen::ClipRect( Bounds( scroll ) );
	}
	
	
	template < class SubViewType >
	void ScrollingView< SubViewType >::Scroll( short dh, short dv, bool updateNow )
	{
		using namespace Nucleus::Operators;
		
		scrollPosition = scrollPosition + Nitrogen::SetPt( dh, dv );
		
		Nucleus::Owned< RgnHandle > updateRegion = Nitrogen::ScrollRect( Bounds(), -dh, -dv );
		
		if ( updateNow )
		{
			// Update immediately
			
			Nucleus::Saved< Nitrogen::Clip_Value > savedClip( updateRegion );
			
			Nitrogen::EraseRgn( updateRegion );
			
			Draw( Bounds() );
		}
		else
		{
			Nitrogen::InvalRgn( updateRegion );
		}
	}
	
}

#endif

