/*
	Scrap.cc
	--------
*/

#include "Scrap.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __SCRAP__
#include <Scrap.h>
#endif


#ifndef __MACERRORS__
enum
{
	noScrapErr = -100,
	noTypeErr  = -102,
};
#endif

short MemErr : 0x0220;

short  TEScrpLength : 0x0AB0;
Handle TEScrpHandle : 0x0AB4;


static unsigned char* scrapName = "\p" "Clipboard File";

static ScrapStuff scrapVars =
{
	0,
	NULL,
	0,
	-1,
	scrapName,
};

#pragma mark -
#pragma mark Getting Desk Scrap Information
#pragma mark -

pascal ScrapStuff* InfoScrap_patch()
{
	return &scrapVars;
}

#pragma mark -
#pragma mark Keeping the Desk Scrap on the Disk
#pragma mark -

pascal long UnloadScrap_patch()
{
	OSErr err;
	short refnum;
	
	if ( scrapVars.scrapState == 0 )
	{
		return noErr;  // scrap already on disk
	}
	
	if ( scrapVars.scrapState < 0 )
	{
		return noScrapErr;  // scrap not initialized
	}
	
	err = FSOpen( scrapName, fsRdWrPerm, &refnum );
	
	if ( err == noErr )
	{
		Handle h = scrapVars.scrapHandle;
		
		Size size = scrapVars.scrapSize;
		
		(err = FSWrite( refnum, &size, *h ))  ||
		(err = SetEOF ( refnum, size ));
		
		FSClose( refnum );
		
		if ( err == noErr )
		{
			DisposeHandle( h );
			
			scrapVars.scrapHandle = NULL;
			scrapVars.scrapState  = 0;
		}
	}
	
	return err;
}

pascal long LoadScrap_patch()
{
	OSErr err;
	short refnum;
	
	if ( scrapVars.scrapState > 0 )
	{
		return noErr;  // scrap already in memory
	}
	
	if ( scrapVars.scrapState < 0 )
	{
		return noScrapErr;  // scrap not initialized
	}
	
	err = FSOpen( scrapName, fsRdPerm, &refnum );
	
	if ( err == noErr )
	{
		err = memFullErr;
		
		Size size = scrapVars.scrapSize;
		
		if ( Handle h = NewHandle( size ) )
		{
			if ( (err = FSRead( refnum, &size, *h )) )
			{
				DisposeHandle( h );
			}
			else
			{
				scrapVars.scrapHandle = h;
				scrapVars.scrapState  = 1;
			}
		}
		
		FSClose( refnum );
	}
	
	return err;
}

#pragma mark -
#pragma mark Writing to the Desk Scrap
#pragma mark -

pascal long ZeroScrap_patch()
{
	if ( Handle h = scrapVars.scrapHandle )
	{
		SetHandleSize( h, 0 );
	}
	else if ( scrapVars.scrapState == 0 )
	{
		OSErr err;
		short refnum;
		
		err = FSOpen( scrapName, fsRdPerm, &refnum );
		
		if ( err == noErr )
		{
			err = SetEOF( refnum, 0 );
			
			FSClose( refnum );
			
			if ( err )
			{
				return err;
			}
		}
	}
	else
	{
		scrapVars.scrapHandle = NewHandle( 0 );
		
		if ( ! scrapVars.scrapHandle )
		{
			return memFullErr;
		}
		
		scrapVars.scrapState = 1;
	}
	
	scrapVars.scrapSize = 0;
	scrapVars.scrapCount++;
	
	return noErr;
}

pascal long PutScrap_patch( long length, ResType type, const char* src )
{
	short prev_state = scrapVars.scrapState;
	
	if ( prev_state < 0 )
	{
		return noScrapErr;
	}
	
	Handle h = scrapVars.scrapHandle;
	
	if ( ! h )
	{
		if ( long err = LoadScrap() )
		{
			return err;
		}
		
		h = scrapVars.scrapHandle;
	}
	
	/*
		Append the new scrap entry to the scrap handle, starting with
		the 8-byte type/length header.
		
		Keep each scrap entry a multiple of two bytes.  Since handle sizes
		are also rounded up to (at least) two bytes, this won't fail.
	*/
	
	OSErr err = (PtrAndHand( &type, h, 8            ), MemErr)  ||
	            (PtrAndHand( src,   h, length       ), MemErr)  ||
	            (PtrAndHand( "\0",  h, length & 0x1 ), MemErr);
	
	if ( err )
	{
		SetHandleSize( h, scrapVars.scrapSize );
	}
	else
	{
		scrapVars.scrapSize += 8 + length + (length & 0x1);
	}
	
	if ( prev_state == 0 )
	{
		UnloadScrap();
	}
	
	return err;
}

#pragma mark -
#pragma mark Reading from the Desk Scrap
#pragma mark -

pascal long GetScrap_patch( Handle dst, ResType type, long* offset )
{
	short prev_state = scrapVars.scrapState;
	
	if ( prev_state < 0 )
	{
		return noScrapErr;
	}
	
	Handle h = scrapVars.scrapHandle;
	
	if ( ! h )
	{
		if ( long err = LoadScrap() )
		{
			return err;
		}
		
		h = scrapVars.scrapHandle;
	}
	
	long result = noTypeErr;
	
	Ptr p = *h;
	
	Ptr end = p + scrapVars.scrapSize;
	
	while ( p < end )
	{
		UInt32* p32 = (UInt32*) p;
		
		ResType entry_type = *p32++;
		long    entry_size = *p32++;
		
		p = (Ptr) p32;
		
		if ( entry_type == type )
		{
			*offset = p - *h;
			
			if ( dst )
			{
				SetHandleSize( dst, entry_size );
				
				if ( (result = MemErr) )
				{
					break;
				}
				
				BlockMoveData( *h + *offset, *dst, entry_size );
			}
			
			result = entry_size;
			
			break;
		}
		
		entry_size += entry_size & 0x1;
		
		p += entry_size;
	}
	
	if ( prev_state == 0 )
	{
		UnloadScrap();
	}
	
	return result;
}
