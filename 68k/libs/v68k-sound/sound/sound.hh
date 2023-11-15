/*
	sound.hh
	--------
*/

#ifndef SOUND_SOUND_HH
#define SOUND_SOUND_HH

// v68k
#include "v68k/memory.hh"


namespace v68k  {
namespace sound {

using v68k::addr_t;
using v68k::fc_t;
using v68k::mem_t;

enum
{
	buffer_size = 740,
};

extern int sound_fd;

extern uint8_t* the_sound_buffer;

void set_audio_level( short level );

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access );

}  // namespace sound
}  // namespace v68k

#endif
