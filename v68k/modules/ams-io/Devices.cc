/*
	Devices.cc
	----------
*/

#include "Devices.hh"

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


#define LENGTH( array )  (sizeof array / sizeof *array)


typedef OSErr (*driver_routine)( short trap_word : __D1, IOParam* pb : __A0 );

typedef OSErr (*cntrl_routine)( short trap_word : __D1, CntrlParam* pb : __A0 );

static
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

static
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

static
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

#define COut_status  NULL

struct driver
{
	const uint8_t*  name;
	driver_routine  prime;
	cntrl_routine   status;
};

#define DRIVER( d )  { "\p." #d, d##_prime, d##_status }

static const driver drivers[] =
{
	DRIVER( CIn  ),
	DRIVER( COut ),
};

short Read_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	UInt16 index = -100 - pb->ioRefNum;
	
	if ( index >= LENGTH( drivers ) )
	{
		return pb->ioResult = unitEmptyErr;
	}
	
	const driver& d = drivers[ index ];
	
	if ( d.prime == NULL )
	{
		return pb->ioResult = readErr;
	}
	
	return d.prime( trap_word, pb );
}

short Write_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	UInt16 index = -100 - pb->ioRefNum;
	
	if ( index >= LENGTH( drivers ) )
	{
		return pb->ioResult = unitEmptyErr;
	}
	
	const driver& d = drivers[ index ];
	
	if ( d.prime == NULL )
	{
		return pb->ioResult = writErr;
	}
	
	return d.prime( trap_word, pb );
}

short Status_patch( short trap_word : __D1, CntrlParam* pb : __A0 )
{
	UInt16 index = -100 - pb->ioCRefNum;
	
	if ( index >= LENGTH( drivers ) )
	{
		return pb->ioResult = unitEmptyErr;
	}
	
	const driver& d = drivers[ index ];
	
	if ( d.status == NULL )
	{
		return pb->ioResult = statusErr;
	}
	
	return d.status( trap_word, pb );
}

short KillIO_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	return pb->ioResult = noErr;
}

short Open_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( pb->ioNamePtr == NULL )
	{
		return pb->ioResult = paramErr;
	}
	
	for ( short i = 0;  i < LENGTH( drivers );  ++i )
	{
		const driver& d = drivers[ i ];
		
		if ( memcmp( pb->ioNamePtr, d.name, 1 + *d.name ) == 0 )
		{
			pb->ioRefNum = -100 - i;
			
			return pb->ioResult = noErr;
		}
	}
	
	return pb->ioResult = fnfErr;
}
