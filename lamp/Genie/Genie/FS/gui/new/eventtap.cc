/*
	gui/new/eventtap.cc
	-------------------
*/

#include "Genie/FS/gui/new/eventtap.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// POSIX
#include <sys/stat.h>

// Standard C
#include <string.h>

// iota
#include "iota/endian.hh"

// mac-config
#include "mac_config/upp-macros.hh"

// more-libc
#include "more/string.h"

// splode
#include "splode/splode.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/ClickTarget.hh"
#include "Pedestal/View.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle_ptr.hh"
#include "vfs/node.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/fixed_dir.hh"

// relix
#include "relix/api/try_again.hh"

// Genie
#include "Genie/FS/Views.hh"


#ifndef CONFIG_CARBONEVENT_HANDLERS
	#if TARGET_API_MAC_CARBON
		#define CONFIG_CARBONEVENT_HANDLERS  1
	#else
		#define CONFIG_CARBONEVENT_HANDLERS  0
	#endif
#endif

#define LENGTH( array )  (sizeof (array) / sizeof *(array))

static inline bool operator==( const Point& a, const Point& b )
{
	return a.v == b.v  &&  a.h == b.h;
}

static inline bool operator!=( const Point& a, const Point& b )
{
	return !( a == b );
}

namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	static Point last_cursor_location;
	
	
	class ring_buffer
	{
		private:
			static const size_t alloc_size = 1024;
			
			unsigned char its_data[ alloc_size ];
			
			unsigned char* its_head;
			unsigned char* its_tail;
			
			// non-copyable
			ring_buffer           ( const ring_buffer& );
			ring_buffer& operator=( const ring_buffer& );
		
		public:
			ring_buffer()
			:
				its_head( its_data ),
				its_tail( its_data )
			{
			}
			
			bool empty() const  { return its_head == its_tail; }
			
			ssize_t read( char* buffer, size_t n );
			
			void write( const unsigned char* buffer, size_t n );
	};
	
	ssize_t ring_buffer::read( char* buffer, size_t n )
	{
		if ( empty() )
		{
			return 0;
		}
		
		size_t len = its_head[ 0 ];
		
		++len;
		
		if ( its_head + len > its_tail )
		{
			p7::throw_errno( EIO );
		}
		
		if ( n < len )
		{
			p7::throw_errno( EINVAL );
		}
		
		mempcpy( buffer, its_head, len );
		
		its_head += len;
		
		if ( empty() )
		{
			its_head = its_data;
			its_tail = its_data;
		}
		
		return len;
	}
	
	void ring_buffer::write( const unsigned char* buffer, size_t n )
	{
		unsigned char* end = its_data + alloc_size;
		
		const size_t tail_room = end - its_tail;
		
		if ( n > tail_room )
		{
			const size_t head_room = its_head - its_data;
			
			if ( head_room < alloc_size / 2 )
			{
				p7::throw_errno( ENOSPC );
			}
			
			/*
				FIXME:  Assert that the write doesn't exceed what we know
				to be at least half the buffer.
			*/
			
			memmove( its_data, its_head, its_tail - its_head );
			
			its_head -= head_room;
			its_tail -= head_room;
		}
		else
		{
			its_tail = (unsigned char*) mempcpy( its_tail, buffer, n );
		}
	}
	
	struct eventtap_stream_client
	{
		ring_buffer* buffer;
	};
	
	struct eventtap_stream_server : vfs::fixed_dir_extra
	{
		eventtap_stream_client* client;
	};
	
	const size_t eventtap_annex_size = sizeof (eventtap_stream_server)
	                                 - sizeof (vfs::fixed_dir_extra);
	
	
	struct non_ascii_char {};
	
	static const class eventtap_handler* active_eventtap;
	
	class eventtap_handler : public Ped::View
	{
		private:
			const vfs::node* itsKey;
			Rect             itsBounds;
			
		#if CONFIG_CARBONEVENT_HANDLERS
			
			EventHandlerRef  itsMouseMovedEventHandler;
			EventHandlerRef  itsRawKeyEventHandler;
			
		#endif
		
		public:
			eventtap_handler( const vfs::node* key ) : itsKey( key )
			{
				*(uint64_t*) &itsBounds = 0;
				
			#if CONFIG_CARBONEVENT_HANDLERS
				
				itsMouseMovedEventHandler = NULL;
				itsRawKeyEventHandler     = NULL;
				
			#endif
			}
			
		#if CONFIG_CARBONEVENT_HANDLERS
			
			void Install( const Rect& bounds );
			
		#endif
			
			void Uninstall();
			
			eventtap_stream_client* client() const
			{
				eventtap_stream_server& extra = *(eventtap_stream_server*) itsKey->extra();
				
				return extra.client;
			}
			
			eventtap_stream_client* active_client() const
			{
				return active_eventtap ? client() : NULL;
			}
			
			void Idle     ( const EventRecord& event );
			bool MouseDown( const EventRecord& event );
			void MouseUp  ( const EventRecord& event ) { MouseDown( event ); }
			bool KeyDown  ( const EventRecord& event );
			void KeyUp    ( const EventRecord& event ) { KeyDown( event ); }
			
			void Draw( const Rect& bounds, bool erasing );
			
			void DrawInContext( CGContextRef context, CGRect bounds );
			
			void Activate( bool activating );
	};
	
