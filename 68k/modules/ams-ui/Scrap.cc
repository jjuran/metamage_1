/*
	Scrap.cc
	--------
*/

#include "Scrap.hh"

// Mac OS
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
	return noErr;
}

pascal long LoadScrap_patch()
{
	return noErr;
}

#pragma mark -
#pragma mark Writing to the Desk Scrap
#pragma mark -

pascal long ZeroScrap_patch()
{
	scrapVars.scrapSize = 0;
	
	if ( Handle h = scrapVars.scrapHandle )
	{
		SetHandleSize( h, 0 );
	}
	else
	{
		scrapVars.scrapHandle = NewHandle( 0 );
	}
	
	scrapVars.scrapCount++;
	scrapVars.scrapState = 1;
	
	return noErr;
}

pascal long PutScrap_patch( long length, ResType type, const char* src )
{
	Handle h = scrapVars.scrapHandle;
	
	if ( ! h )
	{
		return noScrapErr;
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
	
	return err;
}

#pragma mark -
#pragma mark Reading from the Desk Scrap
#pragma mark -

pascal long GetScrap_patch( char** dst, ResType type, long* offset )
{
	Handle h = scrapVars.scrapHandle;
	
	if ( ! h )
	{
		return noTypeErr;
	}
	
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
			
			return entry_size;
		}
		
		entry_size += entry_size & 0x1;
		
		p += entry_size;
	}
	
	return noTypeErr;
}
