/*
	state.hh
	--------
*/

#ifndef TICTACTOE_STATE_HH
#define TICTACTOE_STATE_HH

namespace tictactoe
{

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

player_t get( unsigned i );

bool can_move( unsigned i );
bool can_undo();

move_t move( player_t player, unsigned i );

int undo_move();

void reset();

}

#endif
