/*
	Genie/FS/gui/port/ADDR.cc
	-------------------------
*/

#include "Genie/FS/gui/port/ADDR.hh"

// Standard C
#include <signal.h>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// iota
#include "iota/strings.hh"

// gear
#include "gear/hexidecimal.hh"

// plus
#include "plus/serialize.hh"
#include "plus/string/concat.hh"

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

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/getpgrp.hh"
#include "vfs/filehandle/primitives/hangup.hh"
#include "vfs/functions/new_static_symlink.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/node/types/symbolic_link.hh"
#include "vfs/primitives/lookup.hh"
#include "vfs/primitives/open.hh"

// relix
#include "relix/signal/signal_process_group.hh"

// Genie
#include "Genie/FS/focusable_views.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/file_method_set.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/serialize_qd.hh"
#include "Genie/FS/subview.hh"
#include "Genie/IO/Terminal.hh"
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
		bool                itIsLocked;
		
		boost::intrusive_ptr< Ped::Window >  itsWindow;
		boost::intrusive_ptr< Ped::View   >  itsSubview;
		
		const FSTree* itsFocus;
		
		plus::string itsGesturePaths[ n_gestures ];
		
		FSTreePtr  itsTTYDelegate;
		IOHandle*  itsTerminal;
		
		WindowParameters() : itsOrigin( gZeroPoint ),
		                     itsSize  ( gZeroPoint ),
		                     itsProcID( N::documentProc ),
		                     itIsVisible  ( true ),
		                     itHasCloseBox( true ),
		                     itIsLocked(),
		                     itsSubview( Ped::EmptyView::Get() ),
		                     itsFocus(),
		                     itsTerminal()
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
	
	
	static bool Disconnect_Window_Terminal( IOHandle*& h )
	{
		if ( h != NULL )
		{
			hangup( *h );
			
			h = NULL;
			
			return true;
		}
		
		return false;
	}
	
	static void Destroy_Window( boost::intrusive_ptr< Ped::Window >& window, const FSTree* key )
	{
		if ( window.get() )
		{
			uninstall_view_from_port( window->GetView(), key );
			
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
			
			if ( params.itsTerminal != NULL )
			{
				const pid_t pgid = getpgrp( *params.itsTerminal );
				
				relix::signal_process_group( SIGWINCH, pgid );
			}
		}
	}
	
	
	static bool port_is_locked( const FSTreePtr& port )
	{
		const FSTree* key = port.get();
		
		return gWindowParametersMap[ key ].itIsLocked;
	}
	
	static bool port_has_window( const FSTreePtr& port )
	{
		const FSTree* key = port.get();
		
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
	
	void remove_window_and_views_from_port( const FSTree* key )
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
	
	void notify_port_of_view_loss( const FSTree* port_key, const FSTree* view )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( port_key ) )
		{
			WindowParameters& params = *it;
			
			if ( params.itsFocus == view )
			{
				params.itsFocus = NULL;
			}
		}
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
		if ( WindowRef window = GetWindowRef( window_key ) )
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
		if ( WindowRef window = GetWindowRef( window_key ) )
		{
			if ( window == FrontWindow() )
			{
				SetWindowPort_Scope scope( window );
				
				view.Blur();
			}
		}
	}
	
	bool invalidate_port_WindowRef( const FSTree* key )
	{
		if ( WindowRef window = GetWindowRef( key ) )
		{
			InvalidateWindowRef( window );
			
			return true;
		}
		
		return false;
	}
	
	void install_view_in_port( const boost::intrusive_ptr< Ped::View >& view, const FSTree* key )
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
	
	void uninstall_view_from_port( const boost::intrusive_ptr< Ped::View >& view, const FSTree* key )
	{
		if ( WindowRef window = GetWindowRef( key ) )
		{
			SetWindowPort_Scope scope( window );
			
			InvalidateWindowRef( window );
			
			view->Uninstall();
		}
	}
	
	
	static void focus_remove( const FSTree* that )
	{
		const FSTree* window = that->owner();
		
		const FSTree* focus_file = gWindowParametersMap[ window ].itsFocus;
		
		if ( Ped::View* focus_view = get_focusable_view( focus_file ) )
		{
			BlurViewInWindow( *focus_view, window );
		}
		
		gWindowParametersMap[ window ].itsFocus = NULL;
	}
	
	static void unfocus_symlink( const FSTree*        that,
	                             const plus::string&  target )
	{
		const FSTree* parent = that->owner();
		
		const vfs::node_ptr targeted_file = lookup( *resolve_pathname( target, *parent ), plus::string::null );
		
		Ped::View* target_view = get_focusable_view( targeted_file.get() );
		
		if ( target_view == NULL )
		{
			// The target either doesn't exist or isn't a focusable view
			throw p7::errno_t( EINVAL );
		}
		
		const FSTree* window_key = parent;
		
		FocusViewInWindow( *target_view, window_key );
		
		gWindowParametersMap[ window_key ].itsFocus = targeted_file.get();
	}
	
	static const link_method_set unfocus_link_methods =
	{
		NULL,
		NULL,
		&unfocus_symlink
	};
	
	static const node_method_set unfocus_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&unfocus_link_methods
	};
	
	
	static FSTreePtr focus_resolve( const FSTree* that )
	{
		if ( const FSTree* focus = gWindowParametersMap[ that->owner() ].itsFocus )
		{
			return focus;
		}
		
		throw p7::errno_t( ENOENT );
	}
	
	static const link_method_set focus_link_methods =
	{
		NULL,  // FIXME:  Use relative path
		&focus_resolve
	};
	
	static const node_method_set focus_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&focus_remove,
		NULL,
		NULL,
		&focus_link_methods
	};
	
	
	const FSTree* get_port_focus( const FSTree* port )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( port ) )
		{
			WindowParameters& params = *it;
			
			return params.itsFocus;
		}
		
		return NULL;
	}
	
	void set_port_focus( const FSTree* port, const FSTree* focus )
	{
		gWindowParametersMap[ port ].itsFocus = focus;
	}
	
	
	static void window_touch( const FSTree* that )
	{
		invalidate_port_WindowRef( that->owner() );
	}
	
	static void window_remove( const FSTree* that )
	{
		CloseUserWindow( that->owner() );
	}
	
	#define SYS_APP_WINDOW_LIST  "/sys/app/window/list/"
	
	static plus::string window_readlink( const FSTree* that )
	{
		WindowRef windowPtr = GetWindowRef( that->owner() );
		
		if ( windowPtr == NULL )
		{
			p7::throw_errno( EINVAL );
		}
		
		plus::var_string result = SYS_APP_WINDOW_LIST "12345678";
		
		const size_t hex_offset = STRLEN( SYS_APP_WINDOW_LIST );
		
		gear::encode_32_bit_hex( (unsigned) windowPtr, &result[ hex_offset ] );
		
		return result;
	}
	
	static const link_method_set window_link_methods =
	{
		&window_readlink
	};
	
	static const node_method_set window_methods =
	{
		NULL,
		NULL,
		&window_touch,
		NULL,
		&window_remove,
		NULL,
		NULL,
		&window_link_methods
	};
	
	
	static IOPtr port_tty_open( const FSTree* that, int flags, mode_t mode );
	
	static void port_tty_attach( const FSTree* that, const FSTree* target )
	{
		gWindowParametersMap[ that->owner() ].itsTTYDelegate = target;
	}
	
	static const data_method_set port_tty_data_methods =
	{
		&port_tty_open
	};
	
	static const file_method_set port_tty_file_methods =
	{
		&port_tty_attach
	};
	
	static const node_method_set port_tty_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&port_tty_data_methods,
		NULL,
		NULL,
		&port_tty_file_methods
	};
	
	
	static void destroy_port_tty( vfs::filehandle* that )
	{
		WindowParameters& params = gWindowParametersMap[ that->GetFile()->owner() ];
		
		params.itsTerminal = NULL;
	}
	
	static inline IOPtr
	//
	NewTerminal( const plus::string& name )
	{
		return new TerminalHandle( name );
	}
	
	static IOPtr port_tty_open( const FSTree* that, int flags, mode_t mode )
	{
		WindowParameters& params = gWindowParametersMap[ that->owner() ];
		
		IOPtr tty;
		
		const bool has_tty = params.itsTTYDelegate.get() != NULL;
		
		if ( has_tty )
		{
			tty = open( *params.itsTTYDelegate, flags, 0 );
		}
		else
		{
			tty = new vfs::filehandle( that, 0, NULL, 0, &destroy_port_tty );
		}
		
		plus::string pathname = vfs::pathname( has_tty ? *tty->GetFile()
		                                               : *that           );
		
		IOPtr terminal = NewTerminal( pathname );
		
		if ( has_tty )
		{
			tty->Attach( terminal.get() );
		}
		
		terminal->Attach( tty.get() );
		
		params.itsTerminal = terminal.get();
		
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
	
	
	static void gesture_remove( const FSTree* that );
	
	static plus::string gesture_readlink( const FSTree* that );
	
	static const link_method_set gesture_link_methods =
	{
		&gesture_readlink
	};
	
	static const node_method_set gesture_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&gesture_remove,
		NULL,
		NULL,
		&gesture_link_methods
	};
	
	static void gesture_remove( const FSTree* that )
	{
		const FSTree* view = that->owner();
		
		WindowParameters& params = gWindowParametersMap[ view ];
		
		const int index = LookupGesture( that->name() );
		
		params.itsGesturePaths[ index ].reset();
	}
	
	static void ungesture_symlink( const FSTree*        that,
	                               const plus::string&  target_path )
	{
		const FSTree* view = that->owner();
		
		WindowParameters& params = gWindowParametersMap[ view ];
		
		const int index = LookupGesture( that->name() );
		
		params.itsGesturePaths[ index ] = target_path;
	}
	
	static const link_method_set ungesture_link_methods =
	{
		NULL,
		NULL,
		&ungesture_symlink
	};
	
	static const node_method_set ungesture_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&ungesture_link_methods
	};
	
	static plus::string gesture_readlink( const FSTree* that )
	{
		const FSTree* view = that->owner();
		
		const int index = LookupGesture( that->name() );
		
		const plus::string& link = gWindowParametersMap[ view ].itsGesturePaths[ index ];
		
		if ( link.empty() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return link;
	}
	
	
	namespace
	{
		
		boost::intrusive_ptr< Ped::View >& GetView( const FSTree* key, const plus::string& name )
		{
			return gWindowParametersMap[ key ].itsSubview;
		}
		
	}
	
	
	static WindowParameters& Find( const FSTree* key )
	{
		WindowParameters* it = gWindowParametersMap.find( key );
		
		if ( it == NULL )
		{
			throw undefined_property();
		}
		
		return *it;
	}
	
	
	struct Window_Title : readwrite_property
	{
		static const bool fixed_size = 0;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			result = Find( that ).itsTitle;
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			WindowParameters& params = gWindowParametersMap[ that ];
			
			params.itsTitle = N::Str255( begin, end - begin );
		}
	};
	
	template < class Serialize, typename Serialize::result_type& (*Access)( WindowParameters& ) >
	struct Window_Property : readwrite_property
	{
		static const std::size_t fixed_size = Serialize::fixed_size;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			typedef typename Serialize::result_type result_type;
			
			const result_type& value = Access( Find( that ) );
			
			Serialize::deconstruct::apply( result, value, binary );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			WindowParameters& params = gWindowParametersMap[ that ];
			
			Access( params ) = Serialize::reconstruct::apply( begin, end, binary );
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
	
	using plus::serialize_bool;
	
	typedef plus::serialize_unsigned< N::WindowDefProcID > serialize_ProcID;
	
	
	struct port_property_params
	{
		const property_params  _;
		const bool             is_mutable;
	};
	
	template < class Property, bool variable >
	struct port_property_params_factory
	{
		static const port_property_params value;
	};
	
	template < class Property, bool variable >
	const port_property_params port_property_params_factory< Property, variable >::value =
	{
		{
			Property::fixed_size,
			Property::can_get ? &Property::get : 0,  // NULL
			Property::can_set ? &Property::set : 0   // NULL
		},
		variable
	};
	
	
	class lock_handle : public IOHandle
	{
		private:
			// non-copyable
			lock_handle           ( const lock_handle& );
			lock_handle& operator=( const lock_handle& );
		
		public:
			lock_handle( const vfs::node*  file,
			             int               flags )
			:
				IOHandle( file, flags )
			{
				gWindowParametersMap[ file->owner() ].itIsLocked = true;
			}
			
			~lock_handle();
	};
	
	lock_handle::~lock_handle()
	{
		const vfs::node* port = GetFile()->owner();
		
		gWindowParametersMap[ port ].itIsLocked = false;
		
		remove_window_and_views_from_port( port );
	}
	
	static IOPtr lock_open( const FSTree* that, int flags, mode_t mode )
	{
		if ( gWindowParametersMap[ that->owner() ].itIsLocked )
		{
			if ( flags & O_EXCL )
			{
				p7::throw_errno( EEXIST );
			}
			
			p7::throw_errno( EACCES );
		}
		
		if ( !(flags & O_CREAT) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new lock_handle( that, flags );
	}
	
	static const data_method_set lock_data_methods =
	{
		&lock_open
	};
	
	static const node_method_set lock_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&lock_data_methods
	};
	
	static void unwindow_touch( const FSTree* that )
	{
		CreateUserWindow( that->owner() );
	}
	
	static vfs::filehandle_ptr unwindow_open( const vfs::node* that, int flags, mode_t mode )
	{
		if ( !(flags & O_CREAT) )
		{
			p7::throw_errno( ENOENT );
		}
		
		CreateUserWindow( that->owner() );
		
		return new vfs::filehandle( flags );
	}
	
	static const data_method_set unwindow_data_methods =
	{
		&unwindow_open
	};
	
	static const node_method_set unwindow_methods =
	{
		NULL,
		NULL,
		&unwindow_touch,
		NULL,
		NULL,
		NULL,
		&unwindow_data_methods
	};
	
	static FSTreePtr new_lock( const FSTree*        parent,
	                           const plus::string&  name,
	                           const void*          args )
	{
		const bool exists = port_is_locked( parent );
		
		const mode_t mode = exists ? S_IFREG | 0000
		                           : 0;
		
		return new FSTree( parent, name, mode, &lock_methods );
	}
	
	static FSTreePtr new_window( const FSTree*        parent,
	                             const plus::string&  name,
	                             const void*          args )
	{
		const bool exists = port_has_window( parent );
		
		const mode_t mode = !exists          ? 0
		                  : name.size() == 1 ? S_IFLNK | 0777
		                  :                    S_IFREG | 0600;
		
		const node_method_set& methods = exists ? window_methods
		                                        : unwindow_methods;
		
		return new FSTree( parent, name, mode, &methods );
	}
	
	static FSTreePtr new_focus( const FSTree*        parent,
	                            const plus::string&  name,
	                            const void*          args )
	{
		const bool exists = gWindowParametersMap[ parent ].itsFocus != NULL;
		
		const mode_t mode = exists ? S_IFLNK | 0777
		                           : 0;
		
		const node_method_set* const methods = exists ? &focus_methods
		                                              : &unfocus_methods;
		
		return new FSTree( parent, name, mode, methods );
	}
	
	static FSTreePtr new_gesture( const FSTree*        parent,
	                              const plus::string&  name,
	                              const void*          args )
	{
		const FSTree* view = parent;
		
		const int index = LookupGesture( name );
		
		const bool exists = !gWindowParametersMap[ view ].itsGesturePaths[ index ].empty();
		
		if ( exists )
		{
			return new FSTree( parent, name, S_IFLNK | 0777, &gesture_methods );
		}
		
		return new FSTree( parent, name, 0, &ungesture_methods );
	}
	
	static FSTreePtr new_tty( const FSTree*        parent,
	                          const plus::string&  name,
	                          const void*          args )
	{
		return new FSTree( parent, name, S_IFCHR | 0600, &port_tty_methods );
	}
	
	static FSTreePtr new_port_property( const FSTree*        parent,
	                                    const plus::string&  name,
	                                    const void*          params_ )
	{
		const port_property_params& params = *(const port_property_params*) params_;
		
		const bool has_window = port_has_window( parent );
		
		const bool is_link = has_window  &&  params.is_mutable;
		
		if ( is_link )
		{
			return vfs::new_symbolic_link( parent,
			                               name,
			                               "w/" + name );
		}
		
		return new_property( parent, name, params_ );
	}
	
	#define PROPERTY( var, prop )  &new_port_property, &port_property_params_factory< prop, var >::value
	
	typedef Window_Property< serialize_Point,  &Origin   >  Origin_Property;
	typedef Window_Property< serialize_Point,  &Size     >  Size_Property;
	typedef Window_Property< serialize_bool,   &Visible  >  Visible_Property;
	typedef Window_Property< serialize_ProcID, &ProcID   >  ProcID_Property;
	typedef Window_Property< serialize_bool,   &CloseBox >  CloseBox_Property;
	
	const vfs::fixed_mapping gui_port_ADDR_Mappings[] =
	{
		{ "lock", &new_lock },
		
		{ "window", &new_window },
		{ "w",      &new_window },
		
		{ "view",   &subview_factory, (const void*) static_cast< ViewGetter >( &GetView ) },
		{ "v",      &new_view_dir,                                                        },
		
		{ "focus",  &new_focus },
		
		{ "accept", &new_gesture },
		{ "cancel", &new_gesture },
		
		{ "tty",    &new_tty },
		
		{ "new",    &vfs::new_static_symlink, "../../new" },
		
		{ "title",  PROPERTY( kAttrVariable, Window_Title      ) },
		{ "pos",    PROPERTY( kAttrVariable, Origin_Property   ) },
		{ "size",   PROPERTY( kAttrVariable, Size_Property     ) },
		{ "vis",    PROPERTY( kAttrVariable, Visible_Property  ) },
		{ "procid", PROPERTY( kAttrConstant, ProcID_Property   ) },
		{ "goaway", PROPERTY( kAttrConstant, CloseBox_Property ) },
		
		{ ".~title",  PROPERTY( kAttrVariable, Window_Title      ) },
		{ ".~pos",    PROPERTY( kAttrVariable, Origin_Property   ) },
		{ ".~size",   PROPERTY( kAttrVariable, Size_Property     ) },
		{ ".~vis",    PROPERTY( kAttrVariable, Visible_Property  ) },
		{ ".~procid", PROPERTY( kAttrConstant, ProcID_Property   ) },
		{ ".~goaway", PROPERTY( kAttrConstant, CloseBox_Property ) },
		
		{ NULL, NULL }
	};
	
}

