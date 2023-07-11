/*	=====
 *	ic.cc
 *	=====
 */

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __INTERNETCONFIG__
#include <InternetConfig.h>
#endif

// POSIX
#include <sys/uio.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)

#ifndef STDOUT_FILENO
#define STDOUT_FILENO  1
#endif


static inline void advance( const unsigned char*& p )
{
	p += 1 + p[0];
}

static void ReportMapping( const ICMapEntry& entry )
{
	short version = entry.version;
	
	const Byte* p = (const Byte*) &entry + entry.fixedLength;
	
	const Byte* extension = p;
	
	advance( p );
	
	const Byte* creatorAppName = p;
	
	advance( p );
	
	const Byte* postAppName = p;
	
	advance( p );
	
	const Byte* MIMEType = p;
	
	advance( p );
	
	const Byte* entryName = p;
	
	char vers_char = '0' + version;
	
	struct iovec iov[] =
	{
		{ (char*) STR_LEN( "v"   )                        },
		{ &vers_char,                 sizeof (char)       },
		{ (char*) STR_LEN( ": "  )                        },
		{ (char*) extension      + 1, extension     [ 0 ] },
		{ (char*) STR_LEN( " - " )                        },
		{ (char*) creatorAppName + 1, creatorAppName[ 0 ] },
		{ (char*) STR_LEN( ": "  )                        },
		{ (char*) MIMEType       + 1, MIMEType      [ 0 ] },
		{ (char*) STR_LEN( "\n"  )                        },
	};
	
	writev( STDOUT_FILENO, iov, sizeof iov / sizeof iov[0] );
}

int main( int argc, char** argv )
{
	OSErr err;
	ICInstance instance;
	
	err = ICStart( &instance, 'Poof' );
	
	if ( err == noErr )
	{
	#if ! TARGET_API_MAC_CARBON
		
		err = ICFindConfigFile( instance, 0, NULL );
		
	#endif
		
		if ( err == noErr )
		{
			err = ICBegin( instance, icReadWritePerm );
			
			if ( err == noErr )
			{
				if ( Handle h = NewHandle( 0 ) )
				{
					ICAttr attr;
					
					err = ICFindPrefHandle( instance, kICMapping, &attr, h );
					
					if ( err == noErr )
					{
						HLock( h );
						
						Ptr p = *h;
						Ptr end = p + GetHandleSize( h );
						
						while ( p < end )
						{
							ICMapEntry& entry = *(ICMapEntry*) p;
							
							ReportMapping( entry );
							
							p += entry.totalLength;
						}
					}
					
					DisposeHandle( h );
				}
				
				ICEnd( instance );
			}
		}
		
		ICStop( instance );
	}
	
	return 0;
}
