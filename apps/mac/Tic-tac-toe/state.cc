/*
	state.cc
	--------
*/

#include "state.hh"


#define HORIZONTAL( i )  { (i) * 3 + 0, (i) * 3 + 1, (i) * 3 + 2 }
#define VERTICAL( i )    { (i) + 3 * 0, (i) + 3 * 1, (i) + 3 * 2 }

namespace tictactoe
{

static signed char squares[ n_squares ];

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

static
bool won()
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
	
	squares[ i ] = player;
	
	return move_t( won() );
}

}
