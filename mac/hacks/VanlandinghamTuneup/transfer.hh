/*
	transfer.hh
	-----------
*/

enum
{
	classic_screen_width  = 512,
	classic_screen_height = 342,
	
	classic_screen_rowBytes    = 64,     // 512 / 8
	classic_screen_buffer_size = 21888,  // 64 * 342
};

typedef unsigned char   Byte;
typedef unsigned short  UInt16;


extern short v_offset;
extern short h_offset;

extern UInt16 h_offset_bytes;
extern UInt16 deep_ScreenRow;

extern short log2_depth;


void set_8px_at_offset( Byte octet : __D0, UInt16 src_byte_index : __D1 );
