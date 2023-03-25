/*
	ram-disks.cc
	------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// POSIX
#include <unistd.h>

// mac-sys-utils
#include "mac_sys/unit_table.hh"

// gear
#include "gear/inscribe_decimal.hh"


#pragma exceptions off


#define STRLEN( s )  (sizeof "" s - 1)

#define PSTR_LEN( s )  "\p" s, sizeof s


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

static inline
bool is_RAM_disk_driver_name( ConstStr255Param name )
{
	return memcmp( name, PSTR_LEN( ".EDisk" ) ) == 0;
}

static inline
bool is_RAM_disk_driver( mac::types::AuxDCE** dceHandle )
{
	return is_RAM_disk_driver_name( mac::sys::get_driver_name( dceHandle ) );
}

static char report_buffer[] = "/sys/mac/vol/list/12345\n";

static inline
bool report_disk( short vRefNum )
{
	char* p = report_buffer + STRLEN( "/sys/mac/vol/list/" );
	
	p = gear::inscribe_unsigned_decimal_r( -vRefNum, p );
	
	*p++ = '\n';
	*p   = '\0';
	
	size_t len = p - report_buffer;
	
	return wrote( STDOUT_FILENO, report_buffer, len );
}

static inline
bool is_first_opt( const char* p )
{
	return *p++ == '-'  &&  *p++ == '1'  &&  *p++ == '\0';
}

int main( int argc, char** argv )
{
	const bool first_only = argc > 1  &&  is_first_opt( argv[ 1 ] );
	
	using mac::types::AuxDCE;
	
	AuxDCE*** const unit_table = mac::sys::get_unit_table_base();
	
	HParamBlockRec pb;
	
	pb.volumeParam.ioNamePtr = NULL;
	
	OSErr err;
	short count = 0;
	short index = 0;
	
	do
	{
		pb.volumeParam.ioVRefNum  = 0;
		pb.volumeParam.ioVolIndex = ++index;
		
		err = PBHGetVInfoSync( &pb );
		
		if ( err == noErr )
		{
			if ( const short dRefNum = pb.volumeParam.ioVDRefNum )
			{
				if ( is_RAM_disk_driver( unit_table[ ~dRefNum ] ) )
				{
					if ( ! report_disk( pb.volumeParam.ioVRefNum ) )
					{
						return 13;
					}
					
					if ( first_only )
					{
						return 0;
					}
					
					++count;
				}
			}
		}
	}
	while ( err == noErr );
	
	return err != nsvErr ? 36 : count ? 0 : 1;
}
