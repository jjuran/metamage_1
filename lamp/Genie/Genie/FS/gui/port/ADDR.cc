/*
	Genie/FS/gui/port/ADDR.cc
	-------------------------
*/

#include "Genie/FS/gui/port/ADDR.hh"

// Standard C
#include <signal.h>

// POSIX
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

// iota
#include "iota/strings.hh"

// mac-sys-utils
#include "mac_sys/current_process.hh"
#include "mac_sys/is_front_process.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/screenBits.hh"

// mac-app-utils
#include "mac_app/Window_menu.hh"

// gear
#include "gear/hexadecimal.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/serialize.hh"
#include "plus/simple_map.hh"
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
#include "Nitrogen/CFString.hh"
#include "Nitrogen/MacWindows.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/View.hh"
#include "Pedestal/WindowStorage.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/conjoin.hh"
#include "vfs/filehandle/primitives/getpgrp.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/filehandle/primitives/hangup.hh"
#include "vfs/functions/new_static_symlink.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/property_file.hh"
#include "vfs/node/types/symbolic_link.hh"
#include "vfs/primitives/lookup.hh"
#include "vfs/primitives/open.hh"
#include "vfs/primitives/resolve.hh"

// relix
#include "relix/api/root.hh"
#include "relix/fs/terminal.hh"
#include "relix/signal/signal_process_group.hh"

// Genie
#include "Genie/notify.hh"
#include "Genie/FS/focusable_views.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/serialize_qd.hh"
#include "Genie/FS/subview.hh"


#ifndef CONFIG_COMPOSITING
#ifdef MAC_OS_X_VERSION_10_3
#define CONFIG_COMPOSITING  1
#else
#define CONFIG_COMPOSITING  0
#endif
#endif


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
#ifdef __RELIX__
	
	const bool w_dir_is_real = false;
	
#else
	
	const bool w_dir_is_real = true;
	
