/*
	Pointers.cc
	-----------
*/

#include "Pointers.hh"

// Standard C
#include <stdlib.h>

// ams-common
#include "callouts.hh"

// ams-core
#include "options.hh"
#include "Zones.hh"


typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

THz   TheZone : 0x0118;
short MemErr  : 0x0220;
THz   SysZone : 0x02A6;

enum
{
	kClearFlagMask = 0x0200,
	kSysFlagMask   = 0x0400,
};

enum
{
	noErr      = 0,
	paramErr   = -50,
	memFullErr = -108,
};

enum tag_byte
{
	Tag_free   = 0x00,
	Tag_ptr    = 0x40,
	Tag_handle = 0x80,
};

struct block_header
{
	uint8_t   tag;
	uint8_t   size_high_byte;
	uint16_t  size_low_word;
	uint32_t  misc;
};

bool native_alloc;


static
char* NewPtr_handler( unsigned long size : __D0, short trap_word : __D1 )
{
	MemErr = memFullErr;
	
	if ( size > 4 * 1024 * 1024 )
	{
		return NULL;  // 4 MiB is already out of the question, for now
	}
	
	char* alloc;
	
	if ( const THz zone = (trap_word & kSysFlagMask) ? SysZone : TheZone )
	{
		if (( alloc = zone_alloc_nonrel( zone, size ) ))
		{
			goto coda;
		}
		
		return NULL;
	}
	
	const unsigned long extended_size = size < 12 ? 12 : (size + 3) & ~0x3;
	const unsigned long physical_size = sizeof (block_header) + extended_size;
	
	native_alloc = true;  // Set native_alloc in case malloc() calls NewPtr()
	
	alloc = (char*) malloc( physical_size );
	
	native_alloc = false;
	
	if ( alloc == NULL )
	{
		return NULL;
	}
	
	block_header& header = *(block_header*) alloc;
	
	*(uint32_t*) &header.tag = physical_size;
	
	header.tag = Tag_ptr | extended_size - size;
	header.misc = 0;
	
	alloc += sizeof (block_header);
	
coda:
	
	if ( trap_word & kClearFlagMask  ||  autoclear )
	{
		fast_memset( alloc, '\0', size );
	}
	else
	{
		fast_memset( alloc, '\x75', size );
	}
	
	MemErr = noErr;
	
	return alloc;
}

static
short DisposePtr_handler( char* alloc : __A0 )
{
	if ( alloc == NULL )
	{
		return MemErr = noErr;
	}
	
	block_header& header = *(block_header*) (alloc - sizeof (block_header));
	
	if ( (header.tag & 0xC0) != Tag_ptr )
	{
		// Invalid block type -- possibly due to a double-free error
		return MemErr = paramErr;
	}
	
	if ( zone_free_nonrel( alloc ) )
	{
		return MemErr = noErr;
	}
	
	alloc -= sizeof (block_header);
	
	native_alloc = true;  // Set native_alloc in case free() calls DisposePtr()
	
	free( alloc );
	
	native_alloc = false;
	
	return MemErr = noErr;
}

asm
char* NewPtr_patch( unsigned long size : __D0, short trap_word : __D1 )
{
	TST.B    native_alloc
	BEQ.S    not_native
	
	JMP      0xFFFFFFE6  // alloc
	
not_native:
	JSR      NewPtr_handler
	MOVE.W   MemErr,D0
	RTS
}

asm
short DisposePtr_patch( char* p : __A0 )
{
	TST.B    native_alloc
	BEQ.S    not_native
	
	JMP      0xFFFFFFE4  // dealloc
	
not_native:
	JMP      DisposePtr_handler
}

long GetPtrSize_patch( char* p : __A0 )
{
	block_header& header = *(block_header*) (p - sizeof (block_header));
	
	const long physical_size = header.size_high_byte << 16
	                         | header.size_low_word;
	
	return physical_size - sizeof (block_header) - (header.tag & 0xF);
}

short SetPtrSize_patch( char* p : __A0, long size : __D0 )
{
	long current_size = GetPtrSize_patch( p );
	
	short err = current_size < 0    ? current_size
	          : current_size < size ? memFullErr
	          :                       noErr;
	
	return MemErr = err;
}
