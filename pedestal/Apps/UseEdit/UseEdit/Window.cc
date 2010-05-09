/*	=========
 *	Window.cc
 *	=========
 */

#include "UseEdit/Window.hh"

// Pedestal
#include "Pedestal/Frame.hh"
#include "Pedestal/Scrollbar.hh"
#include "Pedestal/Scroller_beta.hh"
#include "Pedestal/TextEdit.hh"
#include "Pedestal/TrackScrollbar.hh"
#include "Pedestal/UserWindow.hh"


namespace UseEdit
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	using Pedestal::Scrollbar;
	
	
	static short CountLinesForEditing( const TERec& te )
	{
		short teLength = te.teLength;
		
		// An empty buffer counts as one line
		if ( teLength == 0 )
		{
			return 1;
		}
		
		short    nLines = te.nLines;
		::Handle hText  = te.hText;
		
		// Find the last character in the buffer
		char c = ( *hText )[ teLength - 1 ];
		
		// If the last char is a carriage return, add an extra line
		if ( c == '\r' )
		{
			++nLines;
		}
		
		return nLines;
	}
	
	static short GetTextEditingHeight( TEHandle hTE )
	{
		const TERec& te = **hTE;
		
		return CountLinesForEditing( te ) * te.lineHeight;
	}
	
	
	class TextEdit : public Ped::TextEdit, public Ped::ScrollerAPI
	{
		private:
			n::owned< N::TEHandle >  itsTE;
			
			Ped::TextSelection  itsSelectionPriorToSearch;
		
		public:
			TextEdit( const Rect& bounds )
			{
				itsSelectionPriorToSearch.start = -1;
				
				Install( bounds );
			}
			
			void Install( const Rect& bounds );
			void Uninstall();
			
			TEHandle Get() const  { return itsTE; }
			
			void BeginQuasimode();
			void EndQuasimode();
			
			Ped::TextSelection GetPriorSelection() const;
			
			void SetPriorSelection( const Ped::TextSelection& selection );
			
			bool KeyDown( const EventRecord& event );
			
			short ViewWidth () const;
			short ViewHeight() const;
			
			int ClientWidth () const  { return ViewWidth();  }
			int ClientHeight() const;
			
			int GetHOffset() const  { return 0; }
			int GetVOffset() const;
			
			void SetHOffset( int h )  {}
			void SetVOffset( int v );
	};
	
	void TextEdit::Install( const Rect& bounds )
	{
		ASSERT( itsTE == NULL );
		
		N::CGrafPtr thePort = N::GetQDGlobalsThePort();
		
		::TextFont( kFontIDMonaco );
		
		::TextSize( 9 );
		
		itsTE = N::TENew( bounds );
		
		N::TEAutoView( true, itsTE );  // enable auto-scrolling
		
		InstallCustomTEClickLoop( itsTE );
		
		if ( N::GetWindowPort( N::FrontWindow() ) == thePort )
		{
			N::TEActivate( itsTE );
		}
	}
	
	void TextEdit::Uninstall()
	{
		itsTE.reset();
	}
	
	
	static void DrawQuasimodeFrame( Rect frame )
	{
		N::FrameRect( frame );
	}
	
	static void DrawQuasimodeFrame()
	{
		DrawQuasimodeFrame( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	static const RGBColor gRGBBlack = {     0,     0,     0 };
	static const RGBColor gRGBWhite = { 65535, 65535, 65535 };
	
	void TextEdit::BeginQuasimode()
	{
		DrawQuasimodeFrame();
	}
	
	void TextEdit::EndQuasimode()
	{
		N::RGBForeColor( gRGBWhite );
		
		DrawQuasimodeFrame();
		
		N::RGBForeColor( gRGBBlack );
	}
	
	Ped::TextSelection TextEdit::GetPriorSelection() const
	{
		return itsSelectionPriorToSearch;
	}
	
	void TextEdit::SetPriorSelection( const Ped::TextSelection& selection )
	{
		itsSelectionPriorToSearch = selection;
	}
	
	
	typedef TextEdit Scroller;
	
	
	bool TextEdit::KeyDown( const EventRecord& event )
	{
		return Ped::Scroller_KeyDown( *this, event )  ||  Ped::TextEdit::KeyDown( event );
	}
	
	short TextEdit::ViewWidth() const
	{
		const Rect& viewRect = itsTE[0]->viewRect;
		
		return viewRect.right - viewRect.left;
	}
	
	short TextEdit::ViewHeight() const
	{
		const Rect& viewRect = itsTE[0]->viewRect;
		
		return viewRect.bottom - viewRect.top;
	}
	
	int TextEdit::ClientHeight() const
	{
		return GetTextEditingHeight( itsTE );
	}
	
	int TextEdit::GetVOffset() const
	{
		const TERec& te = **itsTE;
		
		return te.viewRect.top - te.destRect.top;
	}
	
	void TextEdit::SetVOffset( int v )
	{
		{
			const TERec& te = **itsTE;
			
			short dv = v - (te.viewRect.top - te.destRect.top);
			
			N::TEPinScroll( 0, -dv, itsTE );
		}
		
		TERec& te = **itsTE;
		
		te.destRect = N::OffsetRect( te.viewRect, 0, -v );
	}
	
	class Frame : public Ped::Frame
	{
		private:
			Scroller  itsScroller;
		
		public:
			Frame( const Rect& bounds ) : itsScroller( Ped::Frame::ApertureFromBounds( bounds ) )
			{
				
			}
			
			short Padding() const  { return 4; }
			
			short OutlineWidth    () const  { return 0; }
			short OutlineOffset   () const  { return 0; }
			short OutlineCurvature() const  { return 0; }
			
			Rect Margin( const Rect& bounds ) const  { return N::SetRect( 0, 0, 0, 0 ); }
			
			Ped::View& Subview()  { return itsScroller; }
	};
	
	
	static Rect MakeVBounds( const Rect& bounds )
	{
		Rect v_bounds = bounds;
		
		v_bounds.top -= 1;
		
		v_bounds.right += 1;
		
		v_bounds.bottom += 1 - 15;
		
		v_bounds.left = v_bounds.right - 16;
		
		return v_bounds;
	}
	
	struct Scrollbar_UserData : Ped::Scrollbar_UserData
	{
		Scroller*  key;
		
		Scrollbar_UserData() : key()
		{
		}

	};
	
	static Ped::ScrollerAPI* RecoverScrollerFromControl( ControlRef control )
	{
		Scrollbar_UserData* userData = N::GetControlReference( control );
		
		ASSERT( userData      != NULL );
		ASSERT( userData->key != NULL );
		
		return userData->key;
	}
	
	static void DebriefAfterTrack( ControlRef control )
	{
		Scrollbar_UserData* userData = N::GetControlReference( control );
		
		ASSERT( userData      != NULL );
		ASSERT( userData->key != NULL );
		
		Ped::ScrollerAPI* proxy = userData->key;
		
		int clientLength = proxy->ClientHeight();
		int viewLength   = proxy->ViewHeight  ();
		int offset       = proxy->GetVOffset  ();
		
		short max_offset = std::max( clientLength - viewLength, offset );
		
		N::SetControlMaximum( control, max_offset );
	}
	
	class ScrollFrame : public Ped::ScrollFrame
	{
		private:
			Rect  itsBounds;
			
			Frame  itsFrame;
			
			Scrollbar  itsVertical;
			Scrollbar  itsHorizontal;
			
			Scrollbar_UserData  itsUserData;
		
		public:
			ScrollFrame()
			:
				itsBounds( N::GetPortBounds( N::GetQDGlobalsThePort() ) ),
				itsFrame( itsBounds )
			{
				itsUserData.trackHook = &Ped::TrackScrollbar;
				itsUserData.fetchHook = &RecoverScrollerFromControl;
				itsUserData.afterHook = &DebriefAfterTrack;
				itsUserData.vertical  = true;
				itsUserData.key       = &GetScroller();
				
				itsVertical.Create( MakeVBounds( itsBounds ), &itsUserData );
				
				Resize_Subviews();
			}
			
			void SetBounds( const Rect& bounds );
			
			void UpdateScrollbars();
			
			void Resize_Subviews();
			
			Ped::View& Subview()  { return itsFrame; }
			
			Scroller& GetScroller()  { return static_cast< Scroller& >( itsFrame.Subview() ); }
			
			const Rect& Bounds() const  { return itsBounds; }
			
			Scrollbar& GetHorizontal()  { return itsHorizontal; }
			Scrollbar& GetVertical  ()  { return itsVertical;   }
			
	};
	
	void ScrollFrame::UpdateScrollbars()
	{
		Scroller& scroller = static_cast< Scroller& >( itsFrame.Subview() );
		
		itsVertical.Adjust( scroller.ClientHeight(),
		                    scroller.GetVOffset(),
		                    itsBounds.bottom - itsBounds.top );
	}
	
	void ScrollFrame::Resize_Subviews()
	{
		Rect v_bounds = MakeVBounds( itsBounds );
		
		itsVertical.UpdateBounds( v_bounds );
		
		itsFrame.SetBounds( ApertureFromBounds( itsBounds ) );
	}
	
	void ScrollFrame::SetBounds( const Rect& bounds )
	{
		itsBounds = bounds;
		
		Resize_Subviews();
	}
	
	
	void SetText( ScrollFrame& view, const plus::string& text )
	{
		Frame& frame = static_cast< Frame& >( view.Subview() );
		
		Scroller& scroller = static_cast< Scroller& >( frame.Subview() );
		
		TextEdit& textedit = scroller;
		
		Handle hText = textedit.Get()[0]->hText;
		
		N::SetHandleSize( hText, text.size() );
		
		std::copy( text.begin(), text.end(), *hText );
		
		N::TECalText( textedit.Get() );
	}
	
	
	static Rect MakeWindowRect()
	{
		Rect screenBounds = N::GetQDGlobalsScreenBits().bounds;
		
		Rect rect = N::SetRect(0, 18, 2*4+6*80+16, 18+2*4+11*25+16);
		short mbarHeight = ::GetMBarHeight();
		short vMargin = (screenBounds.bottom - rect.bottom) - mbarHeight;
		short hMargin = (screenBounds.right - rect.right);
		
		return N::OffsetRect(rect, hMargin / 2, mbarHeight + vMargin / 3);
	}
	
	static inline std::auto_ptr< Ped::View > MakeView()
	{
		std::auto_ptr< Ped::View > view( new ScrollFrame() );
		
		return view;
	}
	
	std::auto_ptr< Ped::Window > NewWindow( ConstStr255Param title )
	{
		Ped::NewWindowContext context( MakeWindowRect(), title );
		
		std::auto_ptr< Ped::Window > window( new Ped::UserWindow( context ) );
		
		window->SetView( MakeView() );
		
		return window;
	}
	
}

