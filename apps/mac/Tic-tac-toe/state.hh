/*
	state.hh
	--------
*/

#ifndef TICTACTOE_STATE_HH
#define TICTACTOE_STATE_HH

namespace tictactoe
{

/*
	Cell indices:    Coordinates:
	
	 0 | 1 | 2        a1 | b1 | c1
	---+---+---      ----+----+----
	 3 | 4 | 5        a2 | b2 | c2
	---+---+---      ----+----+----
	 6 | 7 | 8        a3 | b3 | c3
*/

typedef unsigned char Code;

inline
Code encode_cell_index( unsigned i )
{
	Code code = 0xa0 + i % 3 * 0x10
	          + 0x01 + i / 3 * 0x01;
	
	return code;
}

inline
unsigned decode_cell_index( Code code )
{
	return ((code & 0xF) - 1) * 3 + (code >> 4) - 0xa;
}

enum player_t
{
	Player_none,
	
	Player_X =  1,
	Player_O = -1,
};

inline
player_t opponent( player_t player )
{
	return player_t( -player );
}

enum move_t
{
	Move_bad = -1,
	Move_ok  =  0,
	Move_won =  1,
};

const unsigned n_squares = 9;

extern signed char squares[ n_squares ];

extern player_t current_player;
extern player_t winning_player;

player_t get( unsigned i );

bool can_move( unsigned i );
bool can_undo();

move_t move( player_t player, unsigned i );

int undo_move();

void reset();

const Code* extract();

bool restore( const Code* data, unsigned short size );

}

#endif
