/*
	scrap.cc
	--------
*/

#include "scrap.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __SCRAP__
#include <Scrap.h>
#endif

// Tic-tac-toe
#include "state.hh"


#define CR  "\r"

#define STRLEN( s )  (sizeof "" s - 1)


using tictactoe::Code;


static const char empty_text_grid[] =
"   |   |   " CR
"---+---+---" CR
"   |   |   " CR
"---+---+---" CR
"   |   |   " CR;

static char text_grid[ sizeof empty_text_grid - 1 ];

static inline
unsigned decode_render_index( Code code )
{
	unsigned x = (code >>  4) - 0xa;
	unsigned y = (code & 0xF) - 1;
	
	return y * 2 * STRLEN( "---+---+---\r" ) + x * 4 + 1;
}

static
void render_text( const Code* data, unsigned short size )
{	
	BlockMoveData( empty_text_grid, text_grid, sizeof text_grid );
	
	const Byte token_switch = 'X' ^ 'O';
	
	Byte current_token = 'X';
	
	for ( int i = 0;  i < size;  ++i )
	{
		Code move = data[ i ];
		
		unsigned j = decode_render_index( move );
		
		text_grid[ j ] = current_token;
		
		current_token ^= token_switch;
	}
}

void copy_to_clipboard()
{
	const Code* data = tictactoe::extract();
	
	Size size = *data++;
	
#if ! TARGET_API_MAC_CARBON
	
	ZeroScrap();
	
	PutScrap( size, 'XvO#', (Ptr) data );
	
	render_text( data, size );
	
	PutScrap( sizeof text_grid, 'TEXT', text_grid );
	
#endif
}
