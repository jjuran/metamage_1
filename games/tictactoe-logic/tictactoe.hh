/*
	tictactoe.hh
	------------
*/

#ifndef TICTACTOE_HH
#define TICTACTOE_HH

namespace tictactoe
{

/*
	Cell indices:    Coordinates:
	
	 0 | 1 | 2        a1 | b1 | c1
	---+---+---      ----+----+----
	 3 | 4 | 5        a2 | b2 | c2
	---+---+---      ----+----+----
	 6 | 7 | 8        a3 | b3 | c3
*/

typedef unsigned char Code;

inline
Code encode_cell_index( unsigned i )
{
	Code code = 0xa0 + i % 3 * 0x10
	          + 0x01 + i / 3 * 0x01;
	
	return code;
}

inline
unsigned decode_cell_index( Code code )
{
	return ((code & 0xF) - 1) * 3 + (code >> 4) - 0xa;
}

enum player_t
{
	Player_none,
	
	Player_X =  1,
	Player_O = -1,
};

enum status_t
{
	Status_O_has_won = -2,
	Status_O_is_next = -1,
	Status_not_valid =  0,
	Status_X_is_next =  1,
	Status_X_has_won =  2,
};

inline
player_t opponent( player_t player )
{
	return player_t( -player );
}

inline
player_t winner( status_t status )
{
	return player_t( status / 2 );
}

const unsigned n_squares = 9;

bool won( signed char squares[] );

status_t restore( const Code* data, unsigned short size, signed char board[] );

}

#endif