#if CONFIG_CARBONEVENT_HANDLERS
	
	static
	pascal OSStatus eventtap_MouseMoved( EventHandlerCallRef  handler,
	                                     EventRef             event,
	                                     void*                userData )
	{
		ProcessSerialNumber psn = { 0, kCurrentProcess };
		::WakeUpProcess( &psn );
		
		return eventNotHandledErr;
	}
	
	DEFINE_CARBON_UPP( EventHandler, eventtap_MouseMoved )
	
	static EventTypeSpec mouseMoved_event[] =
	{
		{ kEventClassMouse, kEventMouseMoved   },
		{ kEventClassMouse, kEventMouseDragged },
	};
	
	static
	pascal OSStatus eventtap_RawKey( EventHandlerCallRef  handler,
	                                 EventRef             event,
	                                 void*                userData )
	{
		eventtap_handler* that = (eventtap_handler*) userData;
		
		eventtap_stream_client* client = that->active_client();
		
		if ( ! client )
		{
			return eventNotHandledErr;
		}
		
		OSStatus err;
		
		UInt32 modifiers = 0;
		err = GetEventParameter( event,
		                         kEventParamKeyModifiers,
		                         typeUInt32,
		                         NULL,
		                         sizeof (UInt32),
		                         NULL,
		                         &modifiers );
		
		using namespace splode::modes;
		using namespace splode::key;
		
		const uint8_t mode_mask = Command | Shift | Option | Control;
		const uint8_t attr_mask = Alpha;
		
		splode::ascii_event_buffer buffer =
		{
			sizeof buffer - 1,
			'\0',
			(modifiers >> 8) & mode_mask,
			(modifiers >> 8) & attr_mask,
		};
		
		client->buffer->write( &buffer.len, sizeof buffer );
		
		ProcessSerialNumber psn = { 0, kCurrentProcess };
		::WakeUpProcess( &psn );
		
		return noErr;
	}
	
	DEFINE_CARBON_UPP( EventHandler, eventtap_RawKey )
	
	static EventTypeSpec rawKey_event[] =
	{
		{ kEventClassKeyboard, kEventRawKeyModifiersChanged },
	};
	
	void eventtap_handler::Install( const Rect& bounds )
	{
		Ped::View::Install( bounds );
		
		WindowRef window = GetWindowFromPort( GetQDGlobalsThePort() );
		
		OSStatus err;
		
		err = InstallWindowEventHandler( window,
		                                 UPP_ARG( eventtap_MouseMoved ),
		                                 2,
		                                 mouseMoved_event,
		                                 NULL,
		                                 &itsMouseMovedEventHandler );
		
		if ( err )
		{
			itsMouseMovedEventHandler = NULL;
		}
		
		err = InstallApplicationEventHandler( UPP_ARG( eventtap_RawKey ),
		                                      LENGTH( rawKey_event ),
		                                      rawKey_event,
		                                      this,
		                                      &itsRawKeyEventHandler );
		
		if ( err )
		{
			itsRawKeyEventHandler = NULL;
		}
	}
	
