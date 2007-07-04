/*	================
 *	ScrollingView.hh
 *	================
 */

#ifndef PEDESTAL_SCROLLINGVIEW_HH
#define PEDESTAL_SCROLLINGVIEW_HH

// Universal Interfaces
#ifndef __EVENTS__
#include <Events.h>
#endif

// Nucleus
#include "Nucleus/Saved.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"

// Pedestal
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	
	
	class ScopedOrigin
	{
		private:
			NN::Saved< N::Clip_Value > savedClip;  // automatically save and restore the clip region
			
			void Setup( short h, short v, RgnHandle copyOfClipRegion );
		
		public:
			// Temporarily adjust the origin
			ScopedOrigin( short h, short v )
			{
				Setup( h, v, N::GetClip() );
			}
			
			ScopedOrigin( Point origin )
			{
				Setup( origin.h, origin.v, N::GetClip() );
			}
			
			// Temporarily adjust the origin and set a new clip region
			ScopedOrigin( short h, short v, RgnHandle newClip )
			{
				Setup( h, v, N::CopyRgn( newClip ) );
			}
			
			// Restore (assumed) zero origin (and the original clip region)
			~ScopedOrigin()  { N::SetOrigin( 0, 0 ); }
	};
	
	template < class SubViewType >
	class ScrollingView : public View
	{
		private:
			Rect bounds;
			Point scrollPosition;
			SubViewType mySubView;
		
		public:
			typedef typename SubViewType::Initializer Initializer;
			
			ScrollingView( const Rect& bounds, Initializer init = Initializer() )
			: 
				bounds( bounds ), 
				scrollPosition( N::SetPt( 0, 0 ) ), 
				mySubView( bounds, init )
			{}
			
			SubViewType const& SubView() const  { return mySubView; }
			SubViewType      & SubView()        { return mySubView; }
			
			const Rect& Bounds() const  { return bounds; }
			
			void Resize( const Rect& newBounds )
			{
				/*
				bounds.right = bounds.left + width;
				bounds.bottom = bounds.top + height;
				*/
				
				bounds = newBounds;
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
			
			void Update()
			{
				// FIXME:  This (perhaps unnecessarily) erases the subview content.
				//N::EraseRect( Bounds() );
				
				Rect subviewBounds = SubView().Bounds();
				
				Rect bottom = Bounds();
				bottom.top = subviewBounds.bottom - scrollPosition.v;
				
				Rect right = Bounds();
				right.left = subviewBounds.right - scrollPosition.h;
				
				N::EraseRect( bottom );
				N::EraseRect( right  );
				
				ScopedOrigin scopedOrigin( ScrollPosition() );
				
				SubView().Update();
			}
			
			void Activate( bool activating )
			{
				SubView().Activate( activating );
			}
			
			bool SetCursor( Point location, RgnHandle mouseRgn )
			{
				if ( N::PtInRect( location, SubView().Bounds() ) )
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
	inline Point ScrollPosition( const ScrollingView< SubViewType >& scroll )
	{
		return scroll.ScrollPosition();
	}
	
	template < class SubViewType >
	inline Point ScrollableArea( const ScrollingView< SubViewType >& scroll )
	{
		return NX::RectSize( scroll.SubView().Bounds() );
	}
	
	template < class SubViewType >
	inline Point ScrollableRange( const ScrollingView< SubViewType >& scroll )
	{
		return ScrollableArea( scroll );
	}
	
	template < class SubViewType >
	inline Point ViewableRange( const ScrollingView< SubViewType >& scroll )
	{
		return NX::RectSize( Bounds( scroll ) );
	}
	
	template < class SubViewType >
	inline void Resize( ScrollingView< SubViewType >& scroll, short width, short height )
	{
		scroll.Resize( width, height );
		
		N::ClipRect( Bounds( scroll ) );
	}
	
	template < class SubViewType >
	inline void ScrollView( ScrollingView< SubViewType >& scroll, short dh, short dv, bool updateNow )
	{
		scroll.Scroll( dh, dv, updateNow );
	}
	
	
	template < class SubViewType >
	void ScrollingView< SubViewType >::Scroll( short dh, short dv, bool updateNow )
	{
		using namespace NN::Operators;
		scrollPosition = scrollPosition + N::SetPt( dh, dv );
		
		NN::Owned< RgnHandle > updateRegion = N::ScrollRect( Bounds(), -dh, -dv );
		
		if ( updateNow )
		{
			// Update immediately
			
			NN::Saved< N::Clip_Value > savedClip( updateRegion );
			
			N::EraseRgn( updateRegion );
			Update();
		}
		else
		{
			N::InvalRgn( updateRegion );
		}
	}
	
}

#endif

