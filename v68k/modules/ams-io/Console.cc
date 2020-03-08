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


OSErr CIn_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 )
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
	
	const short immed = pb->ioTrap & (1 << noQueueBit);
	
	if ( ! immed )
	{
		IODone( dce, err );
	}
	
	return err;
}

OSErr COut_prime( short trap_word : __D1, IOParam* pb : __A0, DCE* dce : __A1 )
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
	
	const short immed = pb->ioTrap & (1 << noQueueBit);
	
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
void CIn_watch( int fd )
{
	typedef reactor_core_parameter_block pb_t;
	
	if ( pb_t* reactor = (pb_t*) mac::sys::gestalt( gestaltReactorCoreAddr ) )
	{
		if ( CIn_reactor_node.ready == NULL )
		{
			CIn_reactor_node.fd    = fd;
			CIn_reactor_node.ready = &CIn_ready;
			
			reactor->insert( &CIn_reactor_node );
		}
	}
}

OSErr CIn_status( short trap : __D1, CntrlParam* pb : __A0, DCE* dce : __A1 )
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
				CIn_watch( fd );
			}
			break;
		
		default:
			err = statusErr;
			goto done;
	}
	
done:
	
	const short immed = pb->ioTrap & (1 << noQueueBit);
	
	if ( ! immed )
	{
		IODone( dce, err );
	}
	
	return err;
}
