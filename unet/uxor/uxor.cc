/*
	uxor.cc
	-------
*/

// POSIX
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PROGRAM  "uxor"


#define CHECK_R( call )  check_r( call, #call )

static inline int check_r( int result, const char* code )
{
	if ( result != 0 )
	{
		fprintf( stderr, PROGRAM " error: %s: %s\n", code, strerror( result ) );
		
		exit( 1 );
	}
	
	return -1;
}


static void neg_buffer( char* buffer, size_t n )
{
	char* end = buffer + n;
	
	for ( char* end = buffer + n;  buffer < end;  ++buffer )
	{
		*buffer = ~*buffer;
	}
}

struct fd_pair
{
	int recv;
	int send;
};

struct flow_state
{
	fd_pair    fds;
	pthread_t  thread;
};

static void* flow_thread_start( void* param )
{
	const fd_pair& fds = *(fd_pair*) param;
	
	char buffer[ 4096 ];
	
	ssize_t n_read;
	
	while ( (n_read = read( fds.recv, buffer, sizeof buffer )) > 0 )
	{
		neg_buffer( buffer, n_read );
		
		ssize_t n_written = write( fds.send, buffer, n_read );
		
		if ( n_written < 0 )
		{
			break;
		}
	}
	
	shutdown( fds.send, SHUT_WR );
	
	close( fds.send );
	close( fds.recv );
	
	return NULL;
}

int main( int argc, char** argv )
{
	if ( argc < 1 + 4 )
	{
		return 2;
	}
	
	const int clear_in_fd  = atoi( argv[ 1 ] );
	const int clear_out_fd = atoi( argv[ 2 ] );
	const int muxed_in_fd  = atoi( argv[ 3 ] );
	const int muxed_out_fd = atoi( argv[ 4 ] );
	
	flow_state going_there = { { clear_in_fd, muxed_out_fd } };
	flow_state coming_back = { { muxed_in_fd, clear_out_fd } };
	
	CHECK_R( pthread_create( &going_there.thread, NULL, &flow_thread_start, &going_there.fds ) );
	CHECK_R( pthread_create( &coming_back.thread, NULL, &flow_thread_start, &coming_back.fds ) );
	
	void* result;
	
	CHECK_R( pthread_join( going_there.thread, &result ) );
	CHECK_R( pthread_join( coming_back.thread, &result ) );
	
	return 0;
}