#endif
	
	static const char* gGestures[] =
	{
		"accept",
		"cancel"
	};
	
	static const int n_gestures = sizeof gGestures / sizeof gGestures[0];
	
	static const Point gZeroPoint = { 0 };
	
	struct WindowParameters
	{
		plus::string        itsTitle;
		Point               itsOrigin;
		Point               itsSize;
		N::WindowDefProcID  itsProcID;
		bool                itIsVisible;
		bool                itHasCloseBox;
		bool                itIsLocked;
		
	#if CONFIG_COMPOSITING
		
		bool                it_is_compositing;
		
	#endif
		
		n::owned< WindowRef >              itsWindow;
		
		boost::intrusive_ptr< Ped::View >  itsSubview;
		
	#ifdef __MACH__
		
		pthread_t           its_pthread;
		
	#endif
		
		const vfs::node* itsFocus;
		
		plus::string itsGesturePaths[ n_gestures ];
		
		vfs::node_ptr     itsTTYDelegate;
		vfs::filehandle*  itsTerminal;
		
		WindowParameters() : itsOrigin( gZeroPoint ),
		                     itsSize  ( gZeroPoint ),
		                     itsProcID( N::documentProc ),
		                     itIsVisible  ( true ),
		                     itHasCloseBox( true ),
		                     itIsLocked(),
		                     
		                 #if CONFIG_COMPOSITING
		                     
		                     it_is_compositing(),
		                     
		                 #endif
		                     
		                     itsSubview( Ped::EmptyView() ),
		                     itsFocus(),
		                     itsTerminal()
		{
		}
	};
	
	typedef plus::simple_map< const vfs::node*, WindowParameters > WindowParametersMap;
	
	static WindowParametersMap gWindowParametersMap;
	
	
	static inline
	bool is_front_process()
	{
		return mac::sys::is_front_process( mac::sys::current_process() );
	}
	
	static void CenterWindowRect( Rect& bounds )
	{
		// Pre-conditions:  bounds is set to { 0, 0, v, h }
		
		const short topMargin = ::GetMBarHeight() + 18;  // FIXME:  Calculate title bar height
		
		const Rect& screenBits_bounds = mac::qd::screenBits().bounds;
		
		short spareWidth = screenBits_bounds.right - bounds.right;
		
		short spareHeight = screenBits_bounds.bottom - bounds.bottom - topMargin;
		
		::OffsetRect( &bounds,
		              spareWidth / 2,
		              topMargin + spareHeight / 3 );
	}
	
	
	static bool Disconnect_Window_Terminal( vfs::filehandle*& h )
	{
		if ( h != NULL )
		{
			hangup( *h );
			
			h = NULL;
			
			return true;
		}
		
		return false;
	}
	
	static
	void Destroy_Window( n::owned< WindowRef >& window, const vfs::node* key )
	{
		if ( window.get() )
		{
			Ped::View* view = Ped::get_window_view( window );
			
			uninstall_view_from_port( *view, key );
			
			mac::app::Window_menu_remove( window );
			
			window.reset();
		}
	}
	
	static void CloseUserWindow( const vfs::node* key )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( key ) )
		{
			WindowParameters& params = *it;
			
		#ifdef __MACH__
			
			int err = notify_thread_via_sighup( params.its_pthread );
			
		#endif
			
			if ( !Disconnect_Window_Terminal( params.itsTerminal ) )
			{
				// tty file is not open for this window, just close the window
				
				Destroy_Window( params.itsWindow, key );
			}
		}
	}
	
	static
	void WindowClosed( WindowRef window )
	{
		const vfs::node* key = (const vfs::node*) GetWRefCon( window );
		
		CloseUserWindow( key );
	}
	
	static
	void WindowResized( WindowRef window )
	{
		const vfs::node* key = (const vfs::node*) GetWRefCon( window );
		
		if ( WindowParameters* it = gWindowParametersMap.find( key ) )
		{
			WindowParameters& params = *it;
			
			if ( params.itsTerminal != NULL )
			{
				const pid_t pgid = getpgrp( *params.itsTerminal );
				
				relix::signal_process_group( SIGWINCH, pgid );
			}
		}
	}
	
	
	static bool port_is_locked( const vfs::node* key )
	{
		return gWindowParametersMap[ key ].itIsLocked;
	}
	
	static bool port_has_window( const vfs::node* key )
	{
		return gWindowParametersMap[ key ].itsWindow.get() != NULL;
	}
	
	static void CreateUserWindow( const vfs::node* key )
	{
		WindowParameters* it = gWindowParametersMap.find( key );
		
		if ( it == NULL )
		{
			p7::throw_errno( EPERM );
		}
		
		WindowParameters& params = *it;
		
	#ifdef __MACH__
		
		params.its_pthread = pthread_self();
		
	#endif
		
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
		
		typedef n::owned< WindowRef > Owner;
		
	#if ! TARGET_API_MAC_CARBON
		
		N::Str255 title( params.itsTitle.data(), params.itsTitle.size() );
		
		Owner owner = Ped::CreateWindow( bounds,
		                                 title,
		                                 params.itIsVisible,
		                                 params.itsProcID,
		                                 params.itHasCloseBox );
		
		WindowRef window = owner;
		
	#else
		
	#if MAC_OS_X_VERSION_10_2
		
		const ProcessSerialNumber psn = { 0, kCurrentProcess };
		
		SetFrontProcessWithOptions( &psn, kSetFrontProcessFrontWindowOnly );
		
	#endif
		
		Mac::WindowClass wClass = Mac::kDocumentWindowClass;
		
		Mac::WindowAttributes attrs = Mac::kWindowNoAttributes;
		
		switch ( params.itsProcID )
		{
			case dBoxProc:
				wClass = Mac::kModalWindowClass;
				break;
			
			case plainDBox:
				wClass = Mac::kPlainWindowClass;
				break;
			
			case altDBoxProc:
				wClass = Mac::kAltPlainWindowClass;
				break;
			
			case movableDBoxProc:
				wClass = Mac::kMovableModalWindowClass;
				break;
			
			default:
				if ( (params.itsProcID & ~0xC) != 0 )
				{
					// TODO:  Add more proc ID translations as needed.
					p7::throw_errno( EINVAL );
				}
				
				attrs = Mac::kWindowCollapseBoxAttribute;
				
				if ( params.itHasCloseBox )
				{
					attrs |= Mac::kWindowCloseBoxAttribute;
				}
				
				if ( (params.itsProcID & 0x4) == 0 )
				{
					attrs |= Mac::kWindowResizableAttribute;
				}
				
				if ( (params.itsProcID & 0x8) != 0 )
				{
					attrs |= Mac::kWindowFullZoomAttribute;
				}
		}
		
	#ifdef MAC_OS_X_VERSION_10_3
		
		attrs |= Mac::kWindowAsyncDragAttribute;
		
	#endif
		
	#if CONFIG_COMPOSITING
		
		if ( params.it_is_compositing )
		{
			attrs |= Mac::kWindowCompositingAttribute;
			
		#ifdef MAC_OS_X_VERSION_10_7
		
			attrs |= Mac::kWindowHighResolutionCapableAttribute;
			
		#endif
		}
		
	#endif
		
		Owner owner = Ped::CreateWindow( wClass, attrs, bounds );
		
		WindowRef window = owner;
		
		const plus::string&            title = params.itsTitle;
		const Carbon::CFStringEncoding utf8  = Carbon::kCFStringEncodingUTF8;
		
		OSStatus err;
		
		err = SetWindowTitleWithCFString( window,
		                                  N::CFStringCreateWithBytes( title,
		                                                              utf8,
		                                                              false ) );
		
		mac::app::Window_menu_remove( window );
		mac::app::Window_menu_insert( window );
		
		if ( params.itIsVisible )
		{
			ShowWindow( window );
			
			Ped::invalidate_window( window );
		}
		
	#endif
		
		N::SetWRefCon( window, key );
		
		Ped::set_window_closed_proc ( window, &WindowClosed  );
		Ped::set_window_resized_proc( window, &WindowResized );
		
		params.itsWindow = owner;
		
		Ped::set_window_view( window, params.itsSubview.get() );
		
		// We could copy from above but the actual bounds could be different
		bounds = mac::qd::get_portRect( window );
		
		params.itsSubview->Install( bounds );
		
		if ( is_front_process() )
		{
			params.itsSubview->Activate( true );
		}
	}
	
	void remove_window_and_views_from_port( const vfs::node* key )
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
	
	void notify_port_of_view_loss( const vfs::node* port_key, const vfs::node* view )
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
		
		Ped::invalidate_window( window );
	}
	
	static WindowRef GetWindowRef( const vfs::node* key )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( key ) )
		{
			return it->itsWindow;
		}
		
		return NULL;
	}
	
	static void FocusViewInWindow( Ped::View& view, const vfs::node* window_key )
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
	
	static void BlurViewInWindow( Ped::View& view, const vfs::node* window_key )
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
	
	bool invalidate_port_WindowRef( const vfs::node* key )
	{
		if ( WindowRef window = GetWindowRef( key ) )
		{
			InvalidateWindowRef( window );
			
			return true;
		}
		
		return false;
	}
	
	void install_view_in_port( Ped::View& view, const vfs::node* key )
	{
		if ( WindowRef window = GetWindowRef( key ) )
		{
			SetWindowPort_Scope scope( window );
			
			Rect bounds = mac::qd::get_portRect( window );
			
			view.Install( bounds );
			
			if ( window == FrontWindow()  &&  is_front_process() )
			{
				view.Activate( true );
			}
			
			InvalidateWindowRef( window );
		}
	}
	
	void uninstall_view_from_port( Ped::View& view, const vfs::node* key )
	{
		if ( WindowRef window = GetWindowRef( key ) )
		{
			SetWindowPort_Scope scope( window );
			
			view.Uninstall();
			
			InvalidateWindowRef( window );
		}
	}
	
	
	static void focus_remove( const vfs::node* that )
	{
		const vfs::node* window = that->owner();
		
		const vfs::node* focus_file = gWindowParametersMap[ window ].itsFocus;
		
		if ( Ped::View* focus_view = get_focusable_view( focus_file ) )
		{
			BlurViewInWindow( *focus_view, window );
		}
		
		gWindowParametersMap[ window ].itsFocus = NULL;
	}
	
	static void unfocus_symlink( const vfs::node*     that,
	                             const plus::string&  target )
	{
		const vfs::node* parent = that->owner();
		
		const vfs::node_ptr targeted_file = lookup( *resolve_pathname( *relix::root(), target, *parent ), plus::string::null );
		
		Ped::View* target_view = get_focusable_view( targeted_file.get() );
		
		if ( target_view == NULL )
		{
			// The target either doesn't exist or isn't a focusable view
			throw p7::errno_t( EINVAL );
		}
		
		const vfs::node* window_key = parent;
		
		FocusViewInWindow( *target_view, window_key );
		
		gWindowParametersMap[ window_key ].itsFocus = targeted_file.get();
	}
	
	static const vfs::link_method_set unfocus_link_methods =
	{
		NULL,
		NULL,
		&unfocus_symlink
	};
	
	static const vfs::node_method_set unfocus_methods =
	{
		NULL,
		NULL,
		&unfocus_link_methods
	};
	
	
	static vfs::node_ptr focus_resolve( const vfs::node* that )
	{
		if ( const vfs::node* focus = gWindowParametersMap[ that->owner() ].itsFocus )
		{
			return focus;
		}
		
		throw p7::errno_t( ENOENT );
	}
	
	static const vfs::item_method_set focus_item_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&focus_remove,
	};
	
	static const vfs::link_method_set focus_link_methods =
	{
		NULL,  // FIXME:  Use relative path
		&focus_resolve
	};
	
	static const vfs::node_method_set focus_methods =
	{
		&focus_item_methods,
		NULL,
		&focus_link_methods
	};
	
	
	const vfs::node* get_port_focus( const vfs::node* port )
	{
		if ( WindowParameters* it = gWindowParametersMap.find( port ) )
		{
			WindowParameters& params = *it;
			
			return params.itsFocus;
		}
		
		return NULL;
	}
	
	void set_port_focus( const vfs::node* port, const vfs::node* focus )
	{
		gWindowParametersMap[ port ].itsFocus = focus;
	}
	
	
	static void window_touch( const vfs::node* that )
	{
		invalidate_port_WindowRef( that->owner() );
	}
	
	static void window_remove( const vfs::node* that )
	{
		CloseUserWindow( that->owner() );
	}
	
	#define SYS_APP_WINDOW_LIST  "/sys/app/window/list/"
	
	static plus::string window_readlink( const vfs::node* that )
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
	
	static const vfs::item_method_set window_item_methods =
	{
		NULL,
		NULL,
		&window_touch,
		NULL,
		&window_remove,
	};
	
	static const vfs::link_method_set window_link_methods =
	{
		&window_readlink
	};
	
	static const vfs::node_method_set window_methods =
	{
		&window_item_methods,
		NULL,
		&window_link_methods
	};
	
	
	static vfs::filehandle_ptr port_tty_open( const vfs::node* that, int flags, mode_t mode );
	
	static void port_tty_attach( const vfs::node* that, const vfs::node* target )
	{
		gWindowParametersMap[ that->owner() ].itsTTYDelegate = target;
	}
	
	static const vfs::data_method_set port_tty_data_methods =
	{
		&port_tty_open
	};
	
	static const vfs::file_method_set port_tty_file_methods =
	{
		&port_tty_attach
	};
	
	static const vfs::node_method_set port_tty_methods =
	{
		NULL,
		&port_tty_data_methods,
		NULL,
		NULL,
		&port_tty_file_methods
	};
	
	
	static void destroy_port_tty( vfs::filehandle* that )
	{
		WindowParameters& params = gWindowParametersMap[ get_file( *that )->owner() ];
		
		params.itsTerminal = NULL;
	}
	
	static vfs::filehandle_ptr port_tty_open( const vfs::node* that, int flags, mode_t mode )
	{
		WindowParameters& params = gWindowParametersMap[ that->owner() ];
		
		vfs::filehandle_ptr tty;
		
		const bool has_tty = params.itsTTYDelegate.get() != NULL;
		
		if ( has_tty )
		{
			tty = open( *params.itsTTYDelegate, flags, 0 );
		}
		else
		{
			tty = new vfs::filehandle( that, 0, NULL, 0, &destroy_port_tty );
		}
		
		vfs::filehandle_ptr terminal = relix::new_terminal( *get_file( *tty ) );
		
		if ( has_tty )
		{
			conjoin( *tty, *terminal );
		}
		
		conjoin( *terminal, *tty );
		
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
	
	
	static void gesture_remove( const vfs::node* that );
	
	static plus::string gesture_readlink( const vfs::node* that );
	
	static const vfs::item_method_set gesture_item_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&gesture_remove,
	};
	
	static const vfs::link_method_set gesture_link_methods =
	{
		&gesture_readlink
	};
	
	static const vfs::node_method_set gesture_methods =
	{
		&gesture_item_methods,
		NULL,
		&gesture_link_methods
	};
	
	static void gesture_remove( const vfs::node* that )
	{
		const vfs::node* view = that->owner();
		
		WindowParameters& params = gWindowParametersMap[ view ];
		
		const int index = LookupGesture( that->name() );
		
		params.itsGesturePaths[ index ].reset();
	}
	
	static void ungesture_symlink( const vfs::node*     that,
	                               const plus::string&  target_path )
	{
		const vfs::node* view = that->owner();
		
		WindowParameters& params = gWindowParametersMap[ view ];
		
		const int index = LookupGesture( that->name() );
		
		params.itsGesturePaths[ index ] = target_path;
	}
	
	static const vfs::link_method_set ungesture_link_methods =
	{
		NULL,
		NULL,
		&ungesture_symlink
	};
	
	static const vfs::node_method_set ungesture_methods =
	{
		NULL,
		NULL,
		&ungesture_link_methods
	};
	
	static plus::string gesture_readlink( const vfs::node* that )
	{
		const vfs::node* view = that->owner();
		
		const int index = LookupGesture( that->name() );
		
		const plus::string& link = gWindowParametersMap[ view ].itsGesturePaths[ index ];
		
		if ( link.empty() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return link;
	}
	
	
	static
	Ped::View* get_view( const vfs::node* key, const plus::string& name )
	{
		return gWindowParametersMap[ key ].itsSubview.get();
	}
	
	static
	void set_view( const vfs::node* key, const plus::string&, Ped::View* view )
	{
		WindowParameters& params = gWindowParametersMap[ key ];
		
		params.itsSubview = view;
		
		if ( WindowRef window = params.itsWindow.get() )
		{
			Ped::set_window_view( window, params.itsSubview.get() );
		}
	}
	
	const View_Accessors access =
	{
		&get_view,
		&set_view,
	};
	
	
	static WindowParameters& Find( const vfs::node* key )
	{
		WindowParameters* it = gWindowParametersMap.find( key );
		
		if ( it == NULL )
		{
			throw vfs::undefined_property();
		}
		
		return *it;
	}
	
	
	static
	void title_get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		result = Find( that ).itsTitle;
		
		/*
			title
			.mac-title
			.~title
			.~mac-title
			
			The ".~" prefix sets the binary flag, so if `binary` is true, we
			need to skip two bytes.  The next byte might be either '.' or 'm'
			for MacRoman titles, but will always be 't' for UTF-8.
		*/
		
		if ( ! TARGET_API_MAC_CARBON == (name[ binary * 2 ] == 't') )
		{
			result = TARGET_API_MAC_CARBON ? plus::mac_from_utf8( result )
			                               : plus::utf8_from_mac( result );
		}
	}
	
	static
	void title_set( const vfs::node* that, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		WindowParameters& params = gWindowParametersMap[ that ];
		
		if ( ! TARGET_API_MAC_CARBON == (name[ binary * 2 ] != 't') )
		{
			params.itsTitle.assign( begin, end - begin );
		}
		else
		{
			const size_t n = end - begin;
			
			params.itsTitle = TARGET_API_MAC_CARBON ? plus::mac_from_utf8( begin, n )
			                                        : plus::utf8_from_mac( begin, n );
		}
	}
	
	#define DEFINE_GETTER( p )  \
	static void p##_get( plus::var_string& result, const vfs::node* that, bool binary )  \
	{  \
		const p##_Property::result_type& value = p( Find( that ) );  \
		p##_Property::deconstruct::apply( result, value, binary );    \
	}
	
	#define DEFINE_SETTER( p )  \
	static void p##_set( const vfs::node* that, const char* begin, const char* end, bool binary )  \
	{  \
		WindowParameters& params = gWindowParametersMap[ that ];               \
		p( params ) = p##_Property::reconstruct::apply( begin, end, binary );  \
	}
	
	
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
		
	#if CONFIG_COMPOSITING
		
		bool& Compositing( WindowParameters& params )
		{
			return params.it_is_compositing;
		}
		
	#endif
		
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
		const vfs::property_params  _;
		const bool                  is_mutable;
	};
	
	
	static void destroy_lock_handle( vfs::filehandle* that )
	{
		const vfs::node* port = get_file( *that )->owner();
		
		gWindowParametersMap[ port ].itIsLocked = false;
		
		remove_window_and_views_from_port( port );
	}
	
	static vfs::filehandle_ptr lock_open( const vfs::node* that, int flags, mode_t mode )
	{
		bool& locked = gWindowParametersMap[ that->owner() ].itIsLocked;
		
		if ( locked )
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
		
		vfs::node_ptr file = new vfs::node( that->owner(),
		                                    that->name(),
		                                    S_IFREG | 0000,
		                                    that->methods() );
		
		vfs::filehandle* result = new vfs::filehandle( file.get(),
		                                               flags,
		                                               NULL,
		                                               0,
		                                               &destroy_lock_handle );
		
		locked = true;
		
		return result;
	}
	
	static const vfs::data_method_set lock_data_methods =
	{
		&lock_open
	};
	
	static const vfs::node_method_set lock_methods =
	{
		NULL,
		&lock_data_methods
	};
	
	static void unwindow_touch( const vfs::node* that )
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
		
		return new vfs::filehandle( NULL, flags );
	}
	
	static const vfs::item_method_set unwindow_item_methods =
	{
		NULL,
		NULL,
		&unwindow_touch,
	};
	
	static const vfs::data_method_set unwindow_data_methods =
	{
		&unwindow_open
	};
	
	static const vfs::node_method_set unwindow_methods =
	{
		&unwindow_item_methods,
		&unwindow_data_methods
	};
	
	static vfs::node_ptr new_lock( const vfs::node*     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		const bool exists = port_is_locked( parent );
		
		const mode_t mode = exists ? S_IFREG | 0000
		                           : 0;
		
		return new vfs::node( parent, name, mode, &lock_methods );
	}
	
	static vfs::node_ptr new_window( const vfs::node*     parent,
	                                 const plus::string&  name,
	                                 const void*          args )
	{
		const bool exists = port_has_window( parent );
		
		const mode_t mode = !exists          ? 0
		                  : name.size() == 1 ? S_IFLNK | 0777
		                  :                    S_IFREG | 0600;
		
		const vfs::node_method_set& methods = exists ? window_methods
		                                             : unwindow_methods;
		
		vfs::node_ptr node( new vfs::node( parent, name, mode, &methods ) );
		
		if ( w_dir_is_real  &&  mode == (S_IFLNK | 0777) )
		{
			return resolve( *relix::root(), *node );
		}
		
		return node;
	}
	
	static vfs::node_ptr new_focus( const vfs::node*     parent,
	                                const plus::string&  name,
	                                const void*          args )
	{
		const bool exists = gWindowParametersMap[ parent ].itsFocus != NULL;
		
		const mode_t mode = exists ? S_IFLNK | 0777
		                           : 0;
		
		const vfs::node_method_set* const methods = exists ? &focus_methods
		                                                   : &unfocus_methods;
		
		return new vfs::node( parent, name, mode, methods );
	}
	
	static vfs::node_ptr new_gesture( const vfs::node*     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		const vfs::node* view = parent;
		
		const int index = LookupGesture( name );
		
		const bool exists = !gWindowParametersMap[ view ].itsGesturePaths[ index ].empty();
		
		if ( exists )
		{
			return new vfs::node( parent, name, S_IFLNK | 0777, &gesture_methods );
		}
		
		return new vfs::node( parent, name, 0, &ungesture_methods );
	}
	
	static vfs::node_ptr new_tty( const vfs::node*     parent,
	                              const plus::string&  name,
	                              const void*          args )
	{
		return new vfs::node( parent, name, S_IFCHR | 0600, &port_tty_methods );
	}
	
	static vfs::node_ptr new_port_property( const vfs::node*     parent,
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
		
		return vfs::new_property( parent, name, params_ );
	}
	
	#define PROPERTY( var, prop )  &new_port_property, &prop##_params
	
	#define VARIABLE( prop )  PROPERTY( kAttrVariable, prop )
	#define CONSTANT( prop )  PROPERTY( kAttrConstant, prop )
	
	typedef serialize_Point   Origin_Property;
	typedef serialize_Point   Size_Property;
	typedef serialize_bool    Visible_Property;
	typedef serialize_ProcID  ProcID_Property;
	typedef serialize_bool    CloseBox_Property;
	typedef serialize_bool    Compositing_Property;
	
	static const port_property_params Window_Title_params =
	{
		{
			vfs::no_fixed_size,
			(vfs::property_get_hook) &title_get,
			(vfs::property_set_hook) &title_set,
		},
		kAttrVariable,
	};
	
	DEFINE_GETTER( Origin      );
	DEFINE_GETTER( Size        );
	DEFINE_GETTER( Visible     );
	DEFINE_GETTER( ProcID      );
	DEFINE_GETTER( CloseBox    );
	
	DEFINE_SETTER( Origin      );
	DEFINE_SETTER( Size        );
	DEFINE_SETTER( Visible     );
	DEFINE_SETTER( ProcID      );
	DEFINE_SETTER( CloseBox    );
	
	#define DEFINE_PARAMS( p, v )  \
	static const port_property_params p##_Property_params = {{p##_Property::fixed_size, &p##_get, &p##_set}, v}
	
	#define DEFINE_VARIABLE_PARAMS( p )  DEFINE_PARAMS( p, kAttrVariable )
	#define DEFINE_CONSTANT_PARAMS( p )  DEFINE_PARAMS( p, kAttrConstant )
	
	DEFINE_VARIABLE_PARAMS( Origin      );
	DEFINE_VARIABLE_PARAMS( Size        );
	DEFINE_VARIABLE_PARAMS( Visible     );
	DEFINE_CONSTANT_PARAMS( ProcID      );
	DEFINE_CONSTANT_PARAMS( CloseBox    );
	
