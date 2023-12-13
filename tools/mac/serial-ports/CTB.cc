/*
	CTB.cc
	------
*/

#include "CTB.hh"

// Mac OS
#ifdef __MACOS__
#ifndef __COMMRESOURCES__
#include <CommResources.h>
#endif
#ifndef __CRMSERIALDEVICES__
#include <CRMSerialDevices.h>
#endif
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdio.h>

// more-libc
#include "more/string.h"

// mac-glue-utils
#include "mac_glue/Memory.hh"


struct CStr255
{
	char data[ 256 ];
	
	const char* get()      const  { return data; }
	operator const char*() const  { return data; }
	
	CStr255()
	{
		data[ 0 ] = '\0';
	}
	
	CStr255( Byte  const* s );
	CStr255( Byte* const* s );
};

static
void assign( CStr255& cstr, const Byte* s )
{
	char* p = cstr.data;
	
	size_t size = *s++;
	
	p = (char*) mempcpy( p, s, size );
	
	*p = '\0';
}

inline
CStr255::CStr255( Byte  const* s )
{
	assign( *this, s );
}

inline
CStr255::CStr255( Byte* const* s )
{
	assign( *this, *s );
}

#if CALL_NOT_IN_CARBON

static CRMRec query;

int list_CTB_serial_ports( int verbosity )
{
	InitCRM();
	
	short version = CRMGetCRMVersion();
	
	if ( verbosity > 1 )
	{
		printf( "CRM version: %d\n", version );
		printf( "\n" );
	}
	
	query.crmVersion    = crmRecVersion;
	query.crmDeviceType = crmSerialDevice;
//	query.crmDeviceID   = 0;
	
	CRMRecPtr crmRec = CRMSearch( &query );
	
	while ( crmRec )
	{
		#define PRINT_CRM_FIELD(name)  \
			if ( crmRec->name  )  printf( #name ":  %ld\n", crmRec->name  )
		
		if ( verbosity > 1 )
		{
			printf( "CRM device ID: %d\n", crmRec->crmDeviceID );
			
			PRINT_CRM_FIELD( crmPrivate  );
			PRINT_CRM_FIELD( crmReserved );
			PRINT_CRM_FIELD( crmStatus   );
			PRINT_CRM_FIELD( crmRefCon   );
			
			printf( "\n" );
		}
		
		#undef PRINT_CRM_FIELD
		
		if ( CRMSerialPtr serial = (CRMSerialPtr) crmRec->crmAttributes )
		{
			CStr255 name   = serial->name;
			CStr255 input  = serial->inputDriverName;
			CStr255 output = serial->outputDriverName;
			
			if ( verbosity == 0 )
			{
				printf( "%s\n", name.get() );
			}
			else
			{
				printf( "%s [%s, %s]\n",
				        name.get(),
				        input.get(),
				        output.get() );
			}
			
			if ( verbosity > 1 )
			{
				printf( "    device icon: %p\n", serial->deviceIcon );
				
				if ( CRMIconHandle icon_h = serial->deviceIcon )
				{
					Size size = mac::glue::GetHandleSize_raw( (Handle) icon_h );
					
					printf( "    icon bytes: %ld\n", size );
					
					CRMIconRecord icon = **icon_h;
					
					printf( "    icon suite: %p\n", icon.theSuite );
					
					if ( icon.reserved )
					{
						printf( "    icon reserved: %ld\n", icon.reserved );
					}
				}
				
				printf( "    rated speed: %ld\n", serial->ratedSpeed );
				printf( "    max   speed: %ld\n", serial->maxSpeed );
				
				if ( serial->reserved )
				{
					printf( "    reserved: %ld\n", serial->reserved );
				}
				
				printf( "\n" );
			}
		}
		
		crmRec = CRMSearch( crmRec );
	}
	
	return 0;
}

#endif  // #if CALL_NOT_IN_CARBON
