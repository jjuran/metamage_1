/*	=========
 *	TEView.cc
 *	=========
 */

#include "Pedestal/TEView.hh"

// Universal Interfaces
#include <Fonts.h>

// Standard C++
#include <algorithm>

// Nucleus
#include "Nucleus/NAssert.h"
#include "Nucleus/Saved.h"

// Nitrogen / Carbon support
#include "Nitrogen/TextEdit.h"

// Nitrogen Extras / ClassicToolbox
#include "ClassicToolbox/MacWindows.h"
#include "ClassicToolbox/Scrap.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/Clipboard.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
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
		return BoundsFromViewRect( N::GetTEViewRect( hTE ) );
	}
	
	short CountLinesForDisplay( TEHandle hTE )
	{
		return N::GetTELineCount( hTE );
	}
	
	short CountLinesForEditing( TEHandle hTE )
	{
		short teLength = N::GetTELength( hTE );
		
		// An empty buffer counts as one line
		if ( teLength == 0 )
		{
			return 1;
		}
		
		short    nLines = N::GetTELineCount ( hTE );
		::Handle hText  = N::GetTETextHandle( hTE );
		
		// Find the last character in the buffer
		char c = ( *hText )[ teLength - 1 ];
		
		// If the last char is a carriage return, add an extra line
		if ( c == '\r' )
		{
			++nLines;
		}
		
		return nLines;
	}
	
	short ViewableLines( TEHandle hTE )
	{
		Rect viewRect = N::GetTEViewRect( hTE );
		short viewHeight = viewRect.bottom - viewRect.top;
		return viewHeight / N::GetTELineHeight( hTE );
	}
	
	Point ViewableRange( TEHandle hTE )
	{
		return N::SetPt( 1, ViewableLines( hTE ) );
	}
	
	Point ScrollableRange( TEHandle hTE )
	{
		return N::SetPt( 1, CountLinesForEditing( hTE ) );
	}
	
	Point ScrollStep( TEHandle hTE )
	{
		return N::SetPt( 1,
		                 N::GetTELineHeight( hTE ) );
	}
	
	static int VScrollOffset( TEHandle hTE )
	{
		int dv =   N::GetTEViewRect( hTE ).top
		           - N::GetTEDestRect( hTE ).top;
		
		return dv;
	}
	
	Point ScrollPosition( TEHandle hTE )
	{
		int dv = VScrollOffset( hTE );
		
		return N::SetPt( 0,
		                 dv == 0 ? 0
		                         : (dv - 1) / N::GetTELineHeight( hTE ) + 1 );
	}
	
	void Resize( TEHandle hTE, const Rect& newBounds )
	{
		int dv = VScrollOffset( hTE );
		
		Rect viewRect = ViewRectFromBounds( newBounds );
		
		N::SetTEViewRect( hTE, viewRect );
		N::SetTEDestRect( hTE, N::OffsetRect( viewRect, 0, -dv ) );
		N::TECalText( hTE );
		N::InvalRect( newBounds );
	}
	
	void Resize( TEHandle hTE, short width, short height )
	{
		Rect bounds = Bounds( hTE );
		
		bounds.right = bounds.left + width;
		bounds.bottom = bounds.top + height;
		
		Resize( hTE, bounds );
	}
	
	void Scroll( TEHandle hTE, short dh, short dv )
	{
		Point scrollStep = ScrollStep( hTE );
		N::TEPinScroll( -dh * scrollStep.h, -dv * scrollStep.v, hTE );
	}
	
	
	static TEView* gCurrentTEView = NULL;
	
	static void CustomClickLoop()
	{
		if ( gCurrentTEView == NULL )  return;
		
		TEHandle hTE = gCurrentTEView->Get();
	}
	
	// This gets set the first time we call TENew().
	static ::TEClickLoopUPP gSystemClickLoop = NULL;
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	static pascal asm void MasterTEClickLoop()
	{
		MOVEA.L		gSystemClickLoop,A0	;  // load the default clickLoop
		
		JSR			(A0)				;  // invoke it
		
		MOVEM.L		D1-D2/A1,-(SP)		;  // save registers
		JSR			CustomClickLoop		;  // invoke our custom clickLoop
		MOVEM.L		(SP)+,D1-D2/A1		;  // restore registers
		
		MOVEQ		#1,D0				;  // return true by clearing the zero flag
		RTS
	}
	
#else
	
	static pascal Boolean MasterTEClickLoop( TEPtr pTE )
	{
		(void) ::InvokeTEClickLoopUPP( pTE, gSystemClickLoop );
		
		CustomClickLoop();
		
		return true;
	}
	
