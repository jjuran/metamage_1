/*
	game.hh
	-------
*/

#ifndef CHESS_GAME_HH
#define CHESS_GAME_HH

// chess-logic
#include "chess/chess.hh"


namespace chess
{

struct Army
{
	Square addr[ 16 ];
	Type   type[ 16 ];
	
	bool can_castle[ 2 ];
};

struct Board
{
	Unit unit[ 64 ];
	
	Unit& operator[]( Square i )
	{
		return unit[ i - 1 ];
	}
	
	Unit operator[]( Square i ) const
	{
		return unit[ i - 1 ];
	}
};

struct Game
{
	Board board;
	
	Army army[ 2 ];
	
	Square next_en_passant_square;
	Square prev_en_passant_square;
	
	Square promoting_pawn_source;
	Square promoting_pawn_target;
	
	bool mid_move;  // true if Black's turn, false if White's turn
	bool finished;
	
	bool serving( Unit unit ) const
	{
		return unit  &&  (unit < 0) == mid_move;
	}
	
	bool hostile( Unit unit ) const
	{
		return unit  &&  (unit < 0) != mid_move;
	}
	
	Type type_at( Square addr ) const;
	
	Army& current()  { return army[   mid_move ]; }
	Army& opposed()  { return army[ ! mid_move ]; }
	
	const Army& current() const  { return army[   mid_move ]; }
	const Army& opposed() const  { return army[ ! mid_move ]; }
};

}

#endif
