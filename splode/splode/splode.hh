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
	
}

#endif
