/*
	events.cc
	---------
*/

#include "Pearl/events.hh"

// libsdl2
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>

// splode
#include "splode/splode.hh"

// write-a-splode
#include "splode/write-a-splode.hh"

// Pearl
#include "Pearl/cursor.hh"
#include "Pearl/keycodes.hh"


namespace Pearl
{

int events_fd = -1;

uint32_t pearl_event_class;

static float scale;

static bool Q_hit = false;
static bool X_hit = false;

static inline
uint8_t get_modes( SDL_Keymod modifiers )
{
	using namespace splode::modes;
	using namespace splode::key;

	uint8_t modes = 0;

	if ( modifiers & KMOD_SHIFT )
	{
		modes |= Shift;
	}

	if ( modifiers & KMOD_CTRL )
	{
		modes |= Control;
	}

	if ( modifiers & KMOD_ALT )
	{
		modes |= Option;
	}

	if ( modifiers & KMOD_GUI )
	{
		modes |= Command;
	}

	return modes;
}

static inline
bool is_keypad( SDL_Scancode scancode )
{
	return scancode >= SDL_SCANCODE_NUMLOCKCLEAR  &&
	       scancode <= SDL_SCANCODE_KP_PERIOD;
}

static
void send_key_event( const SDL_KeyboardEvent& event, char c )
{
	using namespace splode::modes;
	using namespace splode::key;

	uint8_t modes = get_modes( SDL_GetModState() );
	uint8_t attrs = 0;

	if ( SDL_GetModState() & KMOD_CAPS )
	{
		attrs |= Alpha;
	}

	if ( event.repeat )
	{
		attrs |= held;
	}
	else
	{
		if ( event.state == SDL_PRESSED )
		{
			attrs |= down;
		}
		else
		{
			attrs |= up;
		}
	}

	if ( is_keypad( event.keysym.scancode ) )
	{
		attrs |= Keypad;
	}

	splode::send_key_event( events_fd, c, modes, attrs );
}

static
void clear_chords()
{
	Q_hit = false;
	X_hit = false;
}

static
bool handle_command( const SDL_KeyboardEvent& event )
{
	const SDL_EventType eventClass    = (SDL_EventType) pearl_event_class;
	const Sint32        scaleMultiple = kEventPearlScaleMultiple;
	const Sint32        integerScale  = kEventPearlIntegerScale;
	const Sint32        fullScreen    = kEventPearlFullscreen;
	const Sint32        keyboardGrab  = kEventPearlKeyboardGrab;
	const Sint32        mouseGrab     = kEventPearlMouseGrab;

	switch ( event.keysym.scancode )
	{
		case SDL_SCANCODE_Q:  Q_hit = true;  break;
		case SDL_SCANCODE_X:  X_hit = true;  break;

		case SDL_SCANCODE_EQUALS:
		case SDL_SCANCODE_MINUS:
		{
			SDL_UserEvent scale_multiple = { eventClass };
			scale_multiple.code = scaleMultiple;
			scale = event.keysym.scancode == SDL_SCANCODE_MINUS ? -0.5 : 0.5;
			scale_multiple.data1 = &scale;
			return SDL_PushEvent( (SDL_Event*) &scale_multiple );
		}

		case SDL_SCANCODE_I:
		{
			SDL_UserEvent integer_scale = { eventClass };
			integer_scale.code = integerScale;
			return SDL_PushEvent( (SDL_Event*) &integer_scale );
		}

		case SDL_SCANCODE_RETURN:
		{
			SDL_UserEvent full_screen = { eventClass };
			full_screen.code = fullScreen;
			return SDL_PushEvent( (SDL_Event*) &full_screen );
		}

		case SDL_SCANCODE_K:
		{
			SDL_UserEvent keyboard_grab = { eventClass };
			keyboard_grab.code = keyboardGrab;
			return SDL_PushEvent( (SDL_Event*) &keyboard_grab );
		}

		case SDL_SCANCODE_M:
		{
			SDL_UserEvent mouse_grab = { eventClass };
			mouse_grab.code = mouseGrab;
			return SDL_PushEvent( (SDL_Event*) &mouse_grab );
		}

		default:
			return false;
	}

	if ( Q_hit  &&  X_hit )
	{
		SDL_QuitEvent quit_event = { SDL_QUIT };
		return SDL_PushEvent( (SDL_Event*) &quit_event );
	}

	return true;
}

bool handle_sdl_event( SDL_Event& event, Cursor& cursor )
{
	switch ( event.type )
	{
		case SDL_KEYDOWN:
			if ( (SDL_GetModState() & KMOD_ALT) == KMOD_ALT )
			{
				handle_command( event.key );
				break;
			}
			else
			{
				clear_chords();
			}

			// Fall through to...

		case SDL_KEYUP:
			send_key_event( event.key, lookup_from_sdl_scancode[ event.key.keysym.scancode ] );
			break;

		case SDL_MOUSEMOTION:
		{
			SDL_Point location = { (int) event.motion.x, (int) event.motion.y };
			cursor.send_mouse_moved_event( location );
			break;
		}

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
			SDL_Point location = { (int) event.button.x, (int) event.button.y };
			cursor.send_mouse_moved_event( location );

			if ( event.button.button == 1 )
			{
				cursor.send_mouse_button_event( get_modes( SDL_GetModState() ),
				                                event.button.state == SDL_PRESSED );
			}
			break;
		}

		case SDL_QUIT:
			return false;

		default:
			break;
	}

	return true;
}

}
