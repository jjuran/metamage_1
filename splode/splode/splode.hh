/*
	splode.hh
	---------
*/

#ifndef SPLODE_SPLODE_HH
#define SPLODE_SPLODE_HH

/*
	SPLode implements the Simple Platform-Inclusive Event Lode protocol.
*/

namespace splode
{
	
	typedef unsigned char  uint8_t;
	typedef short          int16_t;
	
namespace ascii
{
	enum
	{
		Delete = 0x08,  // BS
		Tab    = 0x09,  // TAB
		Return = 0x0D,  // CR
		Esc    = 0x1B,  // ESC
		FwdDel = 0x7F,  // DEL
	};
}
	
namespace modes
{
	// General modes
	
	enum
	{
		Command = 1 << 0,
		Shift   = 1 << 1,
		Alt     = 1 << 2,
		Option  = 1 << 3,
		Control = 1 << 4,
		Meta    = 1 << 5,
		Super   = 1 << 6,
		Hyper   = 1 << 7,
	};
}
	
namespace pointer
{
	// Pointer event attributes
	
	enum
	{
		press = 0,  // atomic down/up action
		down  = 1,
		up    = 2,
		
		action_mask = 3,
	};
}
	
	// Type/length 0:  Null message.
	
	struct null_message_buffer
	{
		uint8_t len;
	};
	
	// Type/length 1:  Synthesized ASCII insertion.
	
	struct ascii_synth_buffer
	{
		uint8_t len;
		uint8_t ascii;
	};
	
	// Type/length 4:  Pointer input event.
	
	struct pointer_event_buffer
	{
		uint8_t len;
		uint8_t modes;
		uint8_t attrs;
		uint8_t device;
		uint8_t button;
	};
	
	// Type/length 5:  Pointer location.
	
	struct pointer_location_buffer
	{
		uint8_t len;
		uint8_t device;
		
		// Big-endian
		int16_t x;
		int16_t y;
	};
	
}

#endif
