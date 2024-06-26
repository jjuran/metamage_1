/*
	play.cc
	-------
*/

#include "chess/play.hh"

// chess-logic
#include "chess/game.hh"
#include "chess/movement.hh"


#pragma exceptions off


namespace chess
{

static inline
bool threatens( const Game& game, Role src_role, Square target )
{
	return can_move( game, game.current().addr[ src_role ], target );
}

bool is_threatened( Game& game, Square target )
{
	game.mid_move = ! game.mid_move;
	
	bool threatened = false;
	
	for ( int role = 0;  role < 16;  ++role )
	{
		if ( threatens( game, Role( role ), target ) )
		{
			threatened = true;
			break;
		}
	}
	
	game.mid_move = ! game.mid_move;
	
	return threatened;
}

bool can_castle( Game& game, Castling castling )
{
	Board& board   = game.board;
	Army&  current = game.current();
	
	if ( ! current.can_castle[ castling > 0 ] )
	{
		return false;
	}
	
	Role rook_role = Role( Role_rook + (castling > 0) );
	
	Square king_square = current.addr[ Role_king ];
	Square rook_square = current.addr[ rook_role ];
	
	Square next = Square( king_square + castling );
	
	if ( ! can_move( game, rook_square, next ) )
	{
		return false;  // path is obstructed
	}
	
	if ( is_threatened( game, king_square ) )
	{
		return false;  // can't castle out of check
	}
	
	Square prev = king_square;
	
	// Move the king, temporarily.
	
	board[ next ] = board[ prev ];
	board[ prev ] = Empty;
	
	current.addr[ Role_king ] = next;
	
	bool ok = ! is_threatened( game, next );  // ... or through check
	
	if ( ok )
	{
		Square last = Square( next + castling );
		
		board[ last ] = board[ next ];
		board[ next ] = Empty;
		
		current.addr[ Role_king ] = last;
		
		ok = ! is_threatened( game, last );  // ... or into check
		
		next = last;
	}
	
	// Move the king back.
	
	board[ prev ] = board[ next ];
	board[ next ] = Empty;
	
	current.addr[ Role_king ] = prev;
	
	return ok;
}

static
bool move_pawn( Game& game, Square source, Square target, Type promotion )
{
	if ( can_move( game, source, target ) )
	{
		const int forward = game.mid_move ? -8 : 8;
		
		Square en_passant = Square_none;
		
		if ( source + forward * 2 == target )
		{
			en_passant = Square( source + forward );
		}
		
		game.next_en_passant_square = en_passant;
		
		Role role = role_of_unit( game.board[ source ] );
		
		if ( target == game.prev_en_passant_square )
		{
			Square destination = Square( target - forward );
			
			move_lowlevel( game, role, destination );
		}
		
		move_lowlevel( game, role, target );
		
		if ( target <= 8  ||  target > 56 )
		{
			if ( promotion < Type_knight  ||  promotion > Type_queen )
			{
				return false;
			}
			
			game.current().type[ role ] = promotion;
		}
		
		return true;
	}
	
	return false;
}

static
bool move_piece( Game& game, Square source, Square target )
{
	if ( can_move( game, source, target ) )
	{
		Army& current = game.current();
		
		Role role = role_of_unit( game.board[ source ] );
		
		if ( role == Role_king )
		{
			current.can_castle[ 0 ] = false;
			current.can_castle[ 1 ] = false;
		}
		else if ( (role & ~0x1) == Role_rook )
		{
			current.can_castle[ role & 0x1 ] = false;
		}
		
		move_lowlevel( game, role, target );
		
		return true;
	}
	
	return false;
}

bool castle( Game& game, Castling castling )
{
	if ( ! can_castle( game, castling ) )
	{
		return false;
	}
	
	Army& current = game.current();
	
	Role rook_role = Role( Role_rook + (castling > 0) );
	
	int next = current.addr[ Role_king ];
	
	move_lowlevel( game, rook_role, Square( next += castling ) );
	move_lowlevel( game, Role_king, Square( next += castling ) );
	
	current.can_castle[ 0 ] = false;
	current.can_castle[ 1 ] = false;
	
	return true;
}

static
Castling is_castling( const Game& game, Square source, Square target )
{
	const Army& current = game.current();
	
	int delta = target - source;
	
	if ( Unit unit = game.board[ source ] )
	{
		if ( role_of_unit( unit ) == Role_king )
		{
			if ( current.can_castle[ 0 ]  &&  delta == -2 )
			{
				return Castling_queenside;
			}
			
			if ( current.can_castle[ 1 ]  &&  delta == 2 )
			{
				return Castling_kingside;
			}
		}
	}
	
	return Castling_none;
}

static
bool legal_move( Game& game, Square source, Square target, Type promotion )
{
	if ( ! can_move( game, source, target ) )
	{
		return false;
	}
	
	const Army& current = game.current();
	
	Unit unit = game.board[ source ];
	Role role = role_of_unit( unit );
	Type type = current.type[ role ];
	
	Game backup = game;
	
	bool ok = false;
	
	if ( type == Type_pawn )
	{
		ok = move_pawn( game, source, target, promotion );
	}
	else
	{
		ok = move_piece( game, source, target );
	}
	
	if ( ok  &&  is_threatened( game, current.addr[ Role_king ] ) )
	{
		game = backup;
		
		ok = false;
	}
	
	return ok;
}

static const int white_pawn_move_deltas[] = {  7,  8,  9,  16 };
static const int black_pawn_move_deltas[] = { -7, -8, -9, -16 };

static const int knight_move_deltas[] = { -17, -10, 6, 15, 17, 10, -6, -15 };

static const int king_move_deltas[] = { -8, -9, -1, 7, 8, 9, 1, -7 };

static
bool is_legal_move( const Game& game, Square source, Square target )
{
	if ( (unsigned) target - 1 < 64  &&  can_move( game, source, target ) )
	{
		Game hypothetical = game;
		
		if ( legal_move( hypothetical, source, target, Type_queen ) )
		{
			return true;
		}
	}
	
	return false;
}

static
bool is_legal_move_within( const Game& game, Square source, int d, int incr )
{
	for ( int target = source - d;  target <= source + d;  target += incr )
	{
		if ( is_legal_move( game, source, Square( target ) ) )
		{
			return true;
		}
	}
	
	return false;
}

static
bool legal_move_exists( const Game& game )
{
	const int* pawn_deltas = game.mid_move ? black_pawn_move_deltas
	                                       : white_pawn_move_deltas;
	
	for ( int role = 15;  role >= 0;  --role )
	{
		Square source = game.current().addr[ role ];
		
		Type type = game.current().type[ role ];
		
		const int* deltas;
		
		unsigned n_deltas = 0;
		
		switch ( type )
		{
			case Type_none:
				continue;
			
			case Type_pawn:
				deltas = pawn_deltas;
				
				n_deltas = 4;
				break;
			
			case Type_knight:
				deltas = knight_move_deltas;
				
				n_deltas = 8;
				break;
			
			case Type_king:
				deltas = king_move_deltas;
				
				n_deltas = 8;
				break;
			
			default:
				break;
		}
		
		if ( n_deltas )
		{
			for ( int i = 0;  i < n_deltas;  ++i )
			{
				Square target = Square( source + deltas[ i ] );
				
				if ( is_legal_move( game, source, target ) )
				{
					return true;
				}
			}
			
			continue;
		}
		
		if ( type != Type_bishop )
		{
			if ( is_legal_move_within( game, source, 7, 1 ) )
			{
				return true;
			}
			
			if ( is_legal_move_within( game, source, 56, 8 ) )
			{
				return true;
			}
		}
		
		if ( type != Type_rook )
		{
			if ( is_legal_move_within( game, source, 63, 9 ) )
			{
				return true;
			}
			
			if ( is_legal_move_within( game, source, 49, 7 ) )
			{
				return true;
			}
		}
	}
	
	return false;
}

static inline
bool is_promotion( const Game& game, Square source, Square target )
{
	return (target <= 8  ||  target > 56)       &&
	       game.type_at( source ) == Type_pawn  &&
	       is_legal_move( game, source, target );
}

Playable play( Game& game, Square src, Square dst, Type promotion )
{
	if ( game.finished )
	{
		return Play_illegal;
	}
	
	if ( is_promotion( game, src, dst )  &&  ! promotion )
	{
		return Play_incomplete;
	}
	
	bool ok;
	
	if ( Castling castling = is_castling( game, src, dst ) )
	{
		ok = castle( game, castling );
	}
	else
	{
		ok = legal_move( game, src, dst, promotion );
	}
	
	if ( ! ok )
	{
		return Play_illegal;
	}
	
	game.prev_en_passant_square = game.next_en_passant_square;
	game.next_en_passant_square = Square_none;
	
	game.mid_move = ! game.mid_move;
	
	if ( ! legal_move_exists( game ) )
	{
		game.finished = true;
		
		Army& current = game.current();
		Army& opposed = game.opposed();
		
		if ( is_threatened( game, current.addr[ Role_king ] ) )
		{
			current.type[ Role_king ] = Type_king_checkmated;
			opposed.type[ Role_king ] = Type_king_victorious;
		}
	}
	
	return Play_allowed;
}

}
