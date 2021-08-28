/*
	state.cc
	--------
*/

#include "state.hh"


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

player_t get( unsigned i )
{
	return i < n_squares ? player_t( squares[ i ] ) : Player_none;
}

bool can_move( unsigned i )
{
	return ! winning_player  &&  i < n_squares  &&  squares[ i ] == 0;
}

bool can_undo()
{
	return ledger.size();
}

move_t move( unsigned i )
{
	if ( ! can_move( i ) )
	{
		return Move_bad;
	}
	
	ledger.enter( encode_cell_index( i ) );
	
	squares[ i ] = current_player;
	
	const bool game_over = won( squares );
	
	if ( game_over )
	{
		winning_player = current_player;
	}
	
	current_player = opponent( current_player );
	
	return move_t( game_over );
}

int undo_move()
{
	if ( Code code = ledger.pop() )
	{
		unsigned i = decode_cell_index( code );
		
		squares[ i ] = Player_none;
		
		winning_player = Player_none;
		current_player = opponent( current_player );
		
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
	
	winning_player = Player_none;
	current_player = Player_X;
}

const Code* extract()
{
	return ledger.entries;
}

bool restore( const Code* data, unsigned short size )
{
	signed char tentative_squares[ n_squares ] = {};
	
	status_t status = restore( data, size, tentative_squares );
	
	if ( status )
	{
		winning_player = winner( status );
		current_player = winning_player ? opponent( winning_player )
		                                : player_t( status );
		
		ledger.reset( data, size );
		
		for ( short i = 0;  i < n_squares;  ++i )
		{
			squares[ i ] = tentative_squares[ i ];
		}
	}
	
	return status;
}

}