#endif
	
	static ::TEClickLoopUPP gMasterClickLoop = ::NewTEClickLoopUPP( MasterTEClickLoop );
	
	
	static int SetTextAttributes()
	{
		::TextFont( kFontIDMonaco );
		::TextSize( 9 );
		
		return 0;
	}
	
	
	TEView::TEView( const Rect&  bounds,
	                Initializer  /**/ ) : itsTE( ( SetTextAttributes(),
	                                               N::TENew( ViewRectFromBounds( bounds ) ) ) )
	{
		N::TEAutoView( true, itsTE );  // enable auto-scrolling
		
		static ::TEClickLoopUPP clickLoop = gSystemClickLoop = itsTE.Get()[0]->clickLoop;
		
		itsTE.Get()[0]->clickLoop = gMasterClickLoop;
	}
	
	void TEView::Idle( const EventRecord& )
	{
		N::TEIdle( itsTE );
		
		AdjustSleepForTimer( ::GetCaretTime() );
	}
	
	void TEView::MouseDown( const EventRecord& event )
	{
		gCurrentTEView = this;
		
		N::TEClick( N::GlobalToLocal( event.where ),
		            event.modifiers & shiftKey,
		            itsTE );
		
		gCurrentTEView = NULL;
	}
	
	bool TEView::KeyDown( const EventRecord& event )
	{
		N::TEKey( event.message & charCodeMask, itsTE );
		return true;
	}
	
	void TEView::Activate( bool activating )
	{
		if ( activating )
		{
			N::TEActivate( itsTE );
		}
		else
		{
			N::TEDeactivate( itsTE );
		}
	}
	
	void TEView::Update()
	{
		Rect bounds = Pedestal::Bounds( itsTE.Get() );
		
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
		
		int textHeight = CountLinesForDisplay( itsTE ) * N::GetTELineHeight( itsTE );
		
		Rect viewRect = N::GetTEViewRect( itsTE );
		short viewHeight = viewRect.bottom - viewRect.top;
		
		Rect destRect = N::GetTEDestRect( itsTE );
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
	
	bool TEView::UserCommand( MenuItemCode code )
	{
		switch ( code )
		{
			// Edit
			case 'undo':
				//Undo();
				break;
			
			case 'cut ':
				Cut();
				break;
			
			case 'copy':
				Copy();
				break;
			
			case 'pste':
				Paste();
				break;
			
			case 'slct':
				SelectAll();
				break;
			
			default:
				return false;
				break;
		}
		
		return true;
	}
	
	bool TEView::SetCursor( Point location, RgnHandle /*mouseRgn*/ )
	{
		if ( N::PtInRect( location, Bounds() ) )
		{
			N::SetCursor( N::GetCursor( N::iBeamCursor ) );
			return true;
		}
		else
		{
			return false;
		}
	}
	
	void TEView::SetSelection( short start, short end )
	{
		N::TESetSelect( start, end, itsTE );
	}
	
	int TEView::AppendChars( const char* data, unsigned int byteCount, bool updateNow )
	{
		if ( byteCount == 0 )  return 0;
		
		ASSERT( data != NULL );
		
		Handle hText = N::GetTETextHandle( itsTE );
		short len = N::GetTELength( itsTE );
		
		byteCount = std::min< unsigned int >( byteCount, 32000 - len );
		
		N::SetHandleSize( hText, len + byteCount );
		std::copy( data, data + byteCount, *hText + len );
		N::SetTELength( itsTE, len + byteCount );
		N::TECalText( itsTE );
		N::TESetSelect( 32767, 32767, itsTE );
		
		if ( updateNow )
		{
			N::GrafPtr port = N::GetTEPort( itsTE );
			
			// This is correct but MWPro6 breaks on it
			//NN::Saved< N::Port_Value > savedPort( port );
			
			NN::Saved< N::Port_Value > savedPort;
			
			N::SetPort( port );
			
			Update();
		}
		
		return byteCount;
	}
	
	void TEView::Cut()
	{
		Clipboard::TECut( itsTE );
	}
	
	void TEView::Copy()
	{
		Clipboard::TECopy( itsTE );
	}
	
	void TEView::Paste()
	{
		Clipboard::TEPaste( itsTE );
	}
	
	void TEView::Clear()
	{
		N::TEDelete( itsTE );
	}
	
	void TEView::SelectAll()
	{
		N::TESetSelect( 0, 32767, itsTE );
	}
	
}

