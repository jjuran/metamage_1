/*	===========
 *	Scroller.hh
 *	===========
 */

#ifndef PEDESTAL_SCROLLER_HH
#define PEDESTAL_SCROLLER_HH

// Mac OS
#include <ControlDefinitions.h>

// Nucleus
#include "Nucleus/NAssert.h"
#include "Nucleus/Saved.h"

// Nitrogen
#include "Nitrogen/Controls.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"

// Pedestal
#include "Pedestal/Control.hh"
#include "Pedestal/TEView.hh"
#include "Pedestal/UserWindow.hh"


namespace Pedestal
{
	
	class ScrollbarView : public View
	{
		private:
			ControlRef     itsControl;
			Control_Hooks  itsControlHooks;
		
		public:
			ScrollbarView( const Rect&              bounds,
			               Nitrogen::ControlProcID  procID,
			               Nitrogen::RefCon         refCon,
			               ControlTracker           tracker );
			
			ControlRef Get() const  { return itsControl; }
			
			void Activate( bool activating );
	};
	
	
	template < bool present >  struct Scrollbar_Traits;
	
	// present:  Is the scrollbar present?
	// profile:  The amount the scrollbar encroaches on the scroll view.
	// Type:     The storage type of the scrollbar object.
	
	template <>  struct Scrollbar_Traits< true >
	{
		static const bool  present = true;
		static const short profile = 15;
		
		typedef ScrollbarView Type;
	};
	
	template <>  struct Scrollbar_Traits< false >
	{
		static const bool  present = false;
		static const short profile = 0;
		
		struct Type
		{
			Type( const Rect&, Nitrogen::ControlProcID, Nitrogen::RefCon, ControlTracker )
			{
			}
			
			Type& Get()  { return *this; }
			
			void Activate( bool )  {}
		};
	};
	
	short ActualScrollbarLength( short viewLength, bool shortened );
	
	Rect VerticalScrollbarBounds  ( short width, short height, bool shortened );
	Rect HorizontalScrollbarBounds( short width, short height, bool shortened );
	
	Point ScrollDimensions( short width, short height, bool vertical, bool horizontal );
	
	inline Rect ScrollBounds( short width, short height, bool vertical, bool horizontal )
	{
		Point dimensions = ScrollDimensions( width, height, vertical, horizontal );
		
		return Nitrogen::SetRect( 0, 0, dimensions.h, dimensions.v );
	}
	
	template < bool vertical, bool horizontal >
	inline Point ScrollDimensions( const Rect& scroller_bounds )
	{
		return ScrollDimensions( NitrogenExtras::RectWidth ( scroller_bounds ),
		                         NitrogenExtras::RectHeight( scroller_bounds ),
		                         Scrollbar_Traits< vertical   >::profile,
		                         Scrollbar_Traits< horizontal >::profile );
	}
	
	template < bool vertical, bool horizontal >
	inline Rect ScrollBounds( const Rect& scroller_bounds )
	{
		return ScrollBounds( NitrogenExtras::RectWidth ( scroller_bounds ),
		                     NitrogenExtras::RectHeight( scroller_bounds ),
		                     Scrollbar_Traits< vertical   >::profile,
		                     Scrollbar_Traits< horizontal >::profile );
	}
	
	Point ScrollbarMaxima( Point scrollableRange, Point viewableRange, Point scrollPosition );
	
	short FigureScrollDistance( Nitrogen::ControlPartCode part, short pageDistance );
	
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
	
	enum ScrollbarAxis
	{
		kVertical,
		kHorizontal
	};
	
#if TARGET_API_MAC_CARBON
	
	inline bool AppearanceManagerExists()  { return true; }
	
#else
	
	bool AppearanceManagerExists();
	
#endif
	
	template < class Existence >
	inline Nitrogen::ControlProcID GetControlProcIDForAppearenceExistence( Existence exists )
	{
		return exists ? Nitrogen::kControlScrollBarLiveProc
		              : Nitrogen::scrollBarProc;
	}
	
	
	using Nitrogen::SetControlMaximum;
	
	inline void SetControlViewSize( ControlRef control, long size )
	{
	#if !TARGET_CPU_68K
		
		if ( TARGET_API_MAC_CARBON || ::SetControlViewSize != (void*)kUnresolvedCFragSymbolAddress )
		{
			::SetControlViewSize( control, size );
		}
		
	#endif
	}
	
	inline void InvalidateControl( ControlRef control )  { Nitrogen::InvalRect( Nitrogen::GetControlBounds( control ) ); }
	
