/*
	DRVR.cc
	-------
*/

#include "DRVR.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif


static
UInt16* set_DRVR_offset( void* base, UInt16* p, short* field, long address )
{
	*field = (char*) p - (char*) base;
	
	*p++ = 0x4EF9;
	*p++ = address >> 16;
	*p++ = address;
	
	return p;
}

static inline
UInt16* set_DRVR_offset( void* base, UInt16* p, short* field, driver_routine f )
{
	return set_DRVR_offset( base, p, field, (long) f );
}

static inline
UInt16* set_DRVR_offset( void* base, UInt16* p, short* field, cntrl_routine f )
{
	return set_DRVR_offset( base, p, field, (long) f );
}

DRVRHeader** make_DRVR( ConstStr255Param  name,
                        driver_routine    open,
                        driver_routine    prime,
                        cntrl_routine     cntrl,
                        cntrl_routine     status,
                        driver_routine    close )
{
	const int RTS_size = 2;
	const int JMP_size = 6;
	
	size_t name_offset = offsetof( DRVRHeader, drvrName );
	
	size_t header_size = (name_offset + 1 + name[ 0 ] + 1) & ~1;
	
	const short n_nulls = ! open + ! prime + ! cntrl + ! status + ! close;
	
	const short n_jumps = 5 - n_nulls;
	
	const size_t block_size = header_size
	                        + RTS_size * (n_nulls > 0)
	                        + JMP_size * n_jumps;
	
	DRVRHeader** h = (DRVRHeader**) NewHandleClear( block_size );
	
	if ( h )
	{
		DRVRHeader& header = **h;
		
		BlockMoveData( name, &header.drvrName, 1 + name[ 0 ] );
		
		UInt16* p = (UInt16*) ((char*) *h + header_size);
		
		if ( n_nulls )
		{
			header.drvrOpen   = sizeof header;
			header.drvrPrime  = sizeof header;
			header.drvrCtl    = sizeof header;
			header.drvrStatus = sizeof header;
			header.drvrClose  = sizeof header;
			
			*p++ = 0x4E75;  // RTS
		}
		
		if ( open   )  p = set_DRVR_offset( *h, p, &header.drvrOpen,   open   );
		if ( prime  )  p = set_DRVR_offset( *h, p, &header.drvrPrime,  prime  );
		if ( cntrl  )  p = set_DRVR_offset( *h, p, &header.drvrCtl,    cntrl  );
		if ( status )  p = set_DRVR_offset( *h, p, &header.drvrStatus, status );
		if ( close  )  p = set_DRVR_offset( *h, p, &header.drvrClose,  close  );
	}
	
	return h;
}
