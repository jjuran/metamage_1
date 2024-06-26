/*
	play.hh
	-------
*/

#ifndef CHESS_PLAY_HH
#define CHESS_PLAY_HH

// chess-logic
#include "chess/chess.hh"


namespace chess
{

enum castling_t
{
	Castling_none,
	Castling_kingside,
	Castling_queenside = -1,
};

typedef castling_t Castling;

enum Playable
{
	Play_illegal,
	Play_allowed,
	Play_incomplete = -1,  // pawn promotion requires additional information
};

bool is_threatened( Game& game, Square target );

bool can_castle( Game& game, Castling castling );

bool castle( Game& game, Castling castling );

Playable play( Game& game, Square src, Square dst, Type promotion = Type() );

}

#endif
