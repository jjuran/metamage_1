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

// Standard C/C++
#include <cstdio>

// plus
#include "plus/string.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	struct ICMapEntryStrings
	{
		plus::string  extension;
		plus::string  creatorAppName;
		plus::string  postAppName;
		plus::string  MIMEType;
		plus::string  entryName;
	};
	
	static inline void advance( const unsigned char*& p )
	{
		p += 1 + p[0];
	}
	
	static ICMapEntryStrings GetStringsFromICMapEntry( const ICMapEntry& entry )
	{
		ICMapEntryStrings result;
		
		const unsigned char* p = (const unsigned char*) &entry + entry.fixedLength;
		
		result.extension = p;
		
		advance( p );
		
		result.creatorAppName = p;
		
		advance( p );
		
		result.postAppName = p;
		
		advance( p );
		
		result.MIMEType = p;
		
		advance( p );
		
		result.entryName = p;
		
		return result;
	}
	
	static void ReportMapping( const ICMapEntry& entry )
	{
		short version = entry.version;
		
		//const unsigned char* ext_ension = (const unsigned char*) entry + entry.fixedLength;
		
		ICMapEntryStrings strings = GetStringsFromICMapEntry( entry );
		
		const char* extension      = strings.extension     .c_str();
		const char* creatorAppName = strings.creatorAppName.c_str();
		const char* postAppName    = strings.postAppName   .c_str();
		const char* MIMEType       = strings.MIMEType      .c_str();
		const char* entryName      = strings.entryName     .c_str();
		
		std::printf( "v%d: %s - %s: %s\n", version, extension, creatorAppName, MIMEType );
	}
	
	int Main( int argc, char** argv )
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

}
