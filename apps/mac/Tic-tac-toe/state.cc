/*
	state.cc
	--------
*/

#include "state.hh"


#define HORIZONTAL( i )  { (i) * 3 + 0, (i) * 3 + 1, (i) * 3 + 2 }
#define VERTICAL( i )    { (i) + 3 * 0, (i) + 3 * 1, (i) + 3 * 2 }

namespace tictactoe
{

player_t current_player = tictactoe::Player_X;
player_t winning_player = tictactoe::Player_none;

struct Ledger
{
	Code entries[ 1 + n_squares ];  // includes a length byte
	
	unsigned char size() const  { return entries[ 0 ]; }
	
	void reset( const Code* data = 0, unsigned short size = 0 );
	
	void enter( Code entry );
	
	Code pop();
};

void Ledger::reset( const Code* data, unsigned short size )
{
	unsigned char& count = entries[ 0 ];
	
	while ( count > size )
	{
		entries[ count-- ] = 0;
	}
	
	count = 0;
	
	while ( count < size )
	{
		entries[ ++count ] = *data++;
	}
}

void Ledger::enter( Code entry )
{
	unsigned char& count = entries[ 0 ];
	
	if ( count >= n_squares )
	{
		return;
	}
	
	entries[ ++count ] = entry;
}

Code Ledger::pop()
{
	unsigned char& count = entries[ 0 ];
	
	if ( count == 0 )
	{
		return 0;
	}
	
	Code last = entries[ count ];
	
	entries[ count-- ] = 0;
	
	return last;
}

static Ledger ledger;

signed char squares[ n_squares ];

static const short wins[][ 3 ] =
{
	HORIZONTAL( 0 ),
	HORIZONTAL( 1 ),
	HORIZONTAL( 2 ),
	VERTICAL( 0 ),
	VERTICAL( 1 ),
	VERTICAL( 2 ),
	{ 0, 4, 8 },
	{ 2, 4, 6 },
};

const short n_wins = sizeof wins / sizeof wins[ 0 ];

player_t get( unsigned i )
{
	return i < n_squares ? player_t( squares[ i ] ) : Player_none;
}

bool can_move( unsigned i )
{
	return i < n_squares  &&  squares[ i ] == 0;
}

bool can_undo()
{
	return ledger.size();
}

static
bool won( signed char squares[] )
{
	const short* p = *wins;
	
	for ( short w = 0;  w < n_wins;  ++w )
	{
		const short i = *p++;
		const short j = *p++;
		const short k = *p++;
		
		const signed char a = squares[ i ];
		const signed char b = squares[ j ];
		const signed char c = squares[ k ];
		
		if ( a  &&  a == b  &&  b == c )
		{
			return true;
		}
	}
	
	return false;
}

move_t move( player_t player, unsigned i )
{
	if ( ! can_move( i ) )
	{
		return Move_bad;
	}
	
	ledger.enter( encode_cell_index( i ) );
	
	squares[ i ] = player;
	
	return move_t( won( squares ) );
}

int undo_move()
{
	if ( Code code = ledger.pop() )
	{
		unsigned i = decode_cell_index( code );
		
		squares[ i ] = Player_none;
		
		return i;
	}
	
	return -1;
}

void reset()
{
	ledger.reset();
	
	for ( short i = 0;  i < n_squares;  ++i )
	{
		squares[ i ] = 0;
	}
}

const Code* extract()
{
	return ledger.entries;
}

bool restore( const Code* data, unsigned short size )
{
	signed char tentative_squares[ n_squares ] = {};
	
	player_t player = Player_X;
	player_t winner = Player_none;
	
	for ( unsigned short i = 0;  i < size;  ++i )
	{
		if ( winner )
		{
			return false;  // superfluous move after game over
		}
		
		Code code = data[ i ];
		
		if ( (code - 0xa0 & 0xf0) > 0x20u  ||  (code - 1 & 0xf) > 2u )
		{
			return false;  // invalid code
		}
		
		unsigned index = decode_cell_index( data[ i ] );
		
		if ( tentative_squares[ index ] )
		{
			return false;  // duplicate move in square
		}
		
		tentative_squares[ index ] = player;
		
		if ( won( tentative_squares ) )
		{
			winner = player;
		}
		
		player = opponent( player );
	}
	
	ledger.reset( data, size );
	
	for ( short i = 0;  i < n_squares;  ++i )
	{
		squares[ i ] = tentative_squares[ i ];
	}
	
	return true;  // passed all validation checks
}

}