	inline void SetBounds         ( Scrollbar_Traits< false >::Type, Rect  )  {}
	inline void SetControlMaximum ( Scrollbar_Traits< false >::Type, short )  {}
	inline void SetValueStretch   ( Scrollbar_Traits< false >::Type, short )  {}
	inline void SetControlViewSize( Scrollbar_Traits< false >::Type, long  )  {}
	inline void InvalidateControl ( Scrollbar_Traits< false >::Type        )  {}
	
	
	template < ScrollbarAxis axis >
	inline short VHSelect( Point point )
	{
		return ( axis == kVertical ) ? point.v : point.h;
	}
	
	Point ComputeScrollbarMaxima( const TEView& scrolledView );
	
	template < class Vertical, class Horizontal >
	inline void SetScrollbarMaxima( const Vertical&    verticalScrollbar,
	                                const Horizontal&  horizontalScrollbar,
	                                Point              maxima )
	{
		SetControlMaximum( verticalScrollbar,   maxima.v );
		SetControlMaximum( horizontalScrollbar, maxima.h );
	}
	
	
	class ClickableScroller
	{
		protected:
			static ClickableScroller* gCurrentScroller;
		
		public:
			//static ClickableScroller& Current();
			
			static void ClickLoop()  { if ( gCurrentScroller )  gCurrentScroller->ClickInLoop(); }
			
			virtual void ClickInLoop() = 0;
	};
	
	class ScrollerBase : public BoundedView, public ClickableScroller
	{
		private:
			UserView itsScrollableView;
		
		public:
			ScrollerBase( const Rect& bounds ) : BoundedView( bounds )
			{
			}
			
			void SetSubView( std::auto_ptr< View > subview )  { itsScrollableView.Set( subview ); }
			
			TEView const& GetSubView() const  { return itsScrollableView.Get< TEView >(); }
			TEView      & GetSubView()        { return itsScrollableView.Get< TEView >(); }
			
			void Idle( const EventRecord& event );
			
			void MouseDown( const EventRecord& event );
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event )
			{
				return GetSubView().EnterShiftSpaceQuasimode( event );
			}
			
			void Draw( const Rect& bounds );
			
