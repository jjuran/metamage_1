/*
	Handles.cc
	----------
*/

#include "Handles.hh"

// Standard C
#include <stdlib.h>

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "master_pointer.hh"

// ams-core
#include "options.hh"
#include "Pointers.hh"


enum
{
	kHandleIsResourceMask = 0x20,
	kHandlePurgeableMask  = 0x40,
	kHandleLockedMask     = 0x80
};

enum
{
	kClearFlagMask = 0x0200,
};

const short noErr        = 0;
const short paramErr     = -50;
const short memFullErr   = -108;
const short nilHandleErr = -109;
const short memSCErr     = -116;

short MemErr : 0x0220;


static inline
void free_with_native_alloc( void* p )
{
	native_alloc = true;
	
	free( p );
	
	native_alloc = false;
}

#define free(p) free_with_native_alloc(p)


const unsigned long Handle_prologue = 0xC7C7C7C7;  // left guillemots
const unsigned long Handle_epilogue = 0xC8C8C8C8;  // right guillemots

struct Handle_header
{
	long  size;
	long  capacity;
	
	master_pointer* backlink;
	
	unsigned long  prologue;
};

struct Handle_footer
{
	unsigned long  epilogue;
};

static inline
bool valid( const Handle_header& header, const Handle_footer& footer )
{
	return header.prologue == Handle_prologue  &&
	       footer.epilogue == Handle_epilogue;
}


static inline
unsigned long padded( size_t size )
{
	return size + 3 & ~3;
}

static inline
Handle_header* get_header( char* p )
{
	return (Handle_header*) p - 1;
}

static inline
Handle_footer* get_footer( Handle_header* header )
{
	return (Handle_footer*) ((char*) &header[ 1 ] + padded( header->size ));
}

static inline
void set_epilogue( Handle_header* header )
{
	get_footer( header )->epilogue = Handle_epilogue;
}

static inline
Handle_footer* get_footer( char* p )
{
	return get_footer( get_header( p ) );
}

static inline
short size_error( unsigned long size : __D0 )
{
	const unsigned extra_size = sizeof (Handle_header)   // 16 bytes
	                          + sizeof (Handle_footer);  //  4 bytes
	
	if ( size > 0x7FFFFFFF - extra_size )
	{
		return memSCErr;
	}
	
	return noErr;
}

static
Handle_header* allocate_Handle_mem( long   size      : __D0,
                                    short  trap_word : __D1 )
{
	const unsigned extra_size = sizeof (Handle_header)   // 16 bytes
	                          + sizeof (Handle_footer);  //  4 bytes
	
	const unsigned long padded_size = padded( size );
	
	if (( MemErr = size_error( size ) ))
	{
		return NULL;
	}
	
	const long full_size = padded_size + extra_size;
	
	native_alloc = true;
	
	void* alloc = malloc( full_size );
	
	native_alloc = false;
	
	if ( alloc == NULL )
	{
		MemErr = memFullErr;
		
		return NULL;
	}
	
	Handle_header* header = (Handle_header*) alloc;
	Handle_footer* footer = (Handle_footer*) ((char*) &header[1] + padded_size);
	
	if ( trap_word & kClearFlagMask  ||  autoclear )
	{
		fast_memset( &header[ 1 ], '\0', padded_size );
	}
	else
	{
		fast_memset( &header[ 1 ], '\x75', padded_size );
	}
	
	header->size     = size;
	header->capacity = padded_size;
	
	header->backlink = NULL;
	
	header->prologue = Handle_prologue;
	footer->epilogue = Handle_epilogue;
	
	return header;
}


static
char** new_empty_handle( short trap_word : __D1 )
{
	MemErr = memFullErr;
	
	native_alloc = true;
	
	master_pointer* mp = (master_pointer*) malloc( sizeof (master_pointer) );
	
	native_alloc = false;
	
	if ( mp != NULL )
	{
		MemErr = noErr;
		
		fast_memset( mp, '\0', sizeof (master_pointer) );
	}
	
	return (char**) mp;
}

static
char** new_handle( long size : __D0, short trap_word : __D1 )
{
	char** h = new_empty_handle( trap_word );
	
	if ( h != NULL )
	{
		ReallocateHandle_patch( h, size, trap_word );
		
		if ( *h != NULL )
		{
			return h;
		}
		
		free( h );
	}
	
	return NULL;
}

#pragma mark -
#pragma mark Allocating and Releasing Relocatable Blocks
#pragma mark -

asm char** NewHandle_patch( long size : __D0, short trap_word : __D1 )
{
	JSR      new_handle
	MOVE.W   MemErr,D0  // Includes the effect of TST.W D0
	
	RTS
}

short DisposeHandle_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "DisposeHandle: NULL address";
		
		return MemErr = nilHandleErr;
	}
	
	if ( *h != NULL )
	{
		Handle_header* header = get_header( *h );
		Handle_footer* footer = get_footer( *h );
		
		if ( ! valid( *header, *footer ) )
		{
			ERROR = "DisposeHandle: invalid handle state (double dispose?)";
			
			return MemErr = paramErr;
		}
		
		EmptyHandle_patch( h );
	}
	
	free( h );
	
	return MemErr = noErr;
}

