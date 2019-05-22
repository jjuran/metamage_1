/*
	rfb.hh
	------
*/

#ifndef RFB_RFB_HH
#define RFB_RFB_HH

// Standard C
#include <stdint.h>

// iota
#include "iota/endian.hh"


#define RFB_PROTOCOL_VERSION_3_3  "RFB 003.003\n"
#define RFB_PROTOCOL_VERSION_3_7  "RFB 003.007\n"
#define RFB_PROTOCOL_VERSION_3_8  "RFB 003.008\n"


namespace rfb
{
	
	typedef iota::big_endian_conversion< int32_t  >::type S_32;
	typedef iota::big_endian_conversion< uint16_t >::type U_16;
	typedef iota::big_endian_conversion< uint32_t >::type U_32;
	
	typedef uint8_t byte;
	
	typedef char ProtocolVersion[ 12 ];
	
	template < class T >
	struct underlying_structure
	{
		typedef void type;
	};
	
	enum auth_type
	{
		Auth_invalid,
		Auth_none,
		Auth_VNC,
	};
	
	template <>
	struct underlying_structure< auth_type >
	{
		typedef byte type;
	};
	
	struct pixel_format
	{
		byte bpp;
		byte depth;
		byte is_big_endian;
		byte is_true_color;
		U_16 red_max;
		U_16 green_max;
		U_16 blue_max;
		byte red_shift;
		byte green_shift;
		byte blue_shift;
		
		byte pad[ 3 ];
	};
	
	struct ClientInit_message
	{
		byte is_shared;
	};
	
	template < int N >
	struct ServerInit_message
	{
		U_16 width;
		U_16 height;
		
		pixel_format format;
		U_32 count;
		char name[ N ];
	};
	
	template <>
	struct ServerInit_message< 0 >
	{
		private:  // For size calculation only
		
		U_16 width;
		U_16 height;
		
		pixel_format format;
		U_32 count;
	};
	
	enum client_to_server_message_type
	{
		SetPixelFormat,
		client_msg_1_undefined,
		SetEncodings,
		FramebufferUpdateRequest,
		KeyEvent,
		PointerEvent,
		ClientCutText,
	};
	
	template <>
	struct underlying_structure< client_to_server_message_type >
	{
		typedef byte type;
	};
	
	struct SetPixelFormat_message
	{
		byte type;
		byte pad[ 3 ];
		
		pixel_format format;
	};
	
	struct SetEncodings_header
	{
		byte type;
		byte pad[ 1 ];
		
		U_16 count;
	};
	
	struct FramebufferUpdateRequest_message
	{
		byte type;
		byte is_incremental;
		U_16 x;
		U_16 y;
		U_16 width;
		U_16 height;
	};
	
	struct KeyEvent_message
	{
		byte type;
		byte is_down;
		byte pad[ 2 ];
		
		U_32 key;
	};
	
	struct PointerEvent_message
	{
		byte type;
		byte button_mask;
		U_16 x;
		U_16 y;
	};
	
	enum server_to_client_message_type
	{
		FramebufferUpdate,
		SetColorMapEntries,
		Bell,
		ServerCutText,
	};
	
	template <>
	struct underlying_structure< server_to_client_message_type >
	{
		typedef byte type;
	};
	
	struct FramebufferUpdate_header
	{
		byte type;
		byte pad[ 1 ];
		
		U_16 count;
	};
	
	struct FramebufferUpdate_element
	{
		U_16 x;
		U_16 y;
		U_16 width;
		U_16 height;
		S_32 encoding;
	};
	
	enum encoding_type
	{
		Encoding_Raw,
	};
	
	template <>
	struct underlying_structure< encoding_type >
	{
		typedef S_32 type;
	};
	
}

#endif
