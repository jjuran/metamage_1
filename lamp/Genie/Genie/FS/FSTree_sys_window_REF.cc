/*	========================
 *	FSTree_sys_window_REF.cc
 *	========================
 */

#include "Genie/FS/FSTree_sys_window_REF.hh"

// POSIX
#include <fcntl.h>

// iota
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/make_string.hh"

// nucleus
#include "nucleus/saved.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Window.hh"

// Genie
#include "Genie/FS/focusable_views.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/ReadableSymLink.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/Views.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/Process.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	static const char* gGestures[] =
	{
		"accept",
		"cancel"
	};
	
	static const int n_gestures = sizeof gGestures / sizeof gGestures[0];
	
	static const Point gZeroPoint = { 0 };
	
	struct WindowParameters
	{
		N::Str255           itsTitle;
		Point               itsOrigin;
		Point               itsSize;
		N::WindowDefProcID  itsProcID;
		bool                itIsVisible;
		bool                itHasCloseBox;
		
		boost::intrusive_ptr< Ped::Window >  itsWindow;
		boost::intrusive_ptr< Ped::View   >  itsSubview;
		
		const FSTree* itsFocus;
		
		plus::string itsGesturePaths[ n_gestures ];
		
		FSTreePtr                    itsTTYDelegate;
		boost::weak_ptr< IOHandle >  itsTerminal;
		
		WindowParameters() : itsOrigin( gZeroPoint ),
		                     itsSize  ( gZeroPoint ),
		                     itsProcID( N::documentProc ),
		                     itIsVisible  ( true ),
		                     itHasCloseBox( true ),
		                     itsSubview( Ped::EmptyView::Get() )
		{
		}
	};
	
	typedef simple_map< const FSTree*, WindowParameters > WindowParametersMap;
	
	static WindowParametersMap gWindowParametersMap;
	
	
	static void CenterWindowRect( Rect& bounds )
	{
		// Pre-conditions:  bounds is set to { 0, 0, v, h }
		
		const short topMargin = ::GetMBarHeight() + 18;  // FIXME:  Calculate title bar height
		
		BitMap screenBits = N::GetQDGlobalsScreenBits();
		
		short spareWidth = screenBits.bounds.right - bounds.right;
		
		short spareHeight = screenBits.bounds.bottom - bounds.bottom - topMargin;
		
		::OffsetRect( &bounds,
		              spareWidth / 2,
		              topMargin + spareHeight / 3 );
	}
	
	
	class Window : public Ped::Window
	{
		private:
			const FSTree* itsKey;
		
		public:
			Window( const FSTree*                 key,
			        const Ped::NewWindowContext&  context )
			:
				Ped::Window( context ),
				itsKey( key )
			{
			}
			
			boost::intrusive_ptr< Ped::View >& GetView();
	};
	
	boost::intrusive_ptr< Ped::View >& Window::GetView()
	{
		return gWindowParametersMap[ itsKey ].itsSubview;
	}
	
	
	static bool Disconnect_Window_Terminal( boost::weak_ptr< IOHandle >& terminal_weak )
	{
		if ( !terminal_weak.expired() )
		{
			const boost::shared_ptr< IOHandle >& terminal_shared = terminal_weak.lock();
			
			TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *terminal_shared ) );
			
			terminal.Disconnect();
			
			terminal_weak.reset();
			
			return true;
		}
		
		return false;
	}
	
	static void Destroy_Window( boost::intrusive_ptr< Ped::Window >& window, const FSTree* key )
	{
		if ( window.get() )
		{
			UninstallViewFromWindow( window->GetView(), key );
			
			window.reset();
		}
	}
	
	static void CloseUserWindow( const FSTree* key )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( key ) )
		{
			WindowParameters& params = *it;
			
			if ( !Disconnect_Window_Terminal( params.itsTerminal ) )
			{
				// tty file is not open for this window, just close the window
				
				Destroy_Window( params.itsWindow, key );
			}
		}
	}
	
	class UserWindowCloseHandler : public Ped::WindowCloseHandler
	{
		private:
			const FSTree* itsKey;
		
		public:
			UserWindowCloseHandler( const FSTree* key ) : itsKey( key )
			{
			}
			
			void operator()( WindowRef ) const  { CloseUserWindow( itsKey ); }
	};
	
	class WindowResizeHandler : public Ped::WindowResizeHandler
	{
		private:
			const FSTree* itsKey;
		
		public:
			WindowResizeHandler( const FSTree* key ) : itsKey( key )
			{
			}
			
			void operator()( WindowRef window, short h, short v ) const;
	};
	
	void WindowResizeHandler::operator()( WindowRef window, short h, short v ) const
	{
		N::SizeWindow( window, h, v, true );
		
		if ( WindowParameters* it = gWindowParametersMap.find( itsKey ) )
		{
			WindowParameters& params = *it;
			
			if ( !params.itsTerminal.expired() )
			{
				const boost::shared_ptr< IOHandle >& handle = params.itsTerminal.lock();
				
				TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *handle ) );
				
				send_signal_to_foreground_process_group_of_terminal( SIGWINCH, terminal );
			}
		}
	}
	
	
	static bool HasWindow( const FSTree* that )
	{
		const FSTree* key = that->ParentRef().get();
		
		return gWindowParametersMap[ key ].itsWindow.get() != NULL;
	}
	
	static void CreateUserWindow( const FSTree* key )
	{
		WindowParameters* it = gWindowParametersMap.find( key );
		
		if ( it == NULL )
		{
			p7::throw_errno( EPERM );
		}
		
		WindowParameters& params = *it;
		
		ConstStr255Param title = params.itsTitle;
		
		Rect bounds = { 0, 0, 90, 120 };
		
		if ( params.itsSize.h || params.itsSize.v )
		{
			reinterpret_cast< Point* >( &bounds )[1] = params.itsSize;
		}
		
		if ( params.itsOrigin.h || params.itsOrigin.v )
		{
			::OffsetRect( &bounds, params.itsOrigin.h, params.itsOrigin.v );
		}
		else
		{
			CenterWindowRect( bounds );
		}
		
		Ped::NewWindowContext context( bounds,
		                               title,
		                               params.itIsVisible,
		                               params.itsProcID,
		                               kFirstWindowOfClass,
		                               params.itHasCloseBox );
		
		boost::intrusive_ptr< Ped::Window > window( new Window( key, context ) );
		
		boost::intrusive_ptr< Ped::WindowCloseHandler > closeHandler( new UserWindowCloseHandler( key ) );
		
		window->SetCloseHandler( closeHandler );
		
		boost::intrusive_ptr< Ped::WindowResizeHandler > resizeHandler( new WindowResizeHandler( key ) );
		
		window->SetResizeHandler( resizeHandler );
		
		params.itsWindow = window;
		
		// We could copy from above but the actual bounds could be different
		bounds = N::GetPortBounds( N::GetWindowPort( window->Get() ) );
		
		params.itsSubview->Install( bounds );
		
		params.itsSubview->Activate( true );
	}
	
	void RemoveUserWindow( const FSTree* key )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( key ) )
		{
			WindowParameters& params = *it;
			
			Disconnect_Window_Terminal( params.itsTerminal );
			
			Destroy_Window( params.itsWindow, key );
		}
		
		gWindowParametersMap.erase( key );
		
		RemoveAllViewParameters( key );
	}
	
	void NotifyWindowOfViewLoss( const FSTree* window_key, const FSTree* view )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( window_key ) )
		{
			WindowParameters& params = *it;
			
			if ( params.itsFocus == view )
			{
				params.itsFocus = NULL;
			}
		}
	}
	
	
	class FSTree_sys_window_REF_Property : public FSTree_Property
	{
		private:
			bool itIsMutable;  // can this be changed after window is created?
		
		public:
			FSTree_sys_window_REF_Property( const FSTreePtr&     parent,
			                                const plus::string&  name,
			                                size_t               fixed_size,
			                                ReadHook             readHook,
			                                WriteHook            writeHook,
			                                bool                 mutability )
			:
				FSTree_Property( parent,
				                 name,
				                 fixed_size,
				                 readHook,
				                 writeHook ),
				itIsMutable( mutability )
			{
			}
			
			bool IsLink() const  { return itIsMutable && HasWindow( this ); }
			
			mode_t FilePermMode() const;
			
			plus::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	mode_t FSTree_sys_window_REF_Property::FilePermMode() const
	{
		const bool has_window = HasWindow( this );
		
		return   !has_window ? S_IRUSR | S_IWUSR            // params are r/w
		       : itIsMutable ? S_IRUSR | S_IWUSR | S_IXUSR  // symlink
		       :               S_IRUSR;                     // fixed attribute
	}
	
	plus::string FSTree_sys_window_REF_Property::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		return "ref/" + Name();
	}
	
	FSTreePtr FSTree_sys_window_REF_Property::ResolveLink() const
	{
		return ResolveRelativePath( ReadLink(), ParentRef() );
	}
	
	boost::shared_ptr< IOHandle > FSTree_sys_window_REF_Property::Open( OpenFlags flags ) const
	{
		const bool writing = (flags & O_ACCMODE) + 1 & FWRITE;
		
		if ( writing  &&  !itIsMutable  &&  HasWindow( this ) )
		{
			// Attempt to open for writing a fixed attribute, like proc ID
			p7::throw_errno( EACCES );
		}
		
		return FSTree_Property::Open( flags );
	}
	
	
	class SetWindowPort_Scope
	{
		private:
			n::saved< N::Port > savePort;
			
			SetWindowPort_Scope           ( const SetWindowPort_Scope& );
			SetWindowPort_Scope& operator=( const SetWindowPort_Scope& );
		
		public:
			SetWindowPort_Scope()
			{
			}
			
			SetWindowPort_Scope( WindowRef window )
			{
				N::SetPortWindowPort( window );
			}
	};
	
	static void InvalidateWindowRef( WindowRef window )
	{
		ASSERT( window != NULL );
		
		if ( ::IsPortVisibleRegionEmpty( N::GetWindowPort( window ) ) )
		{
			return;
		}
		
		SetWindowPort_Scope scope( window );
		
		N::InvalRect( N::GetPortBounds( N::GetWindowPort( window ) ) );
		
		Ped::ScheduleImmediateEventCheck();
	}
	
	static WindowRef GetWindowRef( const FSTree* key )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( key ) )
		{
			const boost::intrusive_ptr< Ped::Window >& window = it->itsWindow;
			
			if ( window.get() != NULL )
			{
				return window->Get();
			}
		}
		
		return NULL;
	}
	
	static void FocusViewInWindow( Ped::View& view, const FSTree* window_key )
	{
		if ( N::WindowRef window = GetWindowRef( window_key ) )
		{
			if ( window == FrontWindow() )
			{
				SetWindowPort_Scope scope( window );
				
				view.Focus();
			}
		}
	}
	
	static void BlurViewInWindow( Ped::View& view, const FSTree* window_key )
	{
		if ( N::WindowRef window = GetWindowRef( window_key ) )
		{
			if ( window == FrontWindow() )
			{
				SetWindowPort_Scope scope( window );
				
				view.Blur();
			}
		}
	}
	
	bool InvalidateWindow( const FSTree* key )
	{
		if ( WindowRef window = GetWindowRef( key ) )
		{
			InvalidateWindowRef( window );
			
			return true;
		}
		
		return false;
	}
	
	void InstallViewInWindow( const boost::intrusive_ptr< Ped::View >& view, const FSTree* key )
	{
		if ( WindowRef window = GetWindowRef( key ) )
		{
			SetWindowPort_Scope scope( window );
			
			InvalidateWindowRef( window );
			
			Rect bounds = N::GetPortBounds( N::GetWindowPort( window ) );
			
			view->Install( bounds );
			
			if ( window == N::FrontWindow() )
			{
				view->Activate( true );
			}
		}
	}
	
	void UninstallViewFromWindow( const boost::intrusive_ptr< Ped::View >& view, const FSTree* key )
	{
		if ( WindowRef window = GetWindowRef( key ) )
		{
			SetWindowPort_Scope scope( window );
			
			InvalidateWindowRef( window );
			
			view->Uninstall();
		}
	}
	
	
	class FSTree_sys_window_REF_focus : public FSTree
	{
		public:
			FSTree_sys_window_REF_focus( const FSTreePtr&     parent,
			                             const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return ParentRef().get(); }
			
			bool Exists() const;
			
			bool IsLink() const  { return Exists(); }
			
			void Delete() const;
			
			void SymLink( const plus::string& target ) const;
			
			plus::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
	bool FSTree_sys_window_REF_focus::Exists() const
	{
		return gWindowParametersMap[ WindowKey() ].itsFocus != NULL;
	}
	
	void FSTree_sys_window_REF_focus::Delete() const
	{
		const FSTree* focus_file = gWindowParametersMap[ WindowKey() ].itsFocus;
		
		if ( Ped::View* focus_view = get_focusable_view( focus_file ) )
		{
			BlurViewInWindow( *focus_view, WindowKey() );
		}
		
		gWindowParametersMap[ WindowKey() ].itsFocus = NULL;
	}
	
	void FSTree_sys_window_REF_focus::SymLink( const plus::string& target ) const
	{
		const FSTreePtr targeted_file = ResolvePathname( target, ParentRef() )->Lookup( "." );
		
		Ped::View* target_view = get_focusable_view( targeted_file.get() );
		
		if ( target_view == NULL )
		{
			// The target either doesn't exist or isn't a focusable view
			throw p7::errno_t( EINVAL );
		}
		
		// symlinkat() won't call us if the file exists
		ASSERT( gWindowParametersMap[ WindowKey() ].itsFocus == NULL );
		
		FocusViewInWindow( *target_view, WindowKey() );
		
		gWindowParametersMap[ WindowKey() ].itsFocus = targeted_file.get();
	}
	
	plus::string FSTree_sys_window_REF_focus::ReadLink() const
	{
		return ResolveLink()->Pathname();  // FIXME:  Use relative path
	}
	
	FSTreePtr FSTree_sys_window_REF_focus::ResolveLink() const
	{
		if ( const FSTree* focus = gWindowParametersMap[ WindowKey() ].itsFocus )
		{
			return FSTreePtr( focus );
		}
		
		throw p7::errno_t( ENOENT );
	}
	
	const FSTree* GetWindowFocus( const FSTree* window )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( window ) )
		{
			WindowParameters& params = *it;
			
			return params.itsFocus;
		}
		
		return NULL;
	}
	
	void SetWindowFocus( const FSTree* window, const FSTree* focus )
	{
		gWindowParametersMap[ window ].itsFocus = focus;
	}
	
	
	class FSTree_sys_window_REF_ref : public FSTree_ReadableSymLink
	{
		public:
			FSTree_sys_window_REF_ref( const FSTreePtr&     parent,
			                           const plus::string&  name )
			:
				FSTree_ReadableSymLink( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return ParentRef().get(); }
			
			bool Exists() const  { return HasWindow( this ); }
			
			bool IsLink() const  { return Exists(); }
			
			void SetTimes() const;
			
			void Delete() const;
			
			plus::string ReadLink() const;
	};
	
	
	void FSTree_sys_window_REF_ref::SetTimes() const
	{
		const FSTree* key = WindowKey();
		
		if ( !InvalidateWindow( key ) )
		{
			CreateUserWindow( key );
		}
	}
	
	void FSTree_sys_window_REF_ref::Delete() const
	{
		const FSTree* key = WindowKey();
		
		CloseUserWindow( key );
	}
	
	#define SYS_APP_WINDOW_LIST  "/sys/app/window/list/"
	
	plus::string FSTree_sys_window_REF_ref::ReadLink() const
	{
		WindowRef windowPtr = GetWindowRef( WindowKey() );
		
		if ( windowPtr == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		plus::var_string result = SYS_APP_WINDOW_LIST "12345678";
		
		const size_t hex_offset = STRLEN( SYS_APP_WINDOW_LIST );
		
		iota::encode_32_bit_hex( (unsigned) windowPtr, &result[ hex_offset ] );
		
		return result;
	}
	
	
	class FSTree_sys_window_REF_tty : public FSTree
	{
		public:
			FSTree_sys_window_REF_tty( const FSTreePtr&     parent,
			                           const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return ParentRef().get(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			void Attach( const FSTreePtr& target ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	void FSTree_sys_window_REF_tty::Attach( const FSTreePtr& target ) const
	{
		gWindowParametersMap[ WindowKey() ].itsTTYDelegate = target;
	}
	
	static inline boost::shared_ptr< IOHandle >
	//
	NewTerminal( const plus::string& name )
	{
		return seize_ptr( new TerminalHandle( name ) );
	}
	
	boost::shared_ptr< IOHandle >
	//
	FSTree_sys_window_REF_tty::Open( OpenFlags flags ) const
	{
		WindowParameters& params = gWindowParametersMap[ WindowKey() ];
		
		boost::shared_ptr< IOHandle > tty;
		
		const bool has_tty = params.itsTTYDelegate.get() != NULL;
		
		if ( has_tty )
		{
			tty = params.itsTTYDelegate->Open( flags );
		}
		
		plus::string pathname = ( has_tty ? tty->GetFile().get()
		                                  : this                 )->Pathname();
		
		boost::shared_ptr< IOHandle > terminal = NewTerminal( pathname );
		
		if ( has_tty )
		{
			tty->Attach( terminal );
			
			terminal->Attach( tty );
		}
		
		params.itsTerminal = terminal;
		
		return terminal;
	}
	
	static int LookupGesture( const plus::string& name )
	{
		for ( int i = 0;  i < n_gestures;  ++i )
		{
			const char* p = gGestures[ i ];
			
			if ( memcmp( p, name.c_str(), name.size() + 1 ) == 0 )
			{
				return i;
			}
		}
		
		return -1;
	}
	
	class FSTree_Window_Gesture : public FSTree_ReadableSymLink
	{
		private:
			int itsIndex;
		
		public:
			FSTree_Window_Gesture( const FSTreePtr&     parent,
			                       const plus::string&  name );
			
			bool Exists() const;
			
			bool IsLink() const  { return Exists(); }
			
			void Delete() const;
			
			void SymLink( const plus::string& target ) const;
			
			plus::string ReadLink() const;
	};
	
	FSTree_Window_Gesture::FSTree_Window_Gesture( const FSTreePtr&     parent,
	                                              const plus::string&  name )
	:
		FSTree_ReadableSymLink( parent, name ),
		itsIndex( LookupGesture( name ) )
	{
		ASSERT( itsIndex != -1 );
	}
	
	bool FSTree_Window_Gesture::Exists() const
	{
		const FSTree* view = GetViewKey( this );
		
		return !gWindowParametersMap[ view ].itsGesturePaths[ itsIndex ].empty();
	}
	
	void FSTree_Window_Gesture::Delete() const
	{
		const FSTree* view = GetViewKey( this );
		
		WindowParameters& params = gWindowParametersMap[ view ];
		
		params.itsGesturePaths[ itsIndex ] = "";
	}
	
	void FSTree_Window_Gesture::SymLink( const plus::string& target_path ) const
	{
		const FSTree* view = GetViewKey( this );
		
		WindowParameters& params = gWindowParametersMap[ view ];
		
		params.itsGesturePaths[ itsIndex ] = target_path;
	}
	
	plus::string FSTree_Window_Gesture::ReadLink() const
	{
		const FSTree* view = GetViewKey( this );
		
		const plus::string& link = gWindowParametersMap[ view ].itsGesturePaths[ itsIndex ];
		
		if ( link.empty() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return link;
	}
	
	
	namespace
	{
		
		boost::intrusive_ptr< Ped::View >& GetView( const FSTree* key )
		{
			return gWindowParametersMap[ key ].itsSubview;
		}
		
	}
	
	
	static WindowParameters& Find( const FSTree* key )
	{
		WindowParameters* it = gWindowParametersMap.find( key );
		
		if ( it == NULL )
		{
			throw FSTree_Property::Undefined();
		}
		
		return *it;
	}
	
	
	struct Window_Title
	{
		static const bool fixed_size = 0;
		
		static plus::string Get( const FSTree* that, bool binary )
		{
			return plus::make_string( Find( GetViewKey( that ) ).itsTitle );
		}
		
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			WindowParameters& params = gWindowParametersMap[ view ];
			
			params.itsTitle = N::Str255( begin, end - begin );
		}
	};
	
	template < class Scribe, typename Scribe::Value& (*Access)( WindowParameters& ) >
	struct Window_Property
	{
		static const bool fixed_size = sizeof (typename Scribe::Value);
		
		static plus::string Get( const FSTree* that, bool binary )
		{
			return Freeze< Scribe >( Access( Find( GetViewKey( that ) ) ), binary );
		}
		
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			WindowParameters& params = gWindowParametersMap[ view ];
			
			Access( params ) = Vivify< Scribe >( begin, end, binary );
		}
	};
	
	
	namespace
	{
		
		Point& Origin( WindowParameters& params )
		{
			return params.itsOrigin;
		}
		
		Point& Size( WindowParameters& params )
		{
			return params.itsSize;
		}
		
		N::WindowDefProcID& ProcID( WindowParameters& params )
		{
			return params.itsProcID;
		}
		
		bool& Visible( WindowParameters& params )
		{
			return params.itIsVisible;
		}
		
		bool& CloseBox( WindowParameters& params )
		{
			return params.itHasCloseBox;
		}
		
	}
	
	enum Variability
	{
		kAttrConstant,
		kAttrVariable
	};
	
	template < Variability variability, class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name )
	{
		return seize_ptr( new FSTree_sys_window_REF_Property( parent,
		                                                      name,
		                                                      Property::fixed_size,
		                                                      &Property::Get,
		                                                      &Property::Set,
		                                                      variability ) );
	}
	
	typedef Integer_Scribe< N::WindowDefProcID >  ProcID_Scribe;
	
	const FSTree_Premapped::Mapping sys_window_REF_Mappings[] =
	{
		{ "ref",    &Basic_Factory< FSTree_sys_window_REF_ref >, true },
		
		{ "view",   &Basic_Factory< FSTree_X_view< GetView > >, true },
		
		{ "focus",  &Basic_Factory< FSTree_sys_window_REF_focus >, true },
		
		{ "accept", &Basic_Factory< FSTree_Window_Gesture >, true },
		{ "cancel", &Basic_Factory< FSTree_Window_Gesture >, true },
		
		{ "tty",    &Basic_Factory< FSTree_sys_window_REF_tty > },
		
		{ "title",  &Property_Factory< kAttrVariable, Window_Title > },
		{ "pos",    &Property_Factory< kAttrVariable, Window_Property< Point_Scribe< ',' >, &Origin   > > },
		{ "size",   &Property_Factory< kAttrVariable, Window_Property< Point_Scribe< 'x' >, &Size     > > },
		{ "vis",    &Property_Factory< kAttrVariable, Window_Property< Boolean_Scribe,      &Visible  > > },
		{ "procid", &Property_Factory< kAttrConstant, Window_Property< ProcID_Scribe,       &ProcID   > > },
		{ "goaway", &Property_Factory< kAttrConstant, Window_Property< Boolean_Scribe,      &CloseBox > > },
		
		{ NULL, NULL }
	};
	
}

