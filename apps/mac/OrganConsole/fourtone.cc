/*
	fourtone.cc
	-----------
*/

#include "fourtone.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// Organ Console
#include "soundgen.hh"


#pragma exceptions off


enum
{
	n_channels = 4,
};

typedef int Key_Num;  // one-based; a virtual keycode + 1, 1 .. 64
typedef int Channel;  // zero-based channel index, 0 .. 3

struct fourtone_state
{
	int n_keys_down;
	
	Key_Num channel_keys[ n_channels ];
};

static fourtone_state state;


static inline
void clear( fourtone_state& state )
{
	int* p = &state.n_keys_down;
	
	int n = sizeof state / sizeof (int);
	
	while ( n-- > 0 )
	{
		*p++ = 0;
	}
}

static
Channel available_channel()
{
	return   ! state.channel_keys[ 0 ] ? 0
	       : ! state.channel_keys[ 1 ] ? 1
	       : ! state.channel_keys[ 2 ] ? 2
	       : ! state.channel_keys[ 3 ] ? 3
	       :                            -1;
}

void fourtone_acquire_key_note( unsigned char keycode, FTRate rate )
{
	Channel channel = available_channel();
	
	if ( channel >= 0 )
	{
		if ( ! state.n_keys_down )
		{
			fourtone_start( rate );
		}
		else
		{
			fourtone_amend_rate( channel, rate );
			fourtone_amend_submit();
		}
		
		state.channel_keys[ channel ] = keycode + 1;
		
		++state.n_keys_down;
	}
}

static inline
void fourtone_release_channel( Channel channel )
{
	state.channel_keys[ channel ] = 0;
	
	fourtone_amend_rate( channel, 0 );
	fourtone_amend_submit();
}

void fourtone_release_not_down()
{
	KeyMap keymap;
	
	GetKeys( keymap );
	
	UInt8* keys = (UInt8*) keymap;
	
	for ( Channel channel = 0;  channel < n_channels;  ++channel )
	{
		if ( Key_Num key = state.channel_keys[ channel ] )
		{
			int keycode = key - 1;
			
			int i = keycode / 8u;
			int j = keycode % 8u;
			
			Boolean down = keys[ i ] & 1u << j;
			
			if ( ! down )
			{
				fourtone_release_channel( channel );
				
				if ( ! --state.n_keys_down )
				{
					full_stop();
					
					clear( state );
					break;
				}
			}
		}
	}
}
