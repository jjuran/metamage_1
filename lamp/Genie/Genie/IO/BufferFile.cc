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
			std::size_t  itsMark;
			bool         itHasReceivedEOF;
		
		public:
			BufferView( const Rect& bounds ) : Ped::TEView( bounds ),
			                                   itsMark(),
			                                   itHasReceivedEOF()
			{
			}
			
			bool KeyDown( const EventRecord& event );
			
			bool ReceivedEOF() const  { return itHasReceivedEOF; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			off_t Seek( off_t offset, int whence );
			
			off_t GetEOF() const;
			
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
	
	int BufferView::SysRead( char* data, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		Handle hText = TextHandle();
		
		std::size_t teLength = TextLength();
		
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
	
	int BufferView::SysWrite( const char* data, std::size_t byteCount )
	{
		SetEOF( itsMark );
		
		int result = AppendChars( data, byteCount, true );
		
		itsMark += result;
		
		return result;
	}
	
	
	off_t BufferView::Seek( off_t offset, int whence )
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
	
	off_t BufferView::GetEOF() const
	{
		return TextLength();
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
	
	static inline std::auto_ptr< Ped::View > MakeView()
	{
		Rect scroller_bounds = MakeWindowRect();
		
		Rect subview_bounds = Pedestal::ScrollBounds< true, false >( scroller_bounds );
		
		Scroller* scroller = NULL;
		
		std::auto_ptr< Ped::View > view( scroller = new Scroller( scroller_bounds ) );
		
		std::auto_ptr< Ped::ScrollableBase > subview( new BufferView( subview_bounds ) );
		
		scroller->SetSubView( subview );
		
		return view;
	}
	
	
	BufferWindow::BufferWindow( TerminalID          id,
	                            const std::string&  name ) : Base( Ped::NewWindowContext( MakeWindowRect(),
	                                                                                      "\p" "Edit",
	                                                                                      false ),
	                                                               N::documentProc ),
	                                                         WindowHandle( name )
	{
		SetCloseHandler ( GetDynamicWindowCloseHandler < BufferFileHandle >( id ) );
		SetResizeHandler( GetDynamicWindowResizeHandler< BufferFileHandle >( id ) );
		
		SetView( MakeView() );
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
	
	unsigned int BufferFileHandle::SysPoll() const
	{
		BufferWindow& window = *static_cast< BufferWindow* >( itsWindow.get() );
		
		const Scroller& scroller = window.SubView().Get< Scroller >();
		
		const BufferView& view = scroller.GetSubView< BufferView >();
		
		unsigned readability = view.ReceivedEOF() ? kPollRead : 0;
		
		return readability | kPollWrite;
	}
	
	int BufferFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		BufferWindow& window = *static_cast< BufferWindow* >( itsWindow.get() );
		
		window.Show();
		
		Scroller& scroller = window.SubView().Get< Scroller >();
		
		BufferView& view = scroller.GetSubView< BufferView >();
		
		while ( !view.ReceivedEOF() )
		{
			TryAgainLater();
		}
		
		// Zero-byte read handled by view
		
		return view.SysRead( data, byteCount );
	}
	
	int BufferFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		BufferWindow& window = *static_cast< BufferWindow* >( itsWindow.get() );
		
		window.Show();
		
		Scroller& scroller = window.SubView().Get< Scroller >();
		
		BufferView& view = scroller.GetSubView< BufferView >();
		
		int result = view.SysWrite( data, byteCount );
		
		scroller.UpdateScrollbars( N::SetPt( 0, 0 ),
		                           N::SetPt( 0, 0 ) );
		
		return result;
	}
	
	off_t BufferFileHandle::Seek( off_t offset, int whence )
	{
		BufferWindow& window = *static_cast< BufferWindow* >( itsWindow.get() );
		
		Scroller& scroller = window.SubView().Get< Scroller >();
		
		BufferView& view = scroller.GetSubView< BufferView >();
		
		return view.Seek( offset, whence );
	}
	
	off_t BufferFileHandle::GetEOF() const
	{
		BufferWindow& window = *static_cast< BufferWindow* >( itsWindow.get() );
		
		Scroller& scroller = window.SubView().Get< Scroller >();
		
		BufferView& view = scroller.GetSubView< BufferView >();
		
		return view.TextLength();
	}
	
	void BufferFileHandle::SetEOF( off_t length )
	{
		BufferWindow& window = *static_cast< BufferWindow* >( itsWindow.get() );
		
		Scroller& scroller = window.SubView().Get< Scroller >();
		
		BufferView& view = scroller.GetSubView< BufferView >();
		
		view.SetEOF( length );
	}
	
}

