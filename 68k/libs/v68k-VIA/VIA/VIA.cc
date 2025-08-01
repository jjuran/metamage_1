/*
	VIA.cc
	------
*/

#include "VIA/VIA.hh"

// log-of-war
#include "logofwar/report.hh"

// v68k
#include "v68k/print.hh"

// v68k-screen
#include "screen/screen.hh"

// v68k-sound
#include "sound/sound.hh"


namespace v68k {
namespace VIA  {

/*
	VIA Register A:
	
	+----------+---------+-------+-------+---------+-------+-----------+
	| Bit #    |    7    |   6   |   5   |    4    |   3   |  2  1  0  |
	+----------+---------+-------+-------+---------+-------+-----------+
	|          |   SCC   |  Alt  |  Disk |   ROM   |  Alt  |   Audio   |
	| Function |  wait/  | video |  SEL  | overlay | audio |   level   |
	|          | request |  off  |  line |         |  off  |           |
	+----------+---------+-------+-------+---------+-------+-----------+
	
	VIA Register B:
	
	+----------+-------+-------+---------+-------+---------+-------+---------+
	| Bit #    |   7   |   6   |  5   4  |   3   |    2    |   1   |    0    |
	+----------+-------+-------+---------+-------+---------+-------+---------+
	|          | Sound | Horiz |  Mouse  | Mouse |   RTC   |  RTC  |   RTC   |
	| Function |  gen  | ontal | Y2 | X2 | click |  serial |  data |  serial |
	|          |  off  | blank |         |       |  enable | clock |   data  |
	+----------+-------+-------+---------+-------+---------+-------+---------+
	
	(constructed from descriptions in Inside Macintosh, Volume III)
*/

/*
	VIA Auxiliary Control Register (ACR):
	
	+----------+---------+---------+-----------------+---------+---------+
	| Bit #    |  7   6  |    5    |  4     3     2  |    1    |    0    |
	+----------+---------+---------+-----------------+---------+---------+
	|          |   T1    |   T2    | Shift Register  |   PB    |   PA    |
	| Function | Control | Control |     Control     |  Latch  |  Latch  |
	|          |         |         |                 | Enable  | Enable  |
	+----------+---------+---------+-----------------+---------+---------+
	
	(from the Synertek SY6522 manual)
*/

static uint8_t mmio_byte;

/*
	VIA address space is divided into sixteen 512-byte blocks,
	in each of which just a single byte is valid.
*/

enum
{
	VIA_reg_B,
	VIA_unused,
	VIA_dir_B,
	VIA_dir_A,
	
	VIA_T1_C,
	VIA_T1_CH,
	VIA_T1_L,
	VIA_T1_LH,
	
	VIA_T2_C,
	VIA_T2_CH,
	VIA_SR,
	VIA_ACR,
	
	VIA_PCR,
	VIA_IFR,
	VIA_IER,
	VIA_reg_A,
	
	n_VIA_bytes,
};

static const char* const field_names[] =
{
	"Register B",
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	
	0,
	0,
	0,
	"ACR",
	
	0,
	0,
	0,
	"Register A",
};

const uint16_t readable_VIA_fields = 1 << VIA_reg_B
                                   | 1 << VIA_ACR
                                   | 1 << VIA_reg_A;

const uint16_t writable_VIA_fields = readable_VIA_fields;

static const uint8_t writable_VIA_bits[] =
{
	0x80,  // register B: sound generation flag
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	
	0,
	0,
	0,
	0x4f,  // register A: video/audio page 2 flags, audio level
};

enum
{
	initial_reg_A = 0x4f,  // use main screen and sound buffers, full volume
	initial_reg_B = 0x80,  // sound disabled
	initial_ACR   = 0x00,  // One-shot; PB7, PB, PA, shift register disabled
};

static uint8_t VIA[ n_VIA_bytes ] =
{
	initial_reg_B,
	0,
	0,
	0,
	
	0,
	0,
	0,
	0,
	
	0,
	0,
	0,
	initial_ACR,
	
	0,
	0,
	0,
	initial_reg_A,
};

const uint32_t vBase  = 0xEFE1FE;

/*
	These counters are used to report page flipping without spamming the
	console.  If incremented at 60 Hz, they wrap every 4.25 seconds.
*/

static uint8_t audio_page_flips;
static uint8_t video_page_flips;

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( length != 1 )
	{
		return 0;  // NULL
	}
	
	if ( (addr & 0xffe1ff) != vBase )
	{
		return 0;  // NULL
	}
	
	int index = addr >> 9 & 0xf;
	
	if ( access == v68k::mem_read )
	{
		if ( readable_VIA_fields & (1 << index) )
		{
			return VIA + index;
		}
		
		return 0;  // NULL
	}
	
	if ( access == v68k::mem_write )
	{
		if ( writable_VIA_fields & (1 << index) )
		{
			return &mmio_byte;
		}
		
		return 0;  // NULL
	}
	
	if ( access == v68k::mem_update )
	{
		if ( uint8_t diff = mmio_byte ^ VIA[ index ] )
		{
			if ( diff & ~writable_VIA_bits[ index ] )
			{
				WARNING = "Write to VIA ", field_names[ index ],
					" flips unimplemented bits: ", (v68k::hex32_t) diff;
			}
			
			switch ( index )
			{
				case VIA_reg_A:
					if ( diff & 0x40 )
					{
						v68k::screen::page_flip();
						
						if ( ! video_page_flips++ )
						{
							NOTICE = "Video page flips occuring";
						}
					}
					
					if ( diff & 0x08 )
					{
						// Audio page flip
						
						if ( ! audio_page_flips++ )
						{
							WARNING = "Audio page flips unimplemented";
						}
					}
					
					if ( diff & 0x07 )
					{
						v68k::sound::set_audio_level( mmio_byte & 7 );
					}
					break;
				
				case VIA_reg_B:
					if ( diff & 0x80 )
					{
						const uint8_t sound_off = mmio_byte & 0x80;
						
						const char* endis = sound_off ? "dis" : "en";
						
						NOTICE = "VIA Register B: sound generation ", endis, "abled";
						
						if ( ! sound_off )
						{
							/*
								Automatically send a frame of silence
								when low-level audio generation starts.
								This seems to help prevent audio gaps.
							*/
							
							sound::send_silent_frame();
						}
					}
					break;
				
				default:
					break;
			}
			
			VIA[ index ] = mmio_byte;
		}
	}
	
	return &mmio_byte;
}

}  // namespace VIA
}  // namespace v68k
