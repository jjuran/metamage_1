/*
	movement.hh
	-----------
*/

#ifndef CHESS_MOVEMENT_HH
#define CHESS_MOVEMENT_HH

// chess-logic
#include "chess/chess.hh"


namespace chess
{

bool can_move( const Game& game, Square source, Square target );

void move_lowlevel( Game& game, Role role, Square next );

}

#endif
