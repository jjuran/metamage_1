/*
	sound.hh
	--------
*/

#ifndef TICTACTOE_SOUND_HH
#define TICTACTOE_SOUND_HH

extern bool sound_enabled;

extern unsigned short player_tones[];

inline
unsigned short tone_for_player( short player )
{
	// player is 1 for Player X, -1 for player O.
	
	return player_tones[ player < 0 ];
}

#endif
