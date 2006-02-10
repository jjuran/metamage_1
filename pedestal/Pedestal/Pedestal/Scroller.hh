/*	===========
 *	Scroller.hh
 *	===========
 */

#ifndef PEDESTAL_SCROLLER_HH
#define PEDESTAL_SCROLLER_HH

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif

// Nitrogen Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen / Carbon support
#include "Nitrogen/Controls.h"

// Nitrogen Extras / ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Nitrogen Extras / Utilities
#include "Utilities/Quickdraw.h"
#include "Utilities/RectangleMath.h"
#include "Utilities/Saved.h"

// Pedestal
#include "Pedestal/View.hh"
#include "Pedestal/Scrollbar.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NX = NitrogenExtras;
	
	
	short ActualScrollbarLength( short viewLength, bool shortened );
	
	Rect VerticalScrollbarBounds  (const Rect& scrollerBounds, bool shortened);
	Rect HorizontalScrollbarBounds(const Rect& scrollerBounds, bool shortened);
	
	Rect Aperture( const Rect& scrollerBounds, bool vertical, bool horizontal );
	
	Point ScrollbarMaxima( Point scrollableRange, Point viewableRange, Point scrollPosition );
	
	short FigureScrollDistance( N::ControlPartCode part, short pageDistance );
	short SetControlValueFromClippedDelta( ControlRef control, short delta );
	
	
	class BoundedView : public View
	{
		private:
			Rect bounds;
		
		public:
			BoundedView(const Rect& bounds) : bounds( bounds )  {}
			
			const Rect& Bounds() const  { return bounds; }
			
			void Resize( short width, short height )
			{
				bounds.right = bounds.left + width;
				bounds.bottom = bounds.top + height;
			}
	};
	
	template < bool present >  struct Scrollbar_Traits;
	
	// present:  Is the scrollbar present?
	// Type:     The storage type of the scrollbar object.
	// profile:  The amount the scrollbar encroaches on the scroll view.
	
	template <>  struct Scrollbar_Traits< true >
	{
		static const bool present = true;
		typedef Scrollbar Type;
		static const short profile = 15;
	};
	
	template <>  struct Scrollbar_Traits< false >
	{
		static const bool present = false;
		
		struct Type
		{
			Type( const Rect&, N::RefCon, ControlTracker )  {}
			
			Type& Get()  { return *this; }
			
			void Activate( bool )  {}
		};
		
		static const short profile = 0;
	};
	
	using N::SetControlMaximum;
	
	static void SetControlViewSize( ControlRef control, long size )
	{
	#if TARGET_CPU_PPC
		
		if ( TARGET_API_MAC_CARBON || ::SetControlViewSize != (void*)kUnresolvedCFragSymbolAddress )
		{
			::SetControlViewSize( control, size );
		}
		
	#endif
	}
	
	static void InvalidateControl( ControlRef control )  { N::InvalRect( N::GetControlBounds( control ) ); }
	
	static void SetBounds         ( Scrollbar_Traits< false >::Type, Rect  )  {}
	static void SetControlMaximum ( Scrollbar_Traits< false >::Type, short )  {}
	static void SetValueStretch   ( Scrollbar_Traits< false >::Type, short )  {}
	static void SetControlViewSize( Scrollbar_Traits< false >::Type, long  )  {}
	static void InvalidateControl ( Scrollbar_Traits< false >::Type        )  {}
	
	enum
	{
		kVerticalScrollbarBit, 
		kHorizontalScrollbarBit, 
		
		kVerticalScrollbarMask   = 1 << kVerticalScrollbarBit, 
		kHorizontalScrollbarMask = 1 << kHorizontalScrollbarBit
	};
	
	enum ScrollbarConfig
	{
		kNoScrollbars, 
		kVerticalScrollbar       = kVerticalScrollbarMask, 
		kHorizontalScrollbar     = kHorizontalScrollbarMask, 
		kBothScrollbars          = kVerticalScrollbar | kHorizontalScrollbar
	};
	
	template < ScrollbarConfig config >  struct Scroller_Traits
	{
		typedef Scrollbar_Traits< bool( config & kVerticalScrollbarMask   ) > VerticalTraits;
		typedef Scrollbar_Traits< bool( config & kHorizontalScrollbarMask ) > HorizontalTraits;
		
		typedef typename VerticalTraits  ::Type VerticalScrollbarType;
		typedef typename HorizontalTraits::Type HorizontalScrollbarType;
	};
	
	template < ScrollbarConfig config >  struct Scroller_Traits;
	
	enum ScrollbarAxis
	{
		kVertical   = kVerticalScrollbar, 
		kHorizontal = kHorizontalScrollbar
	};
	
	
	template < class ScrollViewType, ScrollbarConfig config >
	class Scroller : public BoundedView, public Scroller_Traits< config >
	{
		private:
			typedef Scroller_Traits< config > Traits;
			
			typedef typename Traits::VerticalTraits   VerticalTraits;
			typedef typename Traits::HorizontalTraits HorizontalTraits;
			
			typedef typename Traits::VerticalScrollbarType   VerticalScrollbarType;
			typedef typename Traits::HorizontalScrollbarType HorizontalScrollbarType;
			
			VerticalScrollbarType myScrollV;
			HorizontalScrollbarType myScrollH;
			ScrollViewType myScrollView;
		
		public:
			typedef typename ScrollViewType::Initializer Initializer;
			
			Scroller( const Rect& bounds, Initializer init = Initializer() );
			
			static bool ScrollsVertically()    { return VerticalTraits::present; }
			static bool ScrollsHorizontally()  { return HorizontalTraits::present; }
			
			ScrollViewType&          ScrollView()           { return myScrollView; }
			VerticalScrollbarType&   VerticalScrollbar()    { return myScrollV; }
			HorizontalScrollbarType& HorizontalScrollbar()  { return myScrollH; }
			
			void SetControlViewSizes();
			
			void Calibrate();
			
			void Scroll(short dh, short dv, bool updateNow = 0)
			{
				Pedestal::Scroll( myScrollView, dh, dv, updateNow );
				Calibrate();
			}
			
			template < ScrollbarAxis axis >
			void ScrollAction( ControlRef control, N::ControlPartCode part );
			
			void UpdateScrollbars( Point oldRange, Point oldPosition );
			
			void Idle( const EventRecord& event )
			{
				// Intersect the clip region with the scrollview bounds,
				// so the scrollview doesn't overpaint the scroll bars.
				N::Saved< N::Clip_Value > savedClip( N::SectRgn( N::GetClip(),
				                                                 N::RectRgn( ScrollView().Bounds() ) ) );
				
				ScrollView().Idle( event );
			}
			
			void MouseDown( const EventRecord& event )
			{
				if ( N::PtInRect( N::GlobalToLocal( event.where ), ScrollView().Bounds() ) )
				{
					Point scrollableRange = ScrollableRange( ScrollView() );
					Point scrollPosition  = ScrollPosition ( ScrollView() );
					
					ScrollView().MouseDown( event );
					
					UpdateScrollbars( scrollableRange, scrollPosition );
				}
			}
			
			bool KeyDown( const EventRecord& event )
			{
				Point scrollableRange = ScrollableRange( ScrollView() );
				Point scrollPosition  = ScrollPosition ( ScrollView() );
				
				if ( ScrollView().KeyDown( event ) )
				{
					UpdateScrollbars( scrollableRange, scrollPosition );
					return true;
				}
				
				return false;
			}
			
			void Update()
			{
				// Intersect the clip region with the scrollview bounds,
				// so the scrollview doesn't overpaint the scroll bars.
				N::Saved< N::Clip_Value > savedClip( N::SectRgn( N::GetClip(),
				                                                 N::RectRgn( ScrollView().Bounds() ) ) );
				
				ScrollView().Update();
			}
			
			void Activate( bool activating )
			{
				ScrollView().Activate( activating );
				
				N::Saved< N::Clip_Value > savedClip;
				N::ClipRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
				
				
				VerticalScrollbar  ().Activate( activating );
				HorizontalScrollbar().Activate( activating );
			}
			
			bool SetCursor( Point location, RgnHandle mouseRgn )
			{
				if ( N::PtInRect( location, ScrollView().Bounds() ) )
				{
					return ScrollView().SetCursor( location, mouseRgn );
				}
				
				return false;
			}
			
			bool UserCommand( MenuItemCode code )
			{
				Point scrollableRange = ScrollableRange( ScrollView() );
				Point scrollPosition  = ScrollPosition ( ScrollView() );
				
				if ( ScrollView().UserCommand( code ) )
				{
					UpdateScrollbars( scrollableRange, scrollPosition );
					return true;
				}
				
				return false;
			}
			
			void Resize( const Rect& newBounds )
			{
				BoundedView::Resize( NX::RectWidth ( newBounds ),
				                     NX::RectHeight( newBounds ) );
				
				Rect aperture = Aperture( Bounds(),
				                          VerticalTraits  ::profile,
				                          HorizontalTraits::profile );
				
				// Invalidate old scrollbar regions
				InvalidateControl( VerticalScrollbar  ().Get() );
				InvalidateControl( HorizontalScrollbar().Get() );
				
				SetBounds( VerticalScrollbar  ().Get(),   VerticalScrollbarBounds( Bounds(), true ) );
				SetBounds( HorizontalScrollbar().Get(), HorizontalScrollbarBounds( Bounds(), true ) );
				
				// Invalidate new scrollbar regions
				InvalidateControl( VerticalScrollbar  ().Get() );
				InvalidateControl( HorizontalScrollbar().Get() );
				
				ScrollView().Resize( aperture );
				
				Calibrate();
				
				SetControlViewSizes();
			}
	};
	
	
	template < class ScrollViewType, ScrollbarConfig config, ScrollbarAxis axis >
	void ScrollbarAction( ControlRef control, N::ControlPartCode part )
	{
		typedef Scroller< ScrollViewType, config > Scroller;
		
		Control_Hooks* controlHooks = N::GetControlReference( control );
		Scroller* scroller = controlHooks ? static_cast< Scroller* >( controlHooks->data ) : NULL;
		ASSERT( scroller != NULL );
		
		scroller->template ScrollAction< axis >( control, part );
	}
	
	template < class ScrollViewType, ScrollbarConfig config, ScrollbarAxis axis >
	void Track( ControlRef control, N::ControlPartCode part, Point point )
	{
		N::Saved< N::Clip_Value > savedClip;
		N::ClipRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
		
		switch ( ::ControlPartCode( part ) )
		{
			case kControlIndicatorPart:
			{
				short oldValue = N::GetControlValue( control );
				part = N::TrackControl( control, point );
				if ( part == N::ControlIndicatorPart() )
				{
					short scrollDistance = N::GetControlValue( control ) - oldValue;
					if ( scrollDistance != 0 )
					{
						typedef Scroller< ScrollViewType, config > Scroller;
						
						Control_Hooks* controlHooks = N::GetControlReference( control );
						Scroller* scroller = controlHooks ? reinterpret_cast< Scroller* >( controlHooks->data ) : NULL;
						ASSERT( scroller != NULL );
						
						scroller->Scroll
						(
							( axis != kVertical ) ? scrollDistance : 0, 
							( axis == kVertical ) ? scrollDistance : 0, 
							false
						);
					}
				}
				break;
			}
			case kControlUpButtonPart:
			case kControlDownButtonPart:
			case kControlPageUpPart:
			case kControlPageDownPart:
				part = N::TrackControl< N::ControlActionProcPtr( ScrollbarAction< ScrollViewType, config, axis > ) >( control, point );
				break;
			default:
				break;
		}
	}
	
	
	template < class ScrollViewType, ScrollbarConfig config >
	Rect Bounds( const Scroller< ScrollViewType, config >& scroller )
	{
		return scroller.Bounds();
	}
	
	template < class ScrollViewType, ScrollbarConfig config >
	Scroller< ScrollViewType, config >::Scroller( const Rect& bounds, Initializer init )
	: 
		BoundedView( bounds ), 
		myScrollV( VerticalScrollbarBounds  ( bounds,
		                                      true ),
		           this,
		           Track< ScrollViewType,
		                  config,
		                  kVertical   > ), 
		myScrollH( HorizontalScrollbarBounds( bounds,
		                                      true ),
		           this,
		           Track< ScrollViewType,
		                  config,
		                  kHorizontal > ), 
		myScrollView( Aperture( bounds,
		                        VerticalTraits::profile,
		                        HorizontalTraits::profile ),
		              init )
	{
		SetControlViewSizes();
	}
	
	template < class ScrollViewType, ScrollbarConfig config >
	void Scroller< ScrollViewType, config >::SetControlViewSizes()
	{
		Point range = ViewableRange( myScrollView );
		
		Pedestal::SetControlViewSize( VerticalScrollbar  ().Get(), range.v );
		Pedestal::SetControlViewSize( HorizontalScrollbar().Get(), range.h );
	}
	
	template < class ScrollViewType, ScrollbarConfig config >
	void Scroller< ScrollViewType, config >::Calibrate()
	{
		Point scrollRange = ScrollableRange( myScrollView );
		Point viewRange   = ViewableRange  ( myScrollView );
		
		Point scrollPos   = ScrollPosition ( myScrollView );
		
		Point max = ScrollbarMaxima( scrollRange, viewRange, scrollPos );
		
		SetControlMaximum( myScrollV.Get(), max.v );
		SetControlMaximum( myScrollH.Get(), max.h );
	}
	
	template < class ScrollViewType, ScrollbarConfig config >
	template < ScrollbarAxis axis >
	void Scroller< ScrollViewType, config >::ScrollAction( ControlRef control, N::ControlPartCode part )
	{
		Point viewRange = ViewableRange( myScrollView );
		short jump = ( ( axis == kVertical ) ? viewRange.v : viewRange.h ) - 1;
		short scrollDistance = FigureScrollDistance( part, jump );
		
		short delta = SetControlValueFromClippedDelta( control, scrollDistance );
		
		if ( delta != 0 )
		{
			Scroll
			(
				( axis != kVertical ) ? delta : 0, 
				( axis == kVertical ) ? delta : 0, 
				true
			);
		}
	}
	
	inline bool operator==( Point a, Point b )
	{
		return a.h == b.h  &&  a.v == b.v;
	}
	
	inline bool operator!=( Point a, Point b )
	{
		return !( a == b );
	}
	
	template < class ScrollViewType, ScrollbarConfig config >
	void Scroller< ScrollViewType, config >::UpdateScrollbars( Point oldRange, Point oldPosition )
	{
		using namespace Nucleus::Operators;
		
		Point range = ScrollableRange( ScrollView() );
		Point pos = ScrollPosition( ScrollView() );
		
		if ( oldPosition != pos  ||  oldRange != range )
		{
			N::Saved< N::Clip_Value > savedClip;
			
			N::ClipRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
			
			SetValueStretch( myScrollH.Get(), pos.h );
			SetValueStretch( myScrollV.Get(), pos.v );
			
			// need to update scrollbar maxima
			Calibrate();
		}
	}
	
}

#endif

