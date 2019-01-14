/*
	Console.cc
	----------
*/

#include "Console.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <unistd.h>
#include <sys/select.h>

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// ams-common
#include "reactor.hh"
#include "reactor-gestalt.hh"


OSErr CIn_prime( short trap_word : __D1, IOParam* pb : __A0 )
{
	Ptr     buffer = pb->ioBuffer;
	ssize_t needed = pb->ioReqCount;
	
	pb->ioActCount = 0;
	
	while ( needed > 0 )
	{
		ssize_t n_read = read( STDIN_FILENO, buffer, needed );
		
		if ( n_read < 0 )
		{
			return pb->ioResult = ioErr;
		}
		
		if ( n_read == 0 )
		{
			return pb->ioResult = eofErr;
		}
		
		pb->ioActCount += n_read;
		buffer         += n_read;
		needed         -= n_read;
	}
	
	return pb->ioResult = noErr;
}

OSErr COut_prime( short trap_word : __D1, IOParam* pb : __A0 )
{
	ssize_t n_written = write( STDOUT_FILENO, pb->ioBuffer, pb->ioReqCount );
	
	if ( n_written < 0 )
	{
		return pb->ioResult = ioErr;
	}
	
	pb->ioActCount = n_written;
	
	return pb->ioResult = noErr;
}

static
ssize_t readable_bytes( int fd )
{
	// TODO:  Check FIONREAD
	
	fd_set readfds;
	FD_ZERO( &readfds );
	FD_SET( fd, &readfds );
	
	const int max_fd = fd;
	
	timeval timeout = { 0 };
	
	int selected = select( max_fd + 1, &readfds, NULL, NULL, &timeout );
	
	return selected > 0;
}

static reactor_node CIn_reactor_node;

static
void CIn_ready( reactor_node* node )
{
	typedef reactor_core_parameter_block pb_t;
	
	pb_t* reactor = (pb_t*) mac::sys::gestalt( gestaltReactorCoreAddr );
	
	reactor->remove( node );
	
	CIn_reactor_node.ready = NULL;  // Needed to prevent double insertion
	
	PostEvent( driverEvt, 0 );
}

static
void CIn_watch()
{
	typedef reactor_core_parameter_block pb_t;
	
	if ( pb_t* reactor = (pb_t*) mac::sys::gestalt( gestaltReactorCoreAddr ) )
	{
		if ( CIn_reactor_node.ready == NULL )
		{
			CIn_reactor_node.fd    = STDIN_FILENO;
			CIn_reactor_node.ready = &CIn_ready;
			
			reactor->insert( &CIn_reactor_node );
		}
	}
}

OSErr CIn_status( short trap_word : __D1, CntrlParam* pb : __A0 )
{
	enum
	{
		kSERDInputCount = 2,
	};
	
	switch ( pb->csCode )
	{
		case kSERDInputCount:
			if ( (*(long*) pb->csParam = readable_bytes( STDIN_FILENO )) == 0 )
			{
				CIn_watch();
			}
			break;
		
		default:
			return pb->ioResult = statusErr;
	}
	
	return pb->ioResult = noErr;
}
