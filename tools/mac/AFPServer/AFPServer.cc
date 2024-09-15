/*
	AFPServer.cc
	------------
*/

// Mac OS
#ifndef __APPLETALK__
#include <AppleTalk.h>
#endif
#ifndef __DEVICES__
#include <Devices.h>
#endif

// Standard C
#include <stdio.h>

// more-libc
#include "more/string.h"


#pragma exceptions off


#if CALL_NOT_IN_CARBON

static
short get_server_status( AddrBlock addr )
{
	OSErr err;
	short refNum;
	
	err = OpenDriver( "\p.XPP", &refNum );
	
	if ( err )  return err;
	
	char status[ 512 ];
	
	XPPParamBlock pb;
	
	ASPOpenPrm& pbASP = *(ASPOpenPrm*) &pb;
	XPPPrmBlk&  pbXPP = *(XPPPrmBlk *) &pb;
	
	pbASP.ioRefNum   = refNum;
	pbASP.csCode     = getStatus;
	pbASP.aspTimeout = 5;
	pbASP.aspRetry   = 3;
	pbASP.serverAddr = addr;
	
	pbXPP.rbSize = sizeof status;
	pbXPP.rbPtr  = status;
	
	err = ASPGetStatus( (XPPParamBlock*) &pbXPP, false );
	
	if ( err )  return err;
	
	/*
		If the byte at offset 9 in the status is -5,
		then the server doesn't support AppleTalk.
	*/
	
	return status[ 9 ] == (char) -5;
}

#endif

static
void list_servers()
{
#if CALL_NOT_IN_CARBON
	
	EntityName entity;
	
	const Byte* name = "\p" "=";
	const Byte* type = "\p" "AFPServer";
	const Byte* zone = "\p" "*";
	
	NBPSetEntity( (Ptr) &entity, name, type, zone );
	
	NBPparms pb;
	
	char buffer[ 1024 ];
	
	pb.csCode   = lookupName;
	pb.interval = 5;
	pb.count    = 3;
	pb.nbpPtrs.entityPtr = (Ptr) &entity;
	pb.parm.Lookup.retBuffPtr  = buffer;
	pb.parm.Lookup.retBuffSize = sizeof buffer;
	pb.parm.Lookup.maxToGet    = 10;
	
	OSErr err = PLookupNameSync( (MPPPBPtr) &pb );
	
	if ( err )
	{
		fprintf( stderr, "PLookupName failed: %d\n", err );
		
		return;
	}
	
	short count = pb.parm.Lookup.numGotten;
	
	for ( int i = 1;  i <= count;  ++i )
	{
		AddrBlock addr;
		
		err = NBPExtract( buffer, count, i, &entity, &addr );
		
		if ( err )
		{
			fprintf( stderr, "NBPExtract failed: %d\n", err );
			
			break;
		}
		
		char name[ 33 ];
		
		Byte len = entity.objStr[ 0 ];
		
		char* p = (char*) mempcpy( name, entity.objStr + 1, len );
		
		*p = '\0';
		
		short status = get_server_status( addr );
		
		if ( status < 0 )
		{
			fprintf( stderr, "ASPGetStatus failed: %d\n", status );
		}
		
		char c = status < 0 ? '!'
		       : status > 0 ? ' '
		       :              'X';
		
		printf( "[%c]  %s\n", c, name );
	}
	
#endif
}

int main( int argc, char** argv )
{
	list_servers();
	
	return 0;
}
