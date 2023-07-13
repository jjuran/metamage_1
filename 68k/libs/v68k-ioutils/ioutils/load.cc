/*
	load.cc
	-------
*/

#include "ioutils/load.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// mac-glue-utils
#ifdef __MC68K__
#include "mac_glue/Gestalt.hh"
#endif

// v68k-alloc
#include "v68k-alloc/memory.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k    {
namespace ioutils {


#ifdef __MC68K__

asm
void* load( const char* path : __A0 )
{
	MOVEA.L  A0,A1     // copy A0
	
	// ENTER strlen0
loop:
	TST.B    (A1)+     // while ( *a1++ != '\0' )
	BNE.S    loop      //    continue;
	
	SUBA.L   A0,A1     // a1 -= a0;
	MOVE.L   A1,D0     // d0 = a1;
	// LEAVE strlen0
	
	JSR  0xFFFFFFFC    // the actual load callout
	
	MOVE.L   A0,D2     // if ( *a0 != NULL ) ;
	BNE.S    no_errno  // else
	MOVE.L   D1,errno  //     errno = d1;
no_errno:
	RTS
}

static
asm
void* load_with_size( const char* path : __A0, uint32_t* size )
{
	LINK     A6,#0
	
	JSR      load
	
	MOVEA.L  8(A6),A1
	MOVE.L   D0,(A1)
	
	UNLK     A6
	RTS
}

#endif

void* load_file( const char* path, uint32_t* size )
{
#ifdef __MC68K__
	
	if ( ! mac::glue::gestalt_err( 'v68k' ) )
	{
		return size ? load_with_size( path, size ) : load( path );
	}
	
#endif
	
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		return NULL;
	}
	
	void* alloc = NULL;
	
	struct stat sb;
	
	int err = fstat( fd, &sb );
	
	if ( err < 0 )
	{
		// fail, do nothing
	}
	else
	{
		const ssize_t file_size = sb.st_size;
		
		using v68k::alloc::page_size;
		
		const size_t alloc_size = (file_size + page_size - 1) & ~(page_size - 1);  // round up
		
		alloc = malloc( alloc_size );
		
		if ( alloc == NULL )
		{
			errno = ENOMEM;
		}
		else
		{
			const ssize_t n_read = read( fd, alloc, alloc_size );
			
			if ( n_read != file_size )
			{
				free( alloc );
				
				alloc = NULL;
				
				if ( n_read >= 0 )
				{
					errno = EIO;
				}
			}
			else
			{
				memset( (char*) alloc + n_read, '\0', alloc_size - n_read );
				
				if ( size != NULL )
				{
					*size = n_read;
				}
			}
		}
	}
	
	close( fd );
	
	return alloc;
}

}  // namespace ioutils
}  // namespace v68k
