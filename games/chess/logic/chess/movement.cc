/*
	movement.cc
	--------
*/

#include "chess/movement.hh"

// chess-logic
#include "chess/game.hh"


#pragma exceptions off


namespace chess
{

static inline
int abs( int x )
{
	return x < 0 ? -x : x;
}

static inline
bool can_jump_to( const Game& game, Square target )
{
	return ! game.serving( game.board[ target ] );
}

static inline
bool jump_to( Game& game, Square target )
{
	if ( Unit occupying_unit = game.board[ target ] )
	{
		Army& opposed = game.opposed();
		
		Role role = role_of_unit( occupying_unit );
		
		opposed.type[ role ] = Type_none;
		opposed.addr[ role ] = Removed;
	}
	
	return true;
}

static
bool can_move_to( const Game& game, Square prev, Square target, int di )
{
	int next = prev + di;
	
	while ( next != target )
	{
		if ( Unit occupying_unit = game.board[ Square( next ) ] )
		{
			return false;  // can't move through a piece
		}
		
		next += di;
	}
	
	return can_jump_to( game, target );
}

static
bool can_move_pawn( const Game& game, Square source, Square target )
{
	const Board& board = game.board;
	
	const int forward = game.mid_move ? -8 : 8;
	
	int source_row = (source - 1) / 8u;
	int source_col = (source - 1) % 8u;
	
	int target_row = (target - 1) / 8u;
	int target_col = (target - 1) % 8u;
	
	Square next = Square( source + forward );
	
	if ( source_col == target_col )
	{
		if ( board[ target ] )
		{
			return false;  // pawn march (i.e. forward) to occupied square
		}
		
		if ( next == target )
		{
			return true;  // forward one square
		}
		
		int pawn_row = game.mid_move ? 6 : 1;
		
		if ( source_row == pawn_row  &&  ! board[ next ]  &&  next + forward == target )
		{
			return true;  // forward two squares
		}
	}
	else
	{
		bool ok = (source_col > 0  &&  next - 1 == target)  ||
		          (source_col < 7  &&  next + 1 == target);
		
		if ( ! ok )
		{
			return false;  // not a one-square diagonal
		}
		
		if ( target == game.prev_en_passant_square )
		{
			target = Square( target - forward );
		}
		
		Unit occupying_unit = board[ target ];
		
		return game.hostile( occupying_unit );
	}
	
	return false;
}

static inline
bool can_move_knight( int dx, int dy )
{
	return dx  &&  dy  &&  abs( dx ) + abs( dy ) == 3;
}

static
int bishop_move_increment( int dx, int dy )
{
	if ( ! dx  ||  (dx != dy  &&  dx != -dy) )
	{
		return 0;
	}
	
	int step_x = dx < 0 ? -1 : 1;
	int step_y = dy < 0 ? -1 : 1;
	
	return step_y * 8 + step_x;
}

static
int rook_move_increment( int dx, int dy )
{
	if ( ! dx == ! dy )
	{
		return 0;
	}
	
	int step_x = dx < 0 ? -1 : dx > 0 ? 1 : 0;
	int step_y = dy < 0 ? -1 : dy > 0 ? 1 : 0;
	
	return step_y * 8 + step_x;
}

static inline
int queen_move_increment( int dx, int dy )
{
	int result;
	
	(result = rook_move_increment  ( dx, dy))  ||
	(result = bishop_move_increment( dx, dy));
	
	return result;
}

static inline
bool can_move_king( int dx, int dy )
{
	int adx = abs( dx );
	int ady = abs( dy );
	
	return adx < 2  &&  ady < 2  &&  (adx | ady) != 0;
}

bool can_move( const Game& game, Square source, Square target )
{
	const Board& board   = game.board;
	const Army&  current = game.current();
	
	if ( ! source )
	{
		return false;
	}
	
	Unit unit = board[ source ];
	
	if ( ! game.serving( unit )  ||  game.serving( board[ target ] ) )
	{
		return false;
	}
	
	Role role = role_of_unit( unit );
	
	Type type = current.type[ role ];
	
	if ( type == Type_pawn )
	{
		return can_move_pawn( game, source, target );
	}
	
	int source_row = (source - 1) / 8u;
	int source_col = (source - 1) % 8u;
	
	int target_row = (target - 1) / 8u;
	int target_col = (target - 1) % 8u;
	
	int dx = target_col - source_col;
	int dy = target_row - source_row;
	
	switch ( type )
	{
		case Type_knight:  return can_move_knight( dx, dy );
		case Type_king:    return can_move_king  ( dx, dy );
		
		default:
			break;
	}
	
	int di = 0;
	
	switch ( type )
	{
		case Type_bishop:  di = bishop_move_increment( dx, dy );  break;
		case Type_rook:    di = rook_move_increment  ( dx, dy );  break;
		case Type_queen:   di = queen_move_increment ( dx, dy );  break;
		
		default:
			break;
	}
	
	return di  &&  can_move_to( game, source, target, di );
}

void move_lowlevel( Game& game, Role role, Square next )
{
	Board& board   = game.board;
	Army&  current = game.current();
	
	Square prev = current.addr[ role ];
	
	jump_to( game, next );
	
	board[ next ] = board[ prev ];
	board[ prev ] = Empty;
	
	current.addr[ role ] = next;
}

}
