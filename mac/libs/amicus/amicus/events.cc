/*
	events.cc
	---------
*/

#include "amicus/events.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// splode
#include "splode/splode.hh"

// amicus
#include "amicus/keycodes.hh"
#include "amicus/splode.hh"


#define LENGTH( array )  (sizeof (array) / sizeof *(array))


namespace amicus
{

CommandMode_state commandmode_state;

int events_fd = -1;

CGPoint cursor_limit;
CGPoint last_cursor_location;

void move_cursor_to( CGPoint location )
{
	CGWarpMouseCursorPosition( location );
	
	CGAssociateMouseAndMouseCursorPosition( true );
}

static
CGPoint pin_cursor( CGPoint next_cursor_location )
{
	CGPoint unpinned_location = next_cursor_location;
	
	if ( next_cursor_location.x >= cursor_limit.x )
	{
		next_cursor_location.x = cursor_limit.x - 1;
	}
	else if ( next_cursor_location.x < 0 )
	{
		next_cursor_location.x = 0;
	}
	
	if ( next_cursor_location.y >= cursor_limit.y )
	{
		next_cursor_location.y = cursor_limit.y - 1;
	}
	else if ( next_cursor_location.y < 0 )
	{
		next_cursor_location.y = 0;
	}
	
#ifndef MAC_OS_X_VERSION_10_5
	
	if ( ! CGPointEqualToPoint( next_cursor_location, unpinned_location ) )
	{
		move_cursor_to( next_cursor_location );
	}
	
#endif
	
	return next_cursor_location;
}

static
void handle_mouse_moved_event( CGPoint next_cursor_location )
{
	CGPoint pinned_cursor = pin_cursor( next_cursor_location );
	
	if ( ! CGPointEqualToPoint( pinned_cursor, last_cursor_location ) )
	{
		send_mouse_moved_event( events_fd, pinned_cursor.x, pinned_cursor.y );
		
		last_cursor_location = pinned_cursor;
	}
}

#ifdef MAC_OS_X_VERSION_10_5

static
void handle_mouse_moved_event( CGEventRef event )
{
	long dx = CGEventGetIntegerValueField( event, kCGMouseEventDeltaX );
	long dy = CGEventGetIntegerValueField( event, kCGMouseEventDeltaY );
	
	CGPoint next = last_cursor_location;
	
	next.x += dx;
	next.y += dy;
	
	handle_mouse_moved_event( next );
}

#endif

static inline
bool any_keys_down()
{
	KeyMap  keymap;
	UInt32* keys = (UInt32*) keymap;
	
	GetKeys( keymap );
	
	/*
		These will be endian-swapped on x86 and ARM,
		but we're only checking for zero so we don't care.
	*/
	
	return keys[ 0 ]  ||  keys[ 1 ]  ||  keys[ 2 ]  ||  keys[ 3 ];
}

static
bool strike_commandmode_state()
{
	switch ( commandmode_state )
	{
		case CommandMode_activated:
			commandmode_state = CommandMode_quasimode;
			break;
		
		case CommandMode_oneshot:
			if ( ! any_keys_down() )
			{
				commandmode_state = CommandMode_off;
			}
			break;
		
		default:
			break;
	}
	
	return true;
}

static
void modify_commandmode_state( uint8_t modes )
{
	using namespace splode::modes;
	
	const uint8_t command_option_control = modes & ~Shift;
	
	switch ( commandmode_state )
	{
		case CommandMode_off:
			if ( command_option_control == (Command | Option | Control) )
			{
				// Command, Option, Control all down
				commandmode_state = CommandMode_activated;
			}
			break;
		
		case CommandMode_oneshot:
			break;
		
		case CommandMode_quasimode:
			if ( command_option_control == 0 )
			{
				// Command, Option, Control all up
				commandmode_state = CommandMode_off;
			}
			break;
		
		case CommandMode_activated:
			if ( command_option_control == 0 )
			{
				// Command, Option, Control all up
				commandmode_state = CommandMode_oneshot;
			}
			break;
	}
}

static inline
bool is_mouse_moved_event( const EventRecord& event )
{
	const uint32_t target_message = mouseMovedMessage << 24;
	const uint32_t masked_message = event.message & osEvtMessageMask;
	
	return event.what == osEvt  &&  masked_message == target_message;
}

long send_key_event( EventRef event, char c, uint8_t more_attrs )
{
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
	
	const uint8_t modes = (modifiers >> 8) & mode_mask;
	const uint8_t attrs = (modifiers >> 8) & attr_mask;
	
	modify_commandmode_state( modes );
	
	send_key_event( events_fd, c, modes, attrs | more_attrs );
	
	return noErr;
}

#ifdef MAC_OS_X_VERSION_10_5

bool handle_CGEvent( CGEventRef event, command_handler_proc command_handler )
{
	CGEventType type = CGEventGetType( event );
	
	const CGEventFlags mask_Option_and_Shift = kCGEventFlagMaskAlternate
	                                         | kCGEventFlagMaskShift;
	
	CGEventFlags flags = CGEventGetFlags( event );
	
	const uint8_t modes = (flags & kCGEventFlagMaskCommand) >> 20
	                    | (flags & mask_Option_and_Shift  ) >> 16
	                    | (flags & kCGEventFlagMaskControl) >> 14;
	
	switch ( type )
	{
		case kCGEventLeftMouseDown:
		case kCGEventLeftMouseUp:
		case kCGEventRightMouseDown:
		case kCGEventRightMouseUp:
		case kCGEventMouseMoved:
		case kCGEventLeftMouseDragged:
		case kCGEventRightMouseDragged:
			handle_mouse_moved_event( event );
			break;
		
		default:
			break;
	}
	
	switch ( type )
	{
		case kCGEventLeftMouseDown:
		case kCGEventLeftMouseUp:
			send_mouse_event( events_fd, type, modes );
			return true;
		
		case kCGEventRightMouseDown:
		case kCGEventRightMouseUp:
			return false;
		
		case kCGEventMouseMoved:
		case kCGEventLeftMouseDragged:
		case kCGEventRightMouseDragged:
			return true;
		
		default:
			break;
	}
	
	uint8_t attrs = (flags & kCGEventFlagMaskAlphaShift) >> 14;
	
	bool repeat;
	repeat = CGEventGetIntegerValueField( event, kCGKeyboardEventAutorepeat );
	
	int8_t key = CGEventGetIntegerValueField( event, kCGKeyboardEventKeycode );
	
	uint8_t c = lookup_from_virtual[ key & 0x7F ];
	
	if ( commandmode_state )
	{
		switch ( type )
		{
			case kCGEventKeyDown:
				if ( ! repeat  &&  command_handler )
				{
					command_handler( c );
				}
				
				return true;
			
			case kCGEventKeyUp:
				return strike_commandmode_state();
			
			default:
				break;
		}
	}
	
	switch ( type )
	{
		case kCGEventKeyDown:
			if ( repeat )
			{
				attrs += 2;  // augment from key-down to auto-key
			}
			// fall through
		case kCGEventKeyUp:
			attrs += type - (kCGEventKeyDown - 1);  // splode::key::down
			
			if ( is_keypad( key ) )
			{
				attrs |= splode::key::Keypad;
			}
			
			// fall through
		case kCGEventFlagsChanged:
			send_key_event( events_fd, c, modes, attrs );
			
			modify_commandmode_state( modes );
			return true;
		
		default:
			break;
	}
	
	return false;
}

#endif  // #ifdef MAC_OS_X_VERSION_10_5

bool handle_EventRecord( const EventRecord&    event,
                         command_handler_proc  command_handler )
{
	Point where = event.where;
	
	switch ( event.what )
	{
		case osEvt:
			if ( ! is_mouse_moved_event( event ) )
			{
				break;
			}
			// fall through
		case mouseDown:
		case mouseUp:
			handle_mouse_moved_event( CGPointMake( where.h, where.v ) );
			break;
	}
	
	if ( commandmode_state )
	{
		switch ( event.what )
		{
			case keyDown:
				if ( command_handler )
				{
					/*
						We can't just grab the low byte of event.message
						as the char, because the user might be holding the
						Option key, which would modify the character.
					*/
					
					int8_t key = event.message >> 8;
					
					uint8_t c = lookup_from_virtual[ key & 0x7F ];
					
					command_handler( c );
				}
				
				// fall through
			case autoKey:
				return true;
			
			case keyUp:
				return strike_commandmode_state();
			
			default:
				break;
		}
	}
	
	switch ( event.what )
	{
		case mouseDown:
		case mouseUp:
			send_mouse_event( events_fd, event );
			break;
		
		case keyDown:
		case keyUp:
		case autoKey:
			send_key_event( events_fd, event );
			break;
		
		case kHighLevelEvent:
			(void) AEProcessAppleEvent( &event );
			break;
		
		default:
			break;
	}
	
	return true;
}

}