#if CONFIG_COMPOSITING
	
	DEFINE_GETTER( Compositing );
	DEFINE_SETTER( Compositing );
	
	DEFINE_CONSTANT_PARAMS( Compositing );
	
#endif
	
	const vfs::fixed_mapping gui_port_ADDR_Mappings[] =
	{
		{ "lock", &new_lock },
		
		{ "window", &new_window },
		{ "w",      &new_window },
		
		{ "view",   &subview_factory, &access },
		{ "v",      &new_view_dir,            },
		
		{ "focus",  &new_focus },
		
		{ "accept", &new_gesture },
		{ "cancel", &new_gesture },
		
		{ "tty",    &new_tty },
		
		{ "new",    &vfs::new_static_symlink, "../../new" },
		
		{ ".mac-title",  VARIABLE( Window_Title ) },
		{      "title",  VARIABLE( Window_Title ) },
		{ ".~mac-title", VARIABLE( Window_Title ) },
		{     ".~title", VARIABLE( Window_Title ) },
		
		{ "pos",    VARIABLE( Origin_Property   ) },
		{ "size",   VARIABLE( Size_Property     ) },
		{ "vis",    VARIABLE( Visible_Property  ) },
		{ "procid", CONSTANT( ProcID_Property   ) },
		{ "goaway", CONSTANT( CloseBox_Property ) },
		
		{ ".~pos",    VARIABLE( Origin_Property   ) },
		{ ".~size",   VARIABLE( Size_Property     ) },
		{ ".~vis",    VARIABLE( Visible_Property  ) },
		{ ".~procid", CONSTANT( ProcID_Property   ) },
		{ ".~goaway", CONSTANT( CloseBox_Property ) },
		
	#if CONFIG_COMPOSITING
		
		{ "compositing",   CONSTANT( Compositing_Property ) },
		{ ".~compositing", CONSTANT( Compositing_Property ) },
		
	#endif
		
		{ NULL, NULL }
	};
	
}
