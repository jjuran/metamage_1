/*
	Serial.cc
	---------
*/

#include "Serial.hh"

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

// ams-common
#include "reactor.hh"

// ams-io
#include "reactor-core.hh"


#pragma exceptions off


const short noQueueMask   = 1 << noQueueBit;
const short asyncTrapMask = 1 << asyncTrpBit;

OSErr xIn_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 )
{
	const int fd = dce->dCtlPosition;
	
	OSErr err = noErr;
	
	Ptr     buffer = pb->ioBuffer;
	ssize_t needed = pb->ioReqCount;
	
	pb->ioActCount = 0;
	
	while ( needed > 0 )
	{
		ssize_t n_read = read( fd, buffer, needed );
		
		if ( n_read < 0 )
		{
			err = ioErr;
			goto done;
		}
		
		if ( n_read == 0 )
		{
			err = eofErr;
			goto done;
		}
		
		pb->ioActCount += n_read;
		buffer         += n_read;
		needed         -= n_read;
	}
	
done:
	
	const short immed = pb->ioTrap & noQueueMask;
	
	if ( ! immed )
	{
		IODone( dce, err );
	}
	
	return err;
}

OSErr xOut_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 )
{
	const int fd = dce->dCtlPosition;
	
	OSErr err = noErr;
	
	ssize_t n_written = write( fd, pb->ioBuffer, pb->ioReqCount );
	
	if ( n_written < 0 )
	{
		err = ioErr;
		goto done;
	}
	
	pb->ioActCount = n_written;
	
done:
	
	const short immed = pb->ioTrap & noQueueMask;
	
	if ( ! immed )
	{
		IODone( dce, err );
	}
	
	return err;
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

/*
	At the moment there's only one reactor node, with one fd slot,
	so only one serial port can be waited for at a time.
*/

static reactor_node xIn_reactor_node;

static
void xIn_ready( reactor_node* node )
{
	the_reactor_core->remove( node );
	
	xIn_reactor_node.ready = NULL;  // Needed to prevent double insertion
	
	PostEvent( driverEvt, 0 );
}

static
void xIn_watch( int fd )
{
	if ( the_reactor_core )
	{
		if ( xIn_reactor_node.ready == NULL )
		{
			xIn_reactor_node.fd    = fd;
			xIn_reactor_node.ready = &xIn_ready;
			
			the_reactor_core->insert( &xIn_reactor_node );
		}
	}
}

OSErr xIn_status( short trap : __D1, CntrlParam* pb : __A0, DCE* dce : __A1 )
{
	enum
	{
		kSERDInputCount = 2,
	};
	
	const int fd = dce->dCtlPosition;
	
	OSErr err = noErr;
	
	switch ( pb->csCode )
	{
		case kSERDInputCount:
			if ( (*(long*) pb->csParam = readable_bytes( fd )) == 0 )
			{
				xIn_watch( fd );
			}
			break;
		
		default:
			err = statusErr;
			goto done;
	}
	
done:
	
	const short immed = pb->ioTrap & noQueueMask;
	
	if ( ! immed )
	{
		IODone( dce, err );
	}
	
	return err;
}
