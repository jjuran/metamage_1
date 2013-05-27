/*
	utils.cc
	--------
*/

#include "utils/load.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// v68k-alloc
#include "v68k-alloc/memory.hh"


#pragma exceptions off


namespace v68k  {
namespace utils {


void* load_file( const char* path, uint32_t* size )
{
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
		const size_t file_size = sb.st_size;
		
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

}  // namespace utils
}  // namespace v68k