long GetHandleSize_patch( char** h : __A0 )
{
	if ( h == NULL  ||  *h == NULL )
	{
		WARNING = h ? "GetHandleSize: empty handle"
		            : "GetHandleSize: NULL address";
		
		MemErr = nilHandleErr;
		
		return 0;
	}
	
	const Handle_header* header = get_header( *h );
	
	return header->size;
}

short SetHandleSize_patch( char**  h         : __A0,
                           long    size      : __D0,
                           short   trap_word : __D1 )
{
	if ( h == NULL  ||  *h == NULL )
	{
		WARNING = h ? "SetHandleSize: empty handle"
		            : "SetHandleSize: NULL address";
		
		return MemErr = nilHandleErr;
	}
	
	if ( size < 0 )
	{
		return MemErr = memSCErr;
	}
	
	Handle_header* header = get_header( *h );
	
	if ( size <= header->capacity )
	{
		header->size = size;
	}
	else
	{
		Handle_header* new_header = allocate_Handle_mem( size, trap_word );
		
		if ( new_header == NULL )
		{
			return MemErr = memFullErr;
		}
		
		fast_memcpy( new_header + 1, header + 1, header->size );
		
		new_header->backlink = header->backlink;
		
		*h = (char*) &new_header[1];
		
		free( header );
		
		header = new_header;
	}
	
	set_epilogue( header );
	
	return MemErr = noErr;
}

static
char** recover_handle( char* p : __A0 )
{
	if ( p == NULL )
	{
		WARNING = "RecoverHandle: NULL address";
		
		MemErr = paramErr;
		
		return 0;
	}
	
	Handle_header* header = get_header( p );
	Handle_footer* footer = get_footer( p );
	
	if ( header->prologue != Handle_prologue  ||  footer->epilogue != Handle_epilogue )
	{
		MemErr = paramErr;
		
		return 0;
	}
	
	return (char**) header->backlink;
}

asm
char** RecoverHandle_patch( char* p : __A0 )
{
	MOVE.L   D0,-(SP)
	
	JSR      recover_handle
	
	MOVE.L   (SP)+,D0
	RTS
}

short ReallocateHandle_patch( char**  h         : __A0,
                              long    size      : __D0,
                              short   trap_word : __D1 )
{
	if ( h == NULL )
	{
		WARNING = "ReallocateHandle: NULL address";
		
		return MemErr = nilHandleErr;
	}
	
	if ( short err = EmptyHandle_patch( h ) )
	{
		return err;
	}
	
	Handle_header* header = allocate_Handle_mem( size, trap_word );
	
	if ( header == NULL )
	{
		return MemErr;  // set in allocate_Handle_mem()
	}
	
	*h = (char*) &header[1];
	
	header->backlink = (master_pointer*) h;
	
	return MemErr = noErr;
}

#pragma mark -
#pragma mark Freeing Space in the Heap
#pragma mark -

short EmptyHandle_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "EmptyHandle: NULL address";
		
		return MemErr = nilHandleErr;
	}
	
	if ( *h != NULL )
	{
		Handle_header* header = get_header( *h );
		
		free( header );
		
		*h = NULL;
	}
	
	return MemErr = noErr;
}

#pragma mark -
#pragma mark Properties of Relocatable Blocks
#pragma mark -

short HLock_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "HLock: NULL address";
		
		return MemErr = paramErr;
	}
	
	((master_pointer*) h)->flags |= kHandleLockedMask;
	
	return MemErr = noErr;
}

short HUnlock_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "HUnlock: NULL address";
		
		return MemErr = paramErr;
	}
	
	((master_pointer*) h)->flags &= ~kHandleLockedMask;
	
	return MemErr = noErr;
}

short HPurge_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "HPurge: NULL address";
		
		return MemErr = paramErr;
	}
	
	((master_pointer*) h)->flags |= kHandlePurgeableMask;
	
	return MemErr = noErr;
}

short HNoPurge_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "HNoPurge: NULL address";
		
		return MemErr = paramErr;
	}
	
	((master_pointer*) h)->flags &= ~kHandlePurgeableMask;
	
	return MemErr = noErr;
}

short HSetRBit_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "HSetRBit: NULL address";
		
		return MemErr = paramErr;
	}
	
	((master_pointer*) h)->flags |= kHandleIsResourceMask;
	
	return MemErr = noErr;
}

short HClrRBit_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "HClrRBit: NULL address";
		
		return MemErr = paramErr;
	}
	
	((master_pointer*) h)->flags &= ~kHandleIsResourceMask;
	
	return MemErr = noErr;
}

signed char HGetState_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		WARNING = "HGetState: NULL address";
		
		MemErr = paramErr;
		
		return -1;
	}
	
	MemErr = noErr;
	
	return ((master_pointer*) h)->flags;
}

short HSetState_patch( char** h : __A0, signed char state : __D0 )
{
	if ( h == NULL )
	{
		WARNING = "HSetState: NULL address";
		
		return MemErr = paramErr;
	}
	
	((master_pointer*) h)->flags = state;
	
	return MemErr = noErr;
}

#pragma mark -
#pragma mark Advanced Routine
#pragma mark -

asm
char** NewEmptyHandle_patch( short trap_word : __D1 )
{
	JSR      new_empty_handle
	MOVE.W   MemErr,D0
	
	RTS
}
