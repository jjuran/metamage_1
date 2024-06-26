/*
	setup.cc
	--------
*/

#include "chess/setup.hh"

// chess-logic
#include "chess/game.hh"


namespace chess
{

static const Role initial_back_rank[ 8 ] =
{
	Role_rook_1,
	Role_knight_1,
	Role_bishop_1,
	Role_queen,
	Role_king,
	Role_bishop_2,
	Role_knight_2,
	Role_rook_2,
};

static const Type initial_piece_types[ 8 ] =
{
	Type_king,
	Type_queen,
	Type_bishop,
	Type_bishop,
	Type_knight,
	Type_knight,
	Type_rook,
	Type_rook,
};

inline
Unit white_unit( Role role )
{
	return Unit( White | role );
}

inline
Unit black_unit( Role role )
{
	return Unit( Black | role );
}

static inline
Role pawn_role( int i )
{
	return Role( Role_pawn + i - 1 );
}

void setup( Game& game )
{
	Unit* p = game.board.unit;
	
	Army& white = game.army[ 0 ];
	Army& black = game.army[ 1 ];
	
	white.addr[ Role_rook_1   ] = Square( 1 );
	white.addr[ Role_knight_1 ] = Square( 2 );
	white.addr[ Role_bishop_1 ] = Square( 3 );
	white.addr[ Role_queen    ] = Square( 4 );
	white.addr[ Role_king     ] = Square( 5 );
	white.addr[ Role_bishop_2 ] = Square( 6 );
	white.addr[ Role_knight_2 ] = Square( 7 );
	white.addr[ Role_rook_2   ] = Square( 8 );
	
	for ( int i = 0;  i < 8;  ++i )
	{
		Role role = initial_back_rank[ i ];
		
		*p++ = white_unit( role );
		
		white.type[ role ] = initial_piece_types[ role ];
	}
	
	for ( int i = 9;  i <= 16;  ++i )
	{
		Role role = pawn_role( i - 8 );
		
		*p++ = white_unit( role );
		
		white.type[ role ] = Type_pawn;
		white.addr[ role ] = Square( i );
	}
	
	for ( int i = 16;  i < 48;  ++i )
	{
		*p++ = Empty;
	}
	
	for ( int i = 9;  i <= 16;  ++i )
	{
		Role role = pawn_role( i - 8 );
		
		*p++ = black_unit( role );
		
		black.type[ role ] = Type_pawn;
		black.addr[ role ] = Square( 40 + i );
	}
	
	for ( int i = 0;  i < 8;  ++i )
	{
		Role role = initial_back_rank[ i ];
		
		*p++ = black_unit( role );
		
		black.type[ role ] = initial_piece_types[ role ];
	}
	
	black.addr[ Role_rook_1   ] = Square( 57 );
	black.addr[ Role_knight_1 ] = Square( 58 );
	black.addr[ Role_bishop_1 ] = Square( 59 );
	black.addr[ Role_queen    ] = Square( 60 );
	black.addr[ Role_king     ] = Square( 61 );
	black.addr[ Role_bishop_2 ] = Square( 62 );
	black.addr[ Role_knight_2 ] = Square( 63 );
	black.addr[ Role_rook_2   ] = Square( 64 );
	
	white.can_castle[ 0 ] = true;
	white.can_castle[ 1 ] = true;
	black.can_castle[ 0 ] = true;
	black.can_castle[ 1 ] = true;
	
	game.next_en_passant_square = Square_none;
	game.prev_en_passant_square = Square_none;
	
	game.promoting_pawn_source = Square_none;
	game.promoting_pawn_target = Square_none;
	
	game.mid_move = false;
	game.finished = false;
}

}