#endif
	
	void eventtap_handler::Uninstall()
	{
	#if CONFIG_CARBONEVENT_HANDLERS
		
		OSStatus err;
		
		if ( itsMouseMovedEventHandler )
		{
			err = RemoveEventHandler( itsMouseMovedEventHandler );
		}
		
		if ( itsRawKeyEventHandler )
		{
			err = RemoveEventHandler( itsRawKeyEventHandler );
		}
		
	#endif
		
		if ( active_eventtap == this )
		{
			active_eventtap = NULL;
			
			Ped::set_key_event_tap( false );
		}
		
		Ped::View::Uninstall();
	}
	
	void eventtap_handler::Idle( const EventRecord& event )
	{
		eventtap_stream_server& extra = *(eventtap_stream_server*) itsKey->extra();
		
		if ( event.where == last_cursor_location )
		{
			return;
		}
		
		Point where = event.where;
		
		::GlobalToLocal( &where );
		
		if ( !PtInRect( where, &itsBounds ) )
		{
			return;
		}
		
		last_cursor_location = event.where;
		
		splode::pointer_location_buffer buffer =
		{
			sizeof buffer - 1,
			0,
			iota::big_u16( where.h ),
			iota::big_u16( where.v ),
		};
		
		if ( extra.client )
		{
			extra.client->buffer->write( &buffer.len, sizeof buffer );
		}
	}
	
	bool eventtap_handler::MouseDown( const EventRecord& event )
	{
		using namespace splode::modes;
		namespace pointer = splode::pointer;
		
		eventtap_stream_server& extra = *(eventtap_stream_server*) itsKey->extra();
		
		Idle( event );
		
		const uint8_t mask = Command | Shift | Option | Control;
		
		const uint8_t general_modifiers = (event.modifiers >> 8) & mask;
		const uint8_t button_attributes = event.what;  // Yes, the codes match.
		
		splode::pointer_event_buffer buffer =
		{
			sizeof buffer - 1,
			general_modifiers,
			button_attributes,
			0,  // unspecified device
		};
		
		if ( extra.client )
		{
			extra.client->buffer->write( &buffer.len, sizeof buffer );
		}
		
		Set_ClickTarget( this );
		
		return true;
	}
	
	static
	uint8_t unmodified_char( UInt32 event_message )
	{
		UInt32 state = 0;
		
		Handle kchr = GetResource( 'KCHR', 0 );
		
		if ( kchr == NULL )
		{
			return (uint8_t) event_message;  // Just return the modified char
		}
		
		UInt16 keyCode = (UInt8) (event_message >> 8);
		
		UInt32 key = KeyTranslate( *kchr, keyCode, &state );
		
		return (uint8_t) key;
	}
	
	static inline
	bool is_keypad( UInt16 lower_event_message )
	{
		return (lower_event_message & 0x0000e000) == 0x4000;
	}
	
	bool eventtap_handler::KeyDown( const EventRecord& event )
	{
		using namespace splode::modes;
		using namespace splode::key;
		
		eventtap_stream_server& extra = *(eventtap_stream_server*) itsKey->extra();
		
		const signed char c = unmodified_char( event.message );
		
		if ( c < 0 )
		{
			throw non_ascii_char();
		}
		
		const uint8_t mode_mask = Command | Shift | Option | Control;
		const uint8_t attr_mask = Alpha;
		
		// 3/4/5 -> 1/2/3
		const uint8_t action = event.what - 2;
		const uint8_t keypad = is_keypad( event.message ) ? Keypad : 0;
		
		splode::ascii_event_buffer buffer =
		{
			sizeof buffer - 1,
			c,
			 (event.modifiers >> 8) & mode_mask,
			((event.modifiers >> 8) & attr_mask) | keypad | action,
		};
		
		if ( extra.client )
		{
			extra.client->buffer->write( &buffer.len, sizeof buffer );
		}
		
		return true;
	}
	
	void eventtap_handler::Draw( const Rect& bounds, bool erasing )
	{
		itsBounds = bounds;
		
		View::Draw( bounds, erasing );
	}
	
	void eventtap_handler::DrawInContext( CGContextRef context, CGRect bounds )
	{
		const short top  = (short) bounds.origin.y;
		const short left = (short) bounds.origin.x;
		
		itsBounds.top  = top;
		itsBounds.left = left;
		itsBounds.bottom = top  + (short) bounds.size.height;
		itsBounds.right  = left + (short) bounds.size.width;
		
		View::DrawInContext( context, bounds );
	}
	
	void eventtap_handler::Activate( bool activating )
	{
		active_eventtap = activating ? this : NULL;
		
		Ped::set_key_event_tap( activating );
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		eventtap_stream_server& extra = *(eventtap_stream_server*) delegate->extra();
		
		extra.client = NULL;
		
		return new eventtap_handler( delegate );
	}
	
	
	typedef eventtap_stream_client eventtap_stream_tape_extra;
	
	static unsigned eventtap_stream_poll( vfs::filehandle* that )
	{
		eventtap_stream_client& extra = *(eventtap_stream_client*) that->extra();
		
		return !extra.buffer->empty() ? vfs::Poll_read | vfs::Poll_write : vfs::Poll_write;
	}
	
	static ssize_t eventtap_stream_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		eventtap_stream_client& extra = *(eventtap_stream_client*) that->extra();
		
		while ( extra.buffer->empty() )
		{
			relix::try_again( is_nonblocking( *that ) );
		}
		
		return extra.buffer->read( buffer, n );
	}
	
	static const vfs::stream_method_set eventtap_stream_stream_methods =
	{
		&eventtap_stream_poll,
		&eventtap_stream_read,
	};
	
	static const vfs::filehandle_method_set eventtap_stream_tape_methods =
	{
		NULL,
		NULL,
		&eventtap_stream_stream_methods,
	};
	
	static void destroy_eventtap_stream_tape( vfs::filehandle* that )
	{
		eventtap_stream_client& extra = *(eventtap_stream_client*) that->extra();
		
		((eventtap_stream_server*) get_file( *that )->owner()->extra())->client = NULL;
		
		delete extra.buffer;
	}
	
	static vfs::filehandle_ptr eventtap_stream_open( const vfs::node* that, int flags, mode_t mode )
	{
		vfs::filehandle_ptr result = new vfs::filehandle( that,
		                                                  flags,
		                                                  &eventtap_stream_tape_methods,
		                                                  sizeof (eventtap_stream_tape_extra),
		                                                  &destroy_eventtap_stream_tape );
		
		eventtap_stream_client& extra = *(eventtap_stream_client*) result->extra();
		
		extra.buffer = NULL;
		extra.buffer = new ring_buffer();
		
		((eventtap_stream_server*) that->owner()->extra())->client = &extra;
		
		return result;
	}
	
	static const vfs::data_method_set eventtap_stream_data_methods =
	{
		&eventtap_stream_open
	};
	
	static const vfs::node_method_set eventtap_stream_methods =
	{
		NULL,
		&eventtap_stream_data_methods
	};
	
	
	static vfs::node_ptr eventtap_stream_factory( const vfs::node*     parent,
	                                              const plus::string&  name,
	                                              const void*          args )
	{
		return new vfs::node( parent, name, S_IFCHR | 0400, &eventtap_stream_methods );
	}
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "stream", &eventtap_stream_factory },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr new_eventtap( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     NULL,
		                     eventtap_annex_size );
	}
	
}
