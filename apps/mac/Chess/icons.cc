/*
	icons.cc
	--------
*/

#include "icons.hh"

// chess-logic
#include "chess/game.hh"


const short icon_ResID_for_type[] =
{
	0,          // none
	128 + 128,  // pawn
	128 + 130,  // knight
	128 + 131,  // bishop
	128 + 129,  // rook
	128 + 132,  // queen
	128 + 133,  // king
	128 + 134,  // king checkmated
	128 + 135,  // king victorious
};
