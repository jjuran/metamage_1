/*
	chess-test.cc
	-------------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

#include <stdio.h>

// more-libc
#include "more/string.h"

// iota
#include "iota/char_types.hh"

// gear
#include "gear/parse_decimal.hh"

// chess-logic
#include "chess/game.hh"
#include "chess/play.hh"
#include "chess/setup.hh"


#pragma exceptions off


#define PRINT( s )  write( STDOUT_FILENO, s "\n", sizeof s )

#define FAIL( s )  if ( true ) { PRINT( s ); _exit( 0 ); } else


using chess::Game;
using chess::Square;
using chess::Type;

typedef unsigned char Byte;

enum
{
	row_gutter = 2,  // final fence post and LF
	cell_width = 4,
	grid_width = cell_width * 8 + row_gutter,
	
	cell_height = 2,
	grid_height = cell_height * 8 + 1,
	
	grid_buffer_size = grid_width * grid_height,
};

static char grid_buffer[ grid_buffer_size ];

static const char h_line[] = "+---+---+---+---+---+---+---+---+\n";
static const char h_rank[] = "|   |   |   |   |   |   |   |   |\n";

static const char white_tokens[] = " ^NBRQK#W";
static const char black_tokens[] = " vnbrqk#W";

static Game game;

static unsigned next_move;

static bool en_passant;

static inline
bool is_rank( Byte c )
{
	return (c -= '1') < 8;
}

static inline
bool is_file( Byte c )
{
	return (c -= 'a') < 8;
}

static inline
Type typeof_piece( char c )
{
	using namespace chess;
	
	switch ( c )
	{
		case 'R':  return Type_rook;
		case 'N':  return Type_knight;
		case 'B':  return Type_bishop;
		case 'Q':  return Type_queen;
		case 'K':  return Type_king;
	}
	
	return Type_none;
}

static
Square find_pawn( const Game& game, int file, Square target )
{
	using namespace chess;
	
	int forward = game.mid_move ? -8 : 8;
	
	Square potential_source = Square( target - forward );
	
	bool occupied = game.board[ target ];
	
	if ( ! file  &&  ! occupied )
	{
		if ( game.type_at( potential_source ) == Type_pawn )
		{
			return potential_source;
		}
		
		potential_source = Square( potential_source - forward );
		
		if ( game.type_at( potential_source ) == Type_pawn )
		{
			return potential_source;
		}
		
		return Square_none;
	}
	
	if ( ! file )
	{
		FAIL( "pre-movement file required for pawn capture" );
	}
	
	return Square( ((potential_source - 1 & ~0x7) | file - 1) + 1 );
}

static
Square find_piece( const Game& game, Type type, int file, Square target )
{
	using namespace chess;
	
	const Army& current = game.current();
	
	Square result = Square_none;
	
	for ( int i = 0;  i < 16;  ++i )
	{
		if ( current.type[ i ] == type )
		{
			Square source = current.addr[ i ];
			
			if ( file  &&  (source ^ file) % 8u != 0 )
			{
				continue;  // file mismatch
			}
			
			Game hypothetical = game;
			
			if ( play( hypothetical, source, target ) )
			{
				if ( result )
				{
					FAIL( "ambiguous source inference" );
				}
				
				result = source;
			}
		}
	}
	
	return result;
}

static
void initialize_empty_grid()
{
	char* p = grid_buffer;
	
	// Draw nine horizontal lines.
	
	int n = 8;
	
	goto start;
	
	while ( n-- > 0 )
	{
		p = (char*) mempcpy( p, h_rank, sizeof h_rank - 1 );
		
	start:
		p = (char*) mempcpy( p, h_line, sizeof h_line - 1 );
	}
}

static
void plot_game_into_grid( const Game& game )
{
	using namespace chess;
	
	char* p = grid_buffer;
	
	p += grid_width + 2;
	
	for ( int rank = 8;  rank > 0;  --rank )
	{
		for ( int file = 1;  file <= 8;  ++file )
		{
			Square square = Square( (rank - 1) * 8 + file );
			
			if ( Unit unit = game.board[ square ] )
			{
				const char* tokens = unit > 0 ? white_tokens
				                              : black_tokens;
				
				const Army& army = game.army[ unit < 0 ];
				
				Role role = role_of_unit( unit );
				
				Type type = army.type[ role ];
				
				*p = tokens[ type ];
			}
			
			p += cell_width;
		}
		
		p += row_gutter + grid_width;
	}
}

static
void display_grid()
{
	static bool init = (initialize_empty_grid(), true);
	
	plot_game_into_grid( game );
	
	write( STDOUT_FILENO, grid_buffer, grid_buffer_size );
}

static
void view_board()
{
	using namespace chess;
	
	char row_buffer[ 8 * 2 ] = "a b c d e f g h";
	
	row_buffer[ 15 ] = '\n';
	
	for ( int rank = 8;  rank > 0;  --rank )
	{
		char* p = row_buffer;
		
		for ( int file = 1;  file <= 8;  ++file )
		{
			Square square = Square( (rank - 1) * 8 + file );
			
			char token = ' ';
			
			if ( Unit unit = game.board[ square ] )
			{
				const char* tokens = unit > 0 ? white_tokens
				                              : black_tokens;
				
				const Army& army = game.army[ unit < 0 ];
				
				Role role = role_of_unit( unit );
				
				Type type = army.type[ role ];
				
				token = tokens[ type ];
			}
			
			*p = token;
			
			p += 2;
		}
		
		write( STDOUT_FILENO, row_buffer, sizeof row_buffer );
	}
}

static
unsigned parse( const char* arg )
{
	using namespace chess;
	
	if ( strcmp( arg, "e.p." ) == 0 )
	{
		if ( ! en_passant )
		{
			FAIL( "false claim of en passant" );
		}
		
		return 0;
	}
	
	const char* p = arg;
	
	char c = *p++;
	
	if ( c == 'v'  &&  ! *p )
	{
		view_board();
		
		return 0;
	}
	
	if ( c == 'V'  &&  ! *p )
	{
		display_grid();
		
		return 0;
	}
	
	if ( iota::is_digit( c ) )
	{
		if ( c == '0' )
		{
			if ( *p++ == '-'  &&  *p++ == '0' )
			{
				if ( *p == '\0' )
				{
					if ( ! can_castle( game, Castling_kingside ) )
					{
						FAIL( "castling forbidden" );
					}
					
					return parse( game.mid_move ? "Kg8" : "Kg1" );
				}
				
				if ( *p++ == '-'  &&  *p++ == '0'  &&  *p == '\0' )
				{
					if ( ! can_castle( game, Castling_queenside ) )
					{
						FAIL( "castling forbidden" );
					}
					
					return parse( game.mid_move ? "Kc8" : "Kc1" );
				}
			}
			
			FAIL( "invalid token" );
		}
		
		if ( game.mid_move )
		{
			FAIL( "move number out of turn" );
		}
		
		unsigned n = gear::parse_unsigned_decimal( &--p );
		
		if ( n != next_move )
		{
			FAIL( "incorrect move number" );
		}
		
		if ( *p++ != '.' )
		{
			--p;
		}
		
		if ( *p )
		{
			FAIL( "garbage after move number" );
		}
		
		return 0;
	}
	
	Type piece_type;
	
	bool capturing = false;
	
	if ( (piece_type = typeof_piece( c )) )
	{
		c = *p++;
		
		if ( (capturing = c == 'x')  ||  c == '/' )
		{
			c = *p++;
		}
	}
	
	int rank = 0;
	int file = 0;
	
	if ( ! is_file( c ) )
	{
		FAIL( "file expected" );
	}
	
	file = c - 'a' + 1;
	
	c = *p++;
	
	if ( is_rank( c ) )
	{
		rank = c - '0';
		
		c = *p++;
	}
	
	if ( ! capturing  &&  ((capturing = c == 'x')  ||  c == '-') )
	{
		c = *p++;
	}
	
	Square source = Square_none;
	
	int source_rank = 0;
	int source_file = 0;
	
	if ( is_file( c ) )
	{
		source_file = file;
		source_rank = rank;
		
		rank = 0;
		
		file = c - 'a' + 1;
		
		c = *p++;
	}
	
	if ( is_rank( c ) )
	{
		rank = c - '0';
		
		c = *p++;
	}
	
	unsigned promotion = 0;
	
	if ( (promotion = typeof_piece( c )) )
	{
		if ( piece_type )
		{
			FAIL( "syntactic promotion of non-pawn" );
		}
		
		if ( c == 'K' )
		{
			FAIL( "promotion to king" );
		}
		
		promotion |= 0x8;
		
		c = *p++;
	}
	
	bool checkmating = c == '#';
	bool checking    = c == '+'  ||  checkmating;
	
	if ( checking )
	{
		c = *p++;
	}
	
	if ( c )
	{
		FAIL( "garbage following" );
	}
	
	if ( ! piece_type  &&  rank == (game.mid_move ? 1 : 8)  &&  ! promotion )
	{
		FAIL( "pawn promotion unspecified" );
	}
	
	Square target = Square( (rank - 1) * 8 + file );
	
	if ( source_file  &&  source_rank )
	{
		source = Square( (source_rank - 1) * 8 + source_file );
	}
	
	if ( ! source )
	{
		if ( piece_type )
		{
			source = find_piece( game, piece_type, source_file, target );
		}
		else if ( rank )
		{
			source = find_pawn( game, source_file, target );
		}
		
		if ( ! source )
		{
			FAIL( "source square not inferred" );
		}
	}
	
	if ( Unit unit = game.board[ source ] )
	{
		if ( (unit < 0) != game.mid_move )
		{
			FAIL( "play of opponent's unit" );
		}
		
		Role role = role_of_unit( unit );
		
		Type type = game.current().type[ role ];
		
		if ( piece_type  &&  type != piece_type )
		{
			FAIL( "unit type mismatch" );
		}
	}
	else
	{
		FAIL( "play from empty square" );
	}
	
	return + source
	       | target      << 16
	       | capturing   << 25
	       | checking    << 26
	       | checkmating << 27
	       | promotion   << 28;
}

static
void do_word( const char* arg )
{
	using namespace chess;
	
	Type promotion = Type();
	
	int data = parse( arg );
	
	en_passant = false;
	
	if ( data == 0 )
	{
		return;
	}
	
	int capture     = data & 0x02000000;
	int checking    = data & 0x04000000;
	int checkmating = data & 0x08000000;
	
	if ( data < 0 )
	{
		promotion = Type( ((unsigned) data >> 28) & 0x7 );
	}
	
	data &= 0x01FFFFFF;
	
	Square source = Square( (short) data );
	Square target = Square( data >> 16 );
	
	en_passant = target == game.prev_en_passant_square;
	
	bool false_capture = capture  &&  ! game.hostile( game.board[ target ] );
	
	bool ok = play( game, source, target, promotion ) > 0;
	
	if ( ! ok )
	{
		FAIL( "move failure" );
	}
	
	if ( false_capture  &&  ! en_passant )
	{
		FAIL( "false claim of capture" );
	}
	
	Square king = game.current().addr[ Role_king ];
	
	if ( checkmating  &&  ! game.finished )
	{
		FAIL( "false claim of checkmate" );
	}
	
	if ( checking  &&  ! is_threatened( game, king ) )
	{
		FAIL( "false claim of check" );
	}
	
	if ( game.finished )
	{
		Type type = game.army[ 0 ].type[ Role_king ];
		
		switch ( type )
		{
			default:
			case Type_king:             PRINT( "stalemate" );  break;
			case Type_king_checkmated:  PRINT( "black win" );  break;
			case Type_king_victorious:  PRINT( "white win" );  break;
		}
	}
	
	if ( ! game.mid_move )
	{
		++next_move;
	}
}

int main( int argc, const char *argv[] )
{
	setup( game );  // new game
	
	next_move = 1;
	
	const char** argp = argv + 1;  // skip argv[ 0 ]
	
	if ( *argp )
	{
		while ( const char* arg = *argp++ )
		{
			do_word( arg );
		}
	}
	else
	{
		char buffer[ 32 ];
		
		char* const end = buffer + sizeof buffer;
		
		ssize_t n_read;
		
		do
		{
			char* p = buffer;
			
			while ( (n_read = read( STDIN_FILENO, p, 1 )) > 0 )
			{
				char c = *p++;
				
				if ( c == ' '  ||  c == '\n' )
				{
					*--p = '\0';
					
					break;
				}
				
				if ( p == end )
				{
					FAIL( "overlong input" );
				}
			}
			
			if ( n_read < 0 )
			{
				FAIL( "read error" );
			}
			
			if ( p != buffer )
			{
				do_word( buffer );
			}
		}
		while ( n_read );
	}
	
	return 0;
}
