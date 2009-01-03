/*	=========
 *	TEView.cc
 *	=========
 */

#include "Pedestal/TEView.hh"

// Universal Interfaces
#ifndef __FONTS__
#include <Fonts.h>
#endif

// Standard C++
#include <algorithm>

// Nucleus
#include "Nucleus/NAssert.h"
#include "Nucleus/Saved.h"

// Pedestal
#include "Pedestal/Scroller.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static Rect ViewRectFromBounds( const Rect& bounds )
	{
		return N::InsetRect( bounds, 4, 4 );
	}
	
	static Rect BoundsFromViewRect( const Rect& viewRect )
	{
		return N::InsetRect( viewRect, -4, -4 );
	}
	
	Rect Bounds( TEHandle hTE )
	{
		return BoundsFromViewRect( hTE[0]->viewRect );
	}
	
	short CountLinesForDisplay( TEHandle hTE )
	{
		return hTE[0]->nLines;
	}
	
	short CountLinesForEditing( TEHandle hTE )
	{
		short teLength = hTE[0]->teLength;
		
		// An empty buffer counts as one line
		if ( teLength == 0 )
		{
			return 1;
		}
		
		short    nLines = hTE[0]->nLines;
		::Handle hText  = hTE[0]->hText;
		
		// Find the last character in the buffer
		char c = ( *hText )[ teLength - 1 ];
		
		// If the last char is a carriage return, add an extra line
		if ( c == '\r' )
		{
			++nLines;
		}
		
		return nLines;
	}
	
	Point TEView::ViewableRange() const
	{
		Rect viewRect = itsTE[0]->viewRect;
		
		short viewWidth  = viewRect.right - viewRect.left;
		short viewHeight = viewRect.bottom - viewRect.top;
		
		return N::SetPt( viewWidth  / ::CharWidth( 'M' ),
		                 viewHeight / itsTE[0]->lineHeight );
	}
	
	Point TEView::ScrollableRange() const
	{
		return N::SetPt( 1, CountLinesForEditing( itsTE ) );
	}
	
	Point ScrollStep( TEHandle hTE )
	{
		return N::SetPt( 1,
		                 hTE[0]->lineHeight );
	}
	
	static int VScrollOffset( TEHandle hTE )
	{
		int dv =   hTE[0]->viewRect.top
		         - hTE[0]->destRect.top;
		
		return dv;
	}
	
	Point TEView::ScrollPosition() const
	{
		int dv = VScrollOffset( itsTE );
		
		return N::SetPt( 0,
		                 dv == 0 ? 0
		                         : (dv - 1) / itsTE[0]->lineHeight + 1 );
	}
	
	Point TEView::ScrollUnits() const
	{
		return N::SetPt( ::CharWidth( 'M' ), itsTE[0]->lineHeight );
	}
	
	static void Resize( TEHandle hTE, const Rect& newBounds )
	{
		int dv = VScrollOffset( hTE );
		
		Rect viewRect = ViewRectFromBounds( newBounds );
		Rect destRect = N::OffsetRect( viewRect, 0, -dv );
		
		TERec& te = **hTE;
		
		te.viewRect = viewRect;
		te.destRect = destRect;
		
		N::TECalText( hTE );
	}
	
	void Resize( TEHandle hTE, short width, short height )
	{
		Rect bounds = Bounds( hTE );
		
		bounds.right = bounds.left + width;
		bounds.bottom = bounds.top + height;
		
		Resize( hTE, bounds );
	}
	
	void TEView::Scroll( int dh, int dv )
	{
		Point scrollStep = ScrollStep( itsTE );
		
		N::TEPinScroll( -dh * scrollStep.h, -dv * scrollStep.v, itsTE );
	}
	
	
	static int SetTextAttributes()
	{
		::TextFont( kFontIDMonaco );
		::TextSize( 9 );
		
		return 0;
	}
	
	
	TEView::TEView( const Rect&  bounds ) : itsTE( ( SetTextAttributes(),
	                                                 N::TENew( ViewRectFromBounds( bounds ) ) ) )
	{
		N::TEAutoView( true, itsTE );  // enable auto-scrolling
		
		InstallCustomTEClickLoop( itsTE );
	}
	
	
	static const RGBColor gRGBBlack = {     0,     0,     0 };
	static const RGBColor gRGBWhite = { 65535, 65535, 65535 };
	
	static void DrawQuasimodeFrame( Rect frame )
	{
		N::FrameRect( frame );
	}
	
	void TEView::BeginQuasimode()
	{
		DrawQuasimodeFrame( Bounds() );
	}
	
	void TEView::EndQuasimode()
	{
		N::RGBForeColor( gRGBWhite );
		
		DrawQuasimodeFrame( Bounds() );
		
		N::RGBForeColor( gRGBBlack );
	}
	
	
	boost::shared_ptr< Quasimode > TEView::EnterShiftSpaceQuasimode( const EventRecord& event )
	{
		const bool backward = event.modifiers & shiftKey;
		
		boost::shared_ptr< Quasimode > mode( new IncrementalSearchQuasimode( *this, backward ) );
		
		return mode;
	}
	
	void TEView::Draw( const Rect& bounds )
	{
		Rect top = bounds;
		top.bottom = top.top + 4;
		
		Rect left = bounds;
		left.right = left.left + 4;
		
		Rect bottom = bounds;
		bottom.top = bottom.bottom - 4;
		
		Rect right = bounds;
		right.left = right.right - 4;
		
		N::EraseRect( top    );
		N::EraseRect( left   );
		N::EraseRect( bottom );
		N::EraseRect( right  );
		
		N::TEUpdate( bounds, itsTE );
		
		int textHeight = CountLinesForDisplay( itsTE ) * itsTE[0]->lineHeight;
		
		Rect viewRect = itsTE[0]->viewRect;
		short viewHeight = viewRect.bottom - viewRect.top;
		
		Rect destRect = itsTE[0]->destRect;
		destRect.bottom = destRect.top + textHeight;
		
		// If the bottom of the text doesn't reach the bottom of the viewing area,
		if ( destRect.bottom < viewRect.bottom )
		{
			// then below the text is a blank space, which TEUpdate() ignores.
			// So we need to erase it ourselves.
			viewRect.top = destRect.bottom;
			N::EraseRect( viewRect );
		}
	}
	
	int TEView::AppendChars( const char* data, unsigned int byteCount, bool updateNow )
	{
		if ( byteCount == 0 )  return 0;
		
		ASSERT( data != NULL );
		
		Handle hText = itsTE[0]->hText;
		short len    = itsTE[0]->teLength;
		
		byteCount = std::min< unsigned int >( byteCount, 32000 - len );
		
		N::SetHandleSize( hText, len + byteCount );
		
		std::replace_copy( data,
		                   data + byteCount,
		                   *hText + len,
		                   '\n',
		                   '\r' );
		
		itsTE[0]->teLength = len + byteCount;
		
		N::TECalText( itsTE );
		
		Select( 32767, 32767 );
		
		if ( updateNow )
		{
			N::GrafPtr port = itsTE[0]->inPort;
			
			// This is correct but MWPro6 breaks on it
			//NN::Saved< N::Port_Value > savedPort( port );
			
			NN::Saved< N::Port_Value > savedPort;
			
			N::SetPort( port );
			
			Rect bounds = Pedestal::Bounds( itsTE.Get() );
			
			Draw( bounds );
		}
		
		return byteCount;
	}
	
}

