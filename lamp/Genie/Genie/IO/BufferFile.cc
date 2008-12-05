/*	=============
 *	BufferFile.cc
 *	=============
 */

#include "Genie/IO/BufferFile.hh"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/Scroller.hh"
#include "Pedestal/TEView.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	class BufferView : public Ped::TEView
	{
		private:
			bool itHasReceivedEOF;
		
		public:
			BufferView( const Rect& bounds ) : Ped::TEView( bounds ),
			                                   itHasReceivedEOF()
			{
			}
			
			bool KeyDown( const EventRecord& event );
			
			bool ReceivedEOF() const  { return itHasReceivedEOF; }
			
			ssize_t ReadFrom( char* buffer, std::size_t n, SInt16 mark );
			
			ssize_t WriteTo( const char* buffer, std::size_t n, SInt16 mark );
			
			off_t GetEOF() const  { return TextLength(); }
			
			void SetEOF( off_t length );
		
	};
	
	static bool EventSignalsEOF( const EventRecord& event )
	{
		char c   =  event.message & charCodeMask;
		char key = (event.message & keyCodeMask) >> 8;
		
		if ( c == kEnterCharCode  &&  key >= 0x30 )
		{
			// Enter key
			return true;
		}
		
		if ( event.modifiers & controlKey  &&  c < 0x20  &&  (c | 0x40) == 'D' )
		{
			return true;
		}
		
		return false;
	}
	
	bool BufferView::KeyDown( const EventRecord& event )
	{
		if ( EventSignalsEOF( event ) )
		{
			itHasReceivedEOF = true;
			
			return true;
		}
		
		return Ped::TEView::KeyDown( event );
	}
	
	ssize_t BufferView::ReadFrom( char* buffer, std::size_t n, SInt16 mark )
	{
		if ( n == 0 )
		{
			return 0;
		}
		
		Handle hText = TextHandle();
		
		std::size_t teLength = TextLength();
		
		std::size_t hLength = N::GetHandleSize( N::Handle( hText ) );
		
		ASSERT( teLength == hLength );
		
		if ( mark >= teLength )
		{
			return 0;
		}
		
		// Handle dereferenced here
		const char* begin = *hText;
		const char* end   = begin + hLength;
		
		std::size_t readableBytes = end - begin - mark;  // may be zero
		
		n = std::min( n, readableBytes );
		
		const char* marked = begin + mark;
		
		std::copy( marked, marked + n, buffer );
		
		// CR -> LF
		std::replace( buffer, buffer + n, '\r', '\n' );
		
		return n;
	}
	
	ssize_t BufferView::WriteTo( const char* buffer, std::size_t n, SInt16 mark )
	{
		SetEOF( mark );
		
		ssize_t written = AppendChars( buffer, n, true );
		
		return written;
	}
	
	
	void BufferView::SetEOF( off_t length )
	{
		// Handle dereferenced here
		TERec& te = *Get()[0];
		
		if ( length > te.teLength )
		{
			p7::throw_errno( EINVAL );
		}
		else if ( length == te.teLength )
		{
			return;
		}
		
		te.teLength = length;
		
		SetHandleSize( TextHandle(), length );
		
		SetSelection( length, length );
	}
	
	
	static Rect MakeWindowRect()
	{
		BitMap screenBits = N::GetQDGlobalsScreenBits();
		
		Rect rect = N::SetRect(0, 18, 2*4+6*80+16, 18+2*4+11*25+16);
		short mbarHeight = ::GetMBarHeight();
		short vMargin = (screenBits.bounds.bottom - rect.bottom) - mbarHeight;
		short hMargin = (screenBits.bounds.right  - rect.right);
		
		return N::OffsetRect( rect,
		                      hMargin / 2,
		                      mbarHeight + vMargin / 3 );
	}
	
	
	typedef Ped::Scroller< true > Scroller;
	
	static inline std::auto_ptr< Ped::View > MakeView( const Rect& scroller_bounds )
	{
		Rect subview_bounds = Ped::ScrollBounds< true, false >( scroller_bounds );
		
		Scroller* scroller = NULL;
		
		std::auto_ptr< Ped::View > view( scroller = new Scroller( scroller_bounds ) );
		
		std::auto_ptr< Ped::ScrollableBase > subview( new BufferView( subview_bounds ) );
		
		scroller->SetSubView( subview );
		
		return view;
	}
	
	
	static BufferView& GetBuffer( const boost::shared_ptr< IOHandle >& h )
	{
		WindowHandle& window = *static_cast< WindowHandle* >( h.get() );
		
		Scroller& scroller = window.SubView().Get< Scroller >();
		
		BufferView& view = scroller.GetSubView< BufferView >();
		
		return view;
	}
	
	BufferFileHandle::BufferFileHandle( TerminalID          id,
	                                    const std::string&  name )
	{
		Rect bounds = MakeWindowRect();
		
		Ped::NewWindowContext context( bounds,
	                                   "\p" "Edit",
	                                   false );
		
		WindowHandle* window = new WindowHandle( context, N::documentProc, name );
		
		itsWindow.reset( window );
		
		window->SetCloseHandler ( GetDynamicWindowCloseHandler < BufferFileHandle >( id ) );
		window->SetResizeHandler( GetDynamicWindowResizeHandler< BufferFileHandle >( id ) );
		
		window->SetView( MakeView( bounds ) );
	}
	
	BufferFileHandle::~BufferFileHandle()
	{
	}
	
	IOHandle* BufferFileHandle::Next() const
	{
		return itsWindow.get();
	}
	
	unsigned int BufferFileHandle::SysPoll() const
	{
		unsigned readability = GetBuffer( itsWindow ).ReceivedEOF() ? kPollRead : 0;
		
		return readability | kPollWrite;
	}
	
	int BufferFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		WindowHandle& window = *static_cast< WindowHandle* >( itsWindow.get() );
		
		Scroller& scroller = window.SubView().Get< Scroller >();
		
		BufferView& view = scroller.GetSubView< BufferView >();
		
		window.Show();
		
		while ( !view.ReceivedEOF() )
		{
			TryAgainLater();
		}
		
		// Zero-byte read handled by view
		
		return Advance( view.ReadFrom( data, byteCount, GetFileMark() ) );
	}
	
	int BufferFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		WindowHandle& window = *static_cast< WindowHandle* >( itsWindow.get() );
		
		Scroller& scroller = window.SubView().Get< Scroller >();
		
		BufferView& view = scroller.GetSubView< BufferView >();
		
		window.Show();
		
		ssize_t written = view.WriteTo( data, byteCount, GetFileMark() );
		
		scroller.UpdateScrollbars( N::SetPt( 0, 0 ),
		                           N::SetPt( 0, 0 ) );
		
		return Advance( written );
	}
	
	off_t BufferFileHandle::GetEOF() const
	{
		return GetBuffer( itsWindow ).GetEOF();
	}
	
	void BufferFileHandle::SetEOF( off_t length )
	{
		GetBuffer( itsWindow ).SetEOF( length );
	}
	
}

