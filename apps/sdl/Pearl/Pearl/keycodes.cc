/*
	keycodes.cc
	-----------
*/

#include "Pearl/keycodes.hh"

// libsdl2
#include <SDL2/SDL_scancode.h>


#define ____ 0x00
#define F(n) ____


namespace Pearl
{

const unsigned char lookup_from_sdl_scancode[ SDL_NUM_SCANCODES ] =
{
	____,  // SDL_SCANCODE_UNKNOWN

	____,  // unused
	____,  // unused
	____,  // unused

	'a',
	'b',
	'c',
	'd',
	'e',
	'f',
	'g',
	'h',
	'i',
	'j',
	'k',
	'l',
	'm',
	'n',
	'o',
	'p',
	'q',
	'r',
	's',
	't',
	'u',
	'v',
	'w',
	'x',
	'y',
	'z',

	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	
	0x0D,  // Return
	0x1B,  // Escape
	0x08,  // Backspace / Delete
	0x09,  // Tab
	' ',

	'-',
	'=',
	'[',
	']',
	'\\',
	'\\',
	';',
	'\'',
	'`',
	',',
	'.',
	'/',
	
	____,  // Caps Lock

	F(1),
	F(2),
	F(3),
	F(4),
	F(5),
	F(6),
	F(7),
	F(8),
	F(9),
	F(10),
	F(11),
	F(12),
	____,  // Print Screen
	____,  // Scroll Lock
	____,  // Pause

	0x05,  // Help
	0x01,  // Home
	0x0B,  // Page Up
	0x7F,  // FwdDel
	0x04,  // End
	0x0C,  // Page Down
	0x1D,  // Right
	0x1C,  // Left
	0x1F,  // Down
	0x1E,  // Up

	// Keypad

	0x1B,  // Clear
	'/',
	'*',
	'-',
	'+',
	0x03,  // Enter
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'.',

	'`',
	____,  // Application
	____,  // Power
	'=',
	F(13),
	F(14),
	F(15),
	F(16),
	F(17),
	F(18),
	F(19),
	F(20),
	F(21),
	F(22),
	F(23),
	F(24),
	____,  // Execute
	____,  // Help
	____,  // Menu
	____,  // Select
	____,  // Stop
	____,  // Again
	____,  // Undo
	____,  // Cut
	____,  // Copy
	____,  // Paste
	____,  // Find
	____,  // Mute
	____,  // Volume Up
	____,  // Volume Down
	____,  // Locking Caps Lock
	____,  // Locking Num Lock
	____,  // Locking Scroll Lock
	',',
	'=',

	____,
	____,
	____,  // Yen
	____,
	____,
	____,
	____,
	____,
	____,
	____,  // Hangul/English
	____,  // Hanja
	____,  // Katakana
	____,  // Hiragana
	____,  // Zenkaku/Hankaku
	____,
	____,
	____,
	____,

	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,

	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,  // A
	____,  // B
	____,  // C
	____,  // D
	____,  // E
	____,  // F
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,
	____,

	____,  // Left Control
	____,  // Left Shift
	____,  // Left Option
	____,  // Left Command
	____,  // Right Control
	____,  // Right Shift
	____,  // Right Option
	____,  // Right Command

	____,  // Mode

	____,  // Next
	____,  // Previous
	____,  // Stop
	____,  // Play
	____,  // Mute
	____,  // Select
	____,  // WWW
	____,  // Mail
	____,  // Calculator
	____,  // Computer
	____,  // Search
	____,  // Home
	____,  // Back
	____,  // Forward
	____,  // Stop
	____,  // Refresh
	____,  // Bookmarks

	____,  // Brightness Down
	____,  // Brightness Up
	____,  // Display Switch

	____,  // Keyboard Light Toggle
	____,  // Keyboard Brightness Down
	____,  // Keyboard Brightness Up
	____,  // Eject
	____,  // Sleep

	____,
	____,

	____,  // Rewind
	____,  // Fast Forward

	____,
	____,
	____,
	____,
};

}
