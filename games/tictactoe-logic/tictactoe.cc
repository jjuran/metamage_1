/*
	tictactoe.cc
	------------
*/

#include "tictactoe.hh"


#define HORIZONTAL( i )  { (i) * 3 + 0, (i) * 3 + 1, (i) * 3 + 2 }
#define VERTICAL( i )    { (i) + 3 * 0, (i) + 3 * 1, (i) + 3 * 2 }

namespace tictactoe
{

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

status_t restore( const Code* data, unsigned short size, signed char board[] )
{
	player_t player = Player_X;
	player_t winner = Player_none;
	
	for ( unsigned short i = 0;  i < size;  ++i )
	{
		if ( winner )
		{
			return Status_not_valid;  // superfluous move after game over
		}
		
		Code code = data[ i ];
		
		if ( (code - 0xa0 & 0xf0) > 0x20u  ||  (code - 1 & 0xf) > 2u )
		{
			return Status_not_valid;  // invalid code
		}
		
		unsigned index = decode_cell_index( data[ i ] );
		
		if ( board[ index ] )
		{
			return Status_not_valid;  // duplicate move in square
		}
		
		board[ index ] = player;
		
		if ( won( board ) )
		{
			winner = player;
		}
		
		player = opponent( player );
	}
	
	// Passed all validation checks.
	
	return status_t( winner ? winner * 2 : player );
}

}
