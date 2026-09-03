/*
	state.hh
	--------
*/

#ifndef STATE_HH
#define STATE_HH


typedef char*   Ptr;
typedef char**  Handle;

typedef unsigned char Boolean;

typedef struct SndChannel* SndChannelPtr;

Boolean SoundActive : 0x027e;

struct PerAppState
{
	SndChannelPtr  channel;
	Handle         buffer;
};

inline
PerAppState& get_state( Handle code_1 )
{
	Ptr p = *code_1 + 4;
	
	return *(PerAppState*) p;
}

#endif
