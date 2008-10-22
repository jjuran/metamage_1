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
	
	
	typedef Ped::Scroller< Ped::TEView, Ped::kLiveFeedbackVariant > BufferView;
	
	static inline std::auto_ptr< Ped::View > MakeView()
	{
		return std::auto_ptr< Ped::View >( new BufferView( MakeWindowRect(), BufferView::Initializer() ) );
	}
	
	
	BufferWindow::BufferWindow( TerminalID          id,
	                            const std::string&  name ) : Base( Ped::NewWindowContext( MakeWindowRect(),
	                                                                                      "\p" "Edit",
	                                                                                      false ),
	                                                               N::documentProc ),
	                                                         WindowHandle( name ),
	                                                         itsMark(),
	                                                         itHasReceivedEOF()
	{
		SetCloseHandler ( GetDynamicWindowCloseHandler < BufferFileHandle >( id ) );
		SetResizeHandler( GetDynamicWindowResizeHandler< BufferFileHandle >( id ) );
		
		SetView( MakeView() );
	}
	
	BufferWindow::~BufferWindow()
	{
	}
	
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
	
	bool BufferWindow::KeyDown( const EventRecord& event )
	{
		char c   =  event.message & charCodeMask;
		char key = (event.message & keyCodeMask) >> 8;
		
		bool eofSignalled = EventSignalsEOF( event );
		
		if ( eofSignalled )
		{
			itHasReceivedEOF = true;
			
			return true;
		}
		else
		{
			return SubView().KeyDown( event );
		}
	}
	
	int BufferWindow::SysRead( char* data, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		Ped::TEView& editor = SubView().Get< BufferView >().ScrolledView();
		
		Handle hText = editor.TextHandle();
		
		std::size_t teLength = editor.TextLength();
		
		std::size_t hLength = N::GetHandleSize( N::Handle( hText ) );
		
		ASSERT( teLength == hLength );
		
		if ( itsMark >= teLength )
		{
			return 0;
		}
		
		// Handle dereferenced here
		const char* begin = *hText;
		const char* end   = begin + hLength;
		
		std::size_t readableBytes = end - begin - itsMark;  // may be zero
		
		byteCount = std::min( byteCount, readableBytes );
		
		const char* marked = begin + itsMark;
		
		std::copy( marked, marked + byteCount, data );
		
		// CR -> LF
		std::replace( data, data + byteCount, '\r', '\n' );
		
		itsMark += byteCount;
		
		return byteCount;
	}
	
	int BufferWindow::SysWrite( const char* data, std::size_t byteCount )
	{
		Show();
		
		Ped::TEView& editor = SubView().Get< BufferView >().ScrolledView();
		
		SetEOF( itsMark );
		
		int result = editor.AppendChars( data, byteCount, true );
		
		itsMark += result;
		
		SubView().Get< BufferView >().UpdateScrollbars( N::SetPt( 0, 0 ),
		                                                N::SetPt( 0, 0 ) );
		
		return result;
	}
	
	
	off_t BufferWindow::Seek( off_t offset, int whence )
	{
		switch ( whence )
		{
			case SEEK_SET:
				itsMark = offset;
				break;
			
			case SEEK_CUR:
				itsMark += offset;
				break;
			
			case SEEK_END:
				itsMark = GetEOF() + offset;
				break;
			
			default:
				p7::throw_errno( EINVAL );
		}
		
		return itsMark;
	}
	
	off_t BufferWindow::GetEOF() const
	{
		return SubView().Get< BufferView >().ScrolledView().TextLength();
	}
	
	void BufferWindow::SetEOF( off_t length )
	{
		Ped::TEView& editor = SubView().Get< BufferView >().ScrolledView();
		
		// Handle dereferenced here
		TERec& te = *editor.Get()[0];
		
		if ( length > te.teLength )
		{
			p7::throw_errno( EINVAL );
		}
		else if ( length == te.teLength )
		{
			return;
		}
		
		te.teLength = length;
		
		SetHandleSize( editor.TextHandle(), length );
		
		editor.SetSelection( length, length );
	}
	
	
	BufferFileHandle::BufferFileHandle( TerminalID          id,
	                                    const std::string&  name ) : itsWindow( new BufferWindow( id, name ) )
	{
	}
	
	BufferFileHandle::~BufferFileHandle()
	{
	}
	
	IOHandle* BufferFileHandle::Next() const
	{
		return itsWindow.get();
	}
	
	int BufferFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		BufferWindow* window = static_cast< BufferWindow* >( itsWindow.get() );
		
		window->Show();
		
		while ( !window->ReceivedEOF() )
		{
			TryAgainLater();
		}
		
		// Zero-byte read handled by window
		
		return window->SysRead( data, byteCount );
	}
	
}

