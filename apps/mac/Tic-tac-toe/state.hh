/*
	state.hh
	--------
*/

#ifndef TICTACTOE_STATE_HH
#define TICTACTOE_STATE_HH

// tictactoe-logic
#include "tictactoe.hh"


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

enum move_t
{
	Move_bad = -1,
	Move_ok  =  0,
	Move_won =  1,
};

extern signed char squares[ n_squares ];

extern player_t current_player;
extern player_t winning_player;

player_t get( unsigned i );

bool can_move( unsigned i );
bool can_undo();

move_t move( unsigned i );

int undo_move();

void reset();

const Code* extract();

bool restore( const Code* data, unsigned short size );

}

#endif
