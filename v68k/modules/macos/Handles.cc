/*
	Handles.cc
	----------
*/

#include "Handles.hh"

// Standard C
#include <stdlib.h>
#include <string.h>


const short paramErr     = -50;
const short memFullErr   = -108;
const short nilHandleErr = -109;
const short memSCErr     = -116;

short MemErr : 0x0220;


const unsigned long Handle_prologue = 0xC7C7C7C7;  // left guillemots
const unsigned long Handle_epilogue = 0xC8C8C8C8;  // right guillemots

struct master_pointer
{
	char*  alloc;
	long   flags;
};

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


static inline unsigned long padded_size( size_t size )
{
	return size + 3 & ~3;
}

static inline Handle_header* get_header( char* p )
{
	return (Handle_header*) p - 1;
}

static inline Handle_footer* get_footer( Handle_header* header )
{
	return (Handle_footer*) ((char*) &header[ 1 ] + padded_size( header->size ));
}

static inline void set_epilogue( Handle_header* header )
{
	get_footer( header )->epilogue = Handle_epilogue;
}

static inline Handle_footer* get_footer( char* p )
{
	return get_footer( get_header( p ) );
}

static Handle_header* allocate_Handle_mem( long size : __D0, short trap_word : __D1 ) : __A0
{
	const unsigned extra_size = sizeof (Handle_header)
	                          + sizeof (Handle_footer);  // 16 bytes
	
	const unsigned long size_with_padding = padded_size( size );
	
	if ( size < 0  ||  size_with_padding > 0x7FFFFFFF - extra_size )
	{
		MemErr = memSCErr;
		
		return NULL;
	}
	
	const long full_size = size_with_padding + extra_size;
	
	void* alloc = trap_word & 0x0200 ? calloc( 1, full_size )
	                                 : malloc(    full_size );
	
	if ( alloc == NULL )
	{
		MemErr = memFullErr;
		
		return NULL;
	}
	
	Handle_header* header = (Handle_header*) alloc;
	Handle_footer* footer = (Handle_footer*) ((char*) &header[1] + size_with_padding);
	
	header->size     = size;
	header->capacity = size_with_padding;
	
	header->backlink = NULL;
	
	header->prologue = Handle_prologue;
	footer->epilogue = Handle_epilogue;
	
	return header;
}


pascal char** NewHandle_patch( long size : __D0, short trap_word : __D1 ) : __A0
{
	char** h = NewEmptyHandle_patch();
	
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

/*pascal*/ char** NewEmptyHandle_patch() : __A0
{
	void* alloc = malloc( sizeof (master_pointer) );  // 8 bytes
	
	if ( alloc == NULL )
	{
		MemErr = memFullErr;
		
		return NULL;
	}
	
	master_pointer* h = (master_pointer*) alloc;
	
	h->alloc = NULL;
	h->flags = 0;
	
	return (char**) h;
}

pascal void DisposeHandle_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		MemErr = nilHandleErr;
		
		return;
	}
	
	if ( *h != NULL )
	{
		Handle_header* header = get_header( *h );
	
		free( header );
	}
	
	free( h );
}

pascal void SetHandleSize_patch( char** h : __A0, long size : __D0, short trap_word : __D1 )
{
	if ( h == NULL  ||  *h == NULL )
	{
		MemErr = nilHandleErr;
		
		return;
	}
	
	if ( size < 0 )
	{
		MemErr = memSCErr;
		
		return;
	}
	
	Handle_header* header = get_header( *h );
	
	if ( size <= header->capacity )
	{
		header->size = size;
	}
	else
	{
		if ( Handle_header* new_header = allocate_Handle_mem( size, trap_word ) )
		{
			memcpy( new_header + 1, header + 1, header->size );
			
			new_header->backlink = header->backlink;
			
			*h = (char*) &new_header[1];
			
			free( header );
			
			header = new_header;
		}
		else
		{
			MemErr = memFullErr;
			
			return;
		}
	}
	
	set_epilogue( header );
}

pascal long GetHandleSize_patch( char** h : __A0 ) : __D0
{
	if ( h == NULL  ||  *h == NULL )
	{
		MemErr = nilHandleErr;
		
		return 0;
	}
	
	const Handle_header* header = get_header( *h );
	
	return header->size;
}

pascal void ReallocateHandle_patch( char** h : __A0, long size : __D0, short trap_word : __D1 )
{
	if ( h == NULL )
	{
		MemErr = nilHandleErr;
		
		return;
	}
	
	if ( *h != NULL )
	{
		Handle_header* header = get_header( *h );
		
		free( header );
		
		*h = NULL;
	}
	
	Handle_header* header = allocate_Handle_mem( size, trap_word );
	
	if ( header == NULL )
	{
		return;
	}
	
	*h = (char*) &header[1];
	
	header->backlink = (master_pointer*) h;
}

pascal void EmptyHandle_patch( char** h : __A0 )
{
	if ( h == NULL )
	{
		MemErr = nilHandleErr;
		
		return;
	}
	
	if ( *h == NULL )
	{
		return;
	}
	
	Handle_header* header = get_header( *h );
	
	free( header );
	
	*h = NULL;
}