			bool SetCursor( const EventRecord& event, RgnHandle mouseRgn );
			
	};
	
	
	inline TEView& RecoverScrolledViewFromScrollbar( ControlRef control )
	{
		Control_Hooks* controlHooks = Nitrogen::GetControlReference( control );
		
		ASSERT( controlHooks       != NULL );
		ASSERT( controlHooks->data != NULL );
		
		ScrollerBase& scroller = *static_cast< ScrollerBase* >( controlHooks->data );
		
		return scroller.GetSubView();
	}
	
	
	template < ScrollbarAxis axis >
	inline void ScrollByDelta( TEView& scrolledView, ControlRef control, short delta, bool updateNow )
	{
		if ( delta != 0 )
		{
			scrolledView.Scroll( ( axis != kVertical ) ? delta : 0,
			                     ( axis == kVertical ) ? delta : 0,
			                     updateNow );
			
			SetControlMaximum( control, VHSelect< axis >( ComputeScrollbarMaxima( scrolledView ) ) );
		}
	}
	
	template < ScrollbarAxis axis >
	inline void ScrollByDelta( ControlRef control, short delta, bool updateNow )
	{
		TEView& scrolledView = RecoverScrolledViewFromScrollbar( control );
		
		ScrollByDelta< axis >( scrolledView, control, delta, updateNow );
	}
	
	template < ScrollbarAxis axis >
	void ScrollbarAction( ControlRef control, Nitrogen::ControlPartCode part )
	{
		TEView& scrolledView = RecoverScrolledViewFromScrollbar( control );
		
		short jump = VHSelect< axis >( scrolledView.ViewableRange() ) - 1;
		short scrollDistance = FigureScrollDistance( part, jump );
		
		short delta = SetControlValueFromClippedDelta( control, scrollDistance );
		
		if ( part == Nitrogen::kControlIndicatorPart )
		{
			short oldValue = VHSelect< axis >( scrolledView.ScrollPosition() );
			
			delta = Nitrogen::GetControlValue( control ) - oldValue;
		}
		
		ScrollByDelta< axis >( scrolledView, control, delta, true );
	}
	
	template < ScrollbarAxis axis >
	inline Nitrogen::ControlPartCode TrackScrollBar( ControlRef control, Point point )
	{
		return Nitrogen::TrackControl<
		                               #ifdef __MWERKS__
		                               (Nitrogen::ControlActionProcPtr)
		                               #endif
		                               ScrollbarAction< axis > >( control, point );
	}
	
	template < ScrollbarAxis axis >
	void Track( ControlRef control, Nitrogen::ControlPartCode part, Point point )
	{
		Nucleus::Saved< Nitrogen::Clip_Value > savedClip;
		Nitrogen::ClipRect( Nitrogen::GetPortBounds( Nitrogen::GetQDGlobalsThePort() ) );
		
		switch ( part )
		{
			case kControlIndicatorPart:
				// The user clicked on the indicator
				
				if ( !AppearanceManagerExists() )
				{
					// Classic scrolling, handled specially.
					
					// Get the current scrollbar value.
					short oldValue = Nitrogen::GetControlValue( control );
					
					// Let the system track the drag...
					part = Nitrogen::TrackControl( control, point );
					
					if ( part == Nitrogen::kControlIndicatorPart )
					{
						// Drag was successful (i.e. within bounds).  Subtract to get distance.
						short scrollDistance = Nitrogen::GetControlValue( control ) - oldValue;
						
						// Scroll by that amount, but don't update just yet.
						ScrollByDelta< axis >( control, scrollDistance, false );
					}
					
					// Break here for classic thumb-scrolling (whether sucessful or not).
					break;
				}
				// else fall through for live feedback scrolling
			case kControlUpButtonPart:
			case kControlDownButtonPart:
			case kControlPageUpPart:
			case kControlPageDownPart:
				part = TrackScrollBar< axis >( control, point );
				break;
			
			default:
				break;
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
	
	template < class Vertical, class Horizontal >
	void UpdateScrollbars( const Vertical&        verticalScrollbar,
	                       const Horizontal&      horizontalScrollbar,
	                       Point                  maxima,
	                       Point                  position )
	{
		Nucleus::Saved< Nitrogen::Clip_Value > savedClip;
		
		Nitrogen::ClipRect( Nitrogen::GetPortBounds( Nitrogen::GetQDGlobalsThePort() ) );
		
		SetValueStretch( horizontalScrollbar, position.h );
		SetValueStretch( verticalScrollbar,   position.v );
		
		// need to update scrollbar maxima
		SetScrollbarMaxima( verticalScrollbar,
		                    horizontalScrollbar,
		                    maxima );
	}
	
	template < bool vertical, bool horizontal = false >
	class Scroller : public ScrollerBase
	{
		private:
			typedef Scrollbar_Traits< vertical   > VerticalTraits;
			typedef Scrollbar_Traits< horizontal > HorizontalTraits;
			
			typedef typename VerticalTraits  ::Type VerticalScrollbarType;
			typedef typename HorizontalTraits::Type HorizontalScrollbarType;
			
			VerticalScrollbarType    myScrollV;
			HorizontalScrollbarType  myScrollH;
		
		public:
			Scroller( const Rect& bounds );
			
			static bool ScrollsVertically()    { return VerticalTraits  ::present; }
			static bool ScrollsHorizontally()  { return HorizontalTraits::present; }
			
			VerticalScrollbarType&   VerticalScrollbar()    { return myScrollV; }
			HorizontalScrollbarType& HorizontalScrollbar()  { return myScrollH; }
			
			void SetSubView( std::auto_ptr< View > subview )
			{
				ScrollerBase::SetSubView( subview );
				
				//Point dimensions = ScrollDimensions< true, false >( Bounds() ) );
				
				//GetSubView().Resize( dimensions.h, dimensions.v );
				
				SetControlViewSizes();
			}
			
			void SetControlViewSizes();
			
			void Calibrate()
			{
				SetScrollbarMaxima( myScrollV.Get(),
				                    myScrollH.Get(),
				                    ComputeScrollbarMaxima( GetSubView() ) );
			}
			
			void Scroll(short dh, short dv, bool updateNow = 0)
			{
				GetSubView().Scroll( dh, dv, updateNow );
				
				Calibrate();
			}
			
			void UpdateScrollbars( Point oldRange, Point oldPosition );
			
			void ClickInLoop()
			{
				Pedestal::UpdateScrollbars( myScrollV.Get(),
				                            myScrollH.Get(),
				                            ComputeScrollbarMaxima( GetSubView() ),
				                            GetSubView().ScrollPosition() );
				
				return;
			}
			
			bool KeyDown( const EventRecord& event );
			
			void Activate( bool activating );
			
			void Resize( short width, short height );
			
			bool UserCommand( MenuItemCode code );
	};
	
	
	template < bool vertical, bool horizontal >
	Scroller< vertical, horizontal >::Scroller( const Rect& bounds )
	: 
		ScrollerBase( bounds ),
		myScrollV( VerticalScrollbarBounds  ( NitrogenExtras::RectWidth ( bounds ),
		                                      NitrogenExtras::RectHeight( bounds ),
		                                      true ),
		           GetControlProcIDForAppearenceExistence( AppearanceManagerExists() ),
		           static_cast< ScrollerBase* >( this ),
		           Track< kVertical > ),
		myScrollH( HorizontalScrollbarBounds( NitrogenExtras::RectWidth ( bounds ),
		                                      NitrogenExtras::RectHeight( bounds ),
		                                      true ),
		           GetControlProcIDForAppearenceExistence( AppearanceManagerExists() ),
		           static_cast< ScrollerBase* >( this ),
		           Track< kHorizontal > )
	{
	}
	
	template < bool vertical, bool horizontal >
	void Scroller< vertical, horizontal >::SetControlViewSizes()
	{
		Point range = GetSubView().ViewableRange();
		
		Pedestal::SetControlViewSize( VerticalScrollbar  ().Get(), range.v );
		Pedestal::SetControlViewSize( HorizontalScrollbar().Get(), range.h );
	}
	
	template < bool vertical, bool horizontal >
	void Scroller< vertical, horizontal >::UpdateScrollbars( Point oldRange, Point oldPosition )
	{
		using namespace Nucleus::Operators;
		
		Point range = GetSubView().ScrollableRange();
		Point pos   = GetSubView().ScrollPosition();
		
		if ( oldPosition != pos  ||  oldRange != range )
		{
			Pedestal::UpdateScrollbars( myScrollV.Get(),
			                            myScrollH.Get(),
			                            ComputeScrollbarMaxima( GetSubView() ),
			                            pos );
		}
	}
	
	template < bool vertical, bool horizontal >
	bool Scroller< vertical, horizontal >::KeyDown( const EventRecord& event )
	{
		Point scrollableRange = GetSubView().ScrollableRange();
		Point scrollPosition  = GetSubView().ScrollPosition();
		
		char keyCode = (event.message & keyCodeMask) >> 8;
		
		// Only consider programmers' keys, not control characters
		if ( keyCode >= 0x70 )
		{
			char keyChar = event.message & charCodeMask;
			
			const char pageXCharToPartDiff = kControlPageUpPart - kPageUpCharCode;
			
			ASSERT( kControlPageDownPart - kPageDownCharCode == pageXCharToPartDiff );
			
			switch ( keyChar )
			{
				case kHomeCharCode:
					Scroll( 0, -scrollPosition.v );
					UpdateScrollbars( scrollableRange, scrollPosition );
					return true;
				
				case kEndCharCode:
					Scroll( 0, scrollableRange.v - scrollPosition.v );
					UpdateScrollbars( scrollableRange, scrollPosition );
					return true;
				
				case kPageUpCharCode:
				case kPageDownCharCode:
					if ( ControlRef v = myScrollV.Get() )
					{
						using Nitrogen::ControlPartCode;
						
						ControlPartCode part = ControlPartCode( keyChar + pageXCharToPartDiff );
						
						ScrollbarAction< kVertical >( v, part );
					}
					
					return true;
				
				default:
					break;
			}
		}
		
		if ( GetSubView().KeyDown( event ) )
		{
			UpdateScrollbars( scrollableRange, scrollPosition );
			return true;
		}
		
		return false;
	}
	
	template < bool vertical, bool horizontal >
	void Scroller< vertical, horizontal >::Activate( bool activating )
	{
		GetSubView().Activate( activating );
		
		Nucleus::Saved< Nitrogen::Clip_Value > savedClip;
		
		Nitrogen::ClipRect( Nitrogen::GetPortBounds( Nitrogen::GetQDGlobalsThePort() ) );
		
		
		VerticalScrollbar  ().Activate( activating );
		HorizontalScrollbar().Activate( activating );
	}
	
	template < bool vertical, bool horizontal >
	void Scroller< vertical, horizontal >::Resize( short width, short height )
	{
		BoundedView::Resize( width, height );
		
		Point dimensions = ScrollDimensions( width,
		                                     height,
		                                     VerticalTraits  ::profile,
		                                     HorizontalTraits::profile );
		
		// Invalidate old scrollbar regions
		InvalidateControl( VerticalScrollbar  ().Get() );
		InvalidateControl( HorizontalScrollbar().Get() );
		
		SetBounds( VerticalScrollbar  ().Get(),   VerticalScrollbarBounds( width, height, true ) );
		SetBounds( HorizontalScrollbar().Get(), HorizontalScrollbarBounds( width, height, true ) );
		
		// Invalidate new scrollbar regions
		InvalidateControl( VerticalScrollbar  ().Get() );
		InvalidateControl( HorizontalScrollbar().Get() );
		
		GetSubView().Resize( dimensions.h, dimensions.v );
		
		Calibrate();
		
		SetControlViewSizes();
	}
	
	template < bool vertical, bool horizontal >
	bool Scroller< vertical, horizontal >::UserCommand( MenuItemCode code )
	{
		Point scrollableRange = GetSubView().ScrollableRange();
		Point scrollPosition  = GetSubView().ScrollPosition();
		
		if ( GetSubView().UserCommand( code ) )
		{
			UpdateScrollbars( scrollableRange, scrollPosition );
			
			return true;
		}
		
		return false;
	}
	
}

#endif

