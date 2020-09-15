/*
	GetVol.c
	--------
*/

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <stdio.h>


int main( int argc, char** argv )
{
	short          vRefNum = 0;
	unsigned char  name[ 255 ];
	
	OSErr err = GetVol( name, &vRefNum );
	
	if ( err != noErr )
	{
		fprintf( stderr, "OSErr: %d\n", err );
		return 1;
	}
	else
	{
		unsigned char* p   = name;
		unsigned char  len = *p++;
		
		p[ len ] = '\0';
		
		printf( "%d\t%s\n", vRefNum, (const char*) p );
	}
	
	return 0;
}
