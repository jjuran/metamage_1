/*
	sndtrack.cc
	-----------
*/

#include "sndtrack.hh"

#ifdef __APPLE__

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// Standard C
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Organ Console
#include "waveform.hh"


typedef unsigned char Byte;

static pid_t sndtrack_pid;

static int pipe_fds[ 2 ];


static
void launch_sndtrack()
{
	if ( ! sndtrack_pid )
	{
		pipe( pipe_fds );
		
		pid_t pid = vfork();
		
		if ( pid == 0 )
		{
			dup2( pipe_fds[ 0 ], STDIN_FILENO );
			
			close( pipe_fds[ 0 ] );
			close( pipe_fds[ 1 ] );
			
			const char* sndtrack = "./sndtrack";
			
			execl( sndtrack, sndtrack, NULL );
			
			perror( "sndtrack" );
			
			_exit( 127 );
		}
		
		close( pipe_fds[ 0 ] );
		
		if ( pid < 0 )
		{
			close( pipe_fds[ 1 ] );
			
			perror( "sndtrack" );
		}
		else
		{
			sndtrack_pid = pid;
		}
	}
}

static
void quit_sndtrack()
{
	if ( sndtrack_pid )
	{
		close( pipe_fds[ 1 ] );
		
		int status;
		wait( &status );
		
		sndtrack_pid = 0;
	}
}

static
void send_to_sndtrack( const void* buffer, unsigned n )
{
	if ( sndtrack_pid )
	{
		ssize_t n_written = write( pipe_fds[ 1 ], buffer, n );
	}
}

static
void start_sndtrack()
{
	launch_sndtrack();
	send_to_sndtrack( "\0\0\0\0\1\1\0\0", 8 );  // switch on
}

void full_stop()
{
	send_to_sndtrack( "\0\0\0\0\1\1XX", 8 );  // full stop
}

static
void abort_sndtrack()
{
	full_stop();
	quit_sndtrack();
}

const int header_len = 2 + 2 + 2 + 2;
const int abbrev_len = 4 + 2 + 4 * 8;
const int extent_len = 4 + 2 + 4 * 8 + 4 * 256;
const int buffer_len = header_len + extent_len;
const int update_len = header_len + abbrev_len;

static Byte fourtone_buffer[ buffer_len ];
static Byte fourtone_update[ buffer_len ];

static
void fill_fourtone_buffer_header()
{
	fourtone_buffer[ 2 ] = extent_len / 256u;
	fourtone_buffer[ 3 ] = extent_len % 256u;
	fourtone_buffer[ 4 ] = 'K';
	fourtone_buffer[ 5 ] = 'K';
	fourtone_buffer[ 6 ] = 1;
	fourtone_buffer[ 7 ] = 1;
}

static
void fill_fourtone_update_header()
{
	fourtone_update[ 2 ] = abbrev_len / 256u;
	fourtone_update[ 3 ] = abbrev_len % 256u;
	fourtone_update[ 4 ] = 'J';
	fourtone_update[ 5 ] = 'J';
	fourtone_update[ 6 ] = 2;
	fourtone_update[ 7 ] = 1;
}

static
void fill_wavetable( int channel, const uint8_t* wave )
{
	int i = 21 + channel *   8;  // phase
	int j = 46 + channel * 256;  // wave
	
	memcpy( &fourtone_buffer[ j ], wave, 256 );
	
	fourtone_buffer[ i ] = 0;
}

void fourtone_amend_rate( int channel, FTRate rate )
{
	int i = 14 + channel * 8;
	
	fourtone_update[ i++ ] = rate >> 24;
	fourtone_update[ i++ ] = rate >> 16;
	fourtone_update[ i++ ] = rate >>  8;
	fourtone_update[ i++ ] = rate;
}

void fourtone_start( FTRate rate )
{
	start_sndtrack();
	
	fill_fourtone_buffer_header();
	
	fourtone_buffer[ 12 ] = 127;
	fourtone_buffer[ 13 ] = 255;
	
	fourtone_buffer[ 14 ] = rate >> 24;
	fourtone_buffer[ 15 ] = rate >> 16;
	fourtone_buffer[ 16 ] = rate >>  8;
	fourtone_buffer[ 17 ] = rate;
	
	/*
		fill_sawtooth
		fill_squarewave
		fill_triangle
	*/
	
	fill_wavetable( 0, organ );
	fill_wavetable( 1, organ );
	fill_wavetable( 2, organ );
	fill_wavetable( 3, organ );
	
	memcpy( fourtone_update, fourtone_buffer, sizeof fourtone_update );
	
	fourtone_update[ 2 ] = abbrev_len / 256u;
	fourtone_update[ 3 ] = abbrev_len % 256u;
	fourtone_update[ 4 ] = 'J';
	fourtone_update[ 5 ] = 'J';
	fourtone_update[ 6 ] = 2;
	
	send_to_sndtrack( fourtone_buffer, buffer_len );
}

void fourtone_amend_submit()
{
	send_to_sndtrack( fourtone_update, update_len );
}

#endif
