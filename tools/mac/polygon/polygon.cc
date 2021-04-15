/*
	polygon.cc
	----------
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// iota
#include "iota/bit_iterator.hh"


#pragma exceptions off


static inline
int rowBytes_from_bitwidth( int bits )
{
	return (bits + 15) / 16u * 2;
}

const int icon_width  = 16;
const int icon_height = 16;

const int icon_px = icon_width * icon_height;  // 256
const int icon_bytes = icon_px / 8;  // 32;

const int icon_rowBytes = rowBytes_from_bitwidth( icon_width );  // 2

QDGlobals qd;

static GrafPtr testing_grafPort;

static
void init()
{
	InitGraf( &qd.thePort );
	
	testing_grafPort = (GrafPtr) malloc( sizeof (GrafPort) );
	
	OpenPort( testing_grafPort );
	
	PortSize( icon_width, icon_height );
	
	char* mem = (char*) malloc( icon_bytes );
	
	BitMap bitmap = { mem, icon_rowBytes, { 0, 0, icon_height, icon_width } };
	
	SetPortBits( &bitmap );
}

static
void term()
{
	SetPort( NULL );
	
	if ( testing_grafPort != NULL )
	{
		free( testing_grafPort->portBits.baseAddr );
		
		ClosePort( testing_grafPort );
		
		free( testing_grafPort );
	}
}

/*
	Each row contains N pixels, N-1 spaces, two '|', and one '\n'.
*/

static char buffer[ icon_width * 2 + 2 ];

int main( int argc, char** argv )
{
	const int n = argc - 1;
	
	if ( n < 3 )
	{
		return 0;
	}
	
	Point* points = (Point*) alloca( sizeof (Point) * n );
	
	char** args = argv + 1;
	
	for ( int i = 0;  i < n;  ++i )
	{
		const char* arg = args[ i ];
		const char* sep = strchr( arg, ',' );
		
		if ( ! sep++ )
		{
			return 1;
		}
		
		points[ i ].v = atoi( arg );
		points[ i ].h = atoi( sep );
	}
	
	init();
	
	const char* base   = testing_grafPort->portBits.baseAddr;
	const Rect& bounds = testing_grafPort->portBits.bounds;
	
	EraseRect( &bounds );
	
	PolyHandle poly = OpenPoly();
	
	MoveTo( points[ 0 ].v, points[ 0 ].h );
	
	for ( int i = 1;  i < n;  ++i )
	{
		LineTo( points[ i ].v, points[ i ].h );
	}
	
	ClosePoly();
	
	PaintPoly( poly );
	
	KillPoly( poly );
	
	using iota::bit_iterator;
	
	bit_iterator bits = base;
	
	for ( int i = 0;  i < icon_height;  ++i )
	{
		char* p = buffer;
		
		*p++ = '|';
		
		for ( int j = 0;  j < icon_width;  ++j )
		{
			*p++ = " X"[ *bits ];
			*p++ = ' ';
			
			++bits;
		}
		
		*p = '\n';
		
		*--p = '|';
		
		write( STDOUT_FILENO, buffer, sizeof buffer );
	}
	
	term();
	
	return 0;
}
