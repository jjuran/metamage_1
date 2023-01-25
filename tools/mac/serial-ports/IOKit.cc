/*
	IOKit.cc
	--------
*/

#include "IOKit.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#endif

// Standard C
#include <stdio.h>


#ifdef __MACH__

static
void print( CFStringRef s )
{
	char buffer[ 4096 ];
	
	CFStringGetCString( s, buffer, sizeof buffer, kCFStringEncodingASCII );
	
	printf( "%s\n", buffer );
}

int list_IOKit_serial_ports( bool all_types )
{
	io_iterator_t iterator;
	
	mach_port_t masterPort;
	
	kern_return_t kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
	
	if ( kr )
	{
		fprintf( stderr, "IOMasterPort returned %d\n", kr );
		return 1;
	}
	
	CFMutableDictionaryRef classesToMatch;
	
	classesToMatch = IOServiceMatching( kIOSerialBSDServiceValue );
	
	if ( ! classesToMatch )
	{
		fprintf( stderr, "IOServiceMatching returned a NULL dictionary\n" );
		return 1;
	}
	
	CFDictionarySetValue( classesToMatch,
	                      CFSTR( kIOSerialBSDTypeKey ),
	                      all_types ? CFSTR( kIOSerialBSDAllTypes )
	                                : CFSTR( kIOSerialBSDModemType ) );
	
	kr = IOServiceGetMatchingServices( masterPort, classesToMatch, &iterator );    
	
	if ( kr )
	{
		fprintf( stderr, "IOServiceGetMatchingServices returned %d\n", kr );
		return 1;
	}
	
	while ( io_object_t next = IOIteratorNext( iterator ) )
	{
		CFTypeRef path;
		
		path = IORegistryEntryCreateCFProperty( next,
		                                        CFSTR( kIOCalloutDeviceKey ),
		                                        kCFAllocatorDefault,
		                                        0 );
		
		if ( path )
		{
			print( (CFStringRef) path );
			
			CFRelease( path );
		}
	}
	
	return 0;
}

#endif  // #ifdef __MACH__
