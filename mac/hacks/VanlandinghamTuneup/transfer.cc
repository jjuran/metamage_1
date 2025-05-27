/*
	transfer.cc
	-----------
*/

#include "transfer.hh"


typedef signed   char  SInt8;
typedef unsigned char  UInt8;
typedef unsigned long  UInt32;

Byte* ScrnBase : 0x0824;

short v_offset;
short h_offset;

UInt16 h_offset_bytes;
UInt16 deep_ScreenRow;

short log2_depth;


static const UInt8 deepen_2bpp[] =
{
	0x00, 0x03, 0x0C, 0x0F,
	0x30, 0x33, 0x3C, 0x3F,
	0xC0, 0xC3, 0xCC, 0xCF,
	0xF0, 0xF3, 0xFC, 0xFF,
};

static const UInt16 deepen_4bpp[] =
{
	0x0000, 0x000F, 0x00F0, 0x00FF,
	0x0F00, 0x0F0F, 0x0FF0, 0x0FFF,
	0xF000, 0xF00F, 0xF0F0, 0xF0FF,
	0xFF00, 0xFF0F, 0xFFF0, 0xFFFF,
};

static const UInt32 deepen_8bpp[] =
{
	0x00000000, 0x000000FF, 0x0000FF00, 0x0000FFFF,
	0x00FF0000, 0x00FF00FF, 0x00FFFF00, 0x00FFFFFF,
	0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
	0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF,
};

void set_8px_at_offset( Byte octet : __D0, UInt16 src_byte_index : __D1 )
{
	if ( src_byte_index >= classic_screen_buffer_size )
	{
		return;
	}
	
	UInt16 x = src_byte_index & 0x3f;
	UInt16 y = src_byte_index >> 6;
	
	x += h_offset_bytes;
	y += v_offset;
	
	Byte* row_addr = ScrnBase + deep_ScreenRow * y;
	
	x <<= log2_depth;
	
	Byte* dst = row_addr + x;
	
	UInt8*  p8  = (UInt8 *) dst;
	UInt16* p16 = (UInt16*) dst;
	UInt32* p32 = (UInt32*) dst;
	
	switch ( log2_depth )
	{
		case 0:
			*dst = octet;
			break;
		
		case 1:
			*p8++ = deepen_2bpp[ octet >> 4  ];
			*p8++ = deepen_2bpp[ octet & 0xF ];
			break;
		
		case 2:
			*p16++ = deepen_4bpp[ octet >> 4  ];
			*p16++ = deepen_4bpp[ octet & 0xF ];
			break;
		
		case 3:
			*p32++ = deepen_8bpp[ octet >> 4  ];
			*p32++ = deepen_8bpp[ octet & 0xF ];
			break;
		
		case 4:
			for ( int i = 0;  i < 8;  ++i )
			{
				*p16++ = (SInt8) octet < 0 ? 0x0000 : 0x7FFF;
				
				octet <<= 1;
			}
			break;
		
		case 5:
			for ( int i = 0;  i < 8;  ++i )
			{
				*p32++ = (SInt8) octet < 0 ? 0x00000000 : 0x00FFFFFF;
				
				octet <<= 1;
			}
			break;
		
		default:
			break;
	}
}
