/*
	rgn2pict.cc
	-----------
*/

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// rgn
#include "rgn/rgn.hh"


#pragma exceptions off


#define PROGRAM  "rgn2pict"

#define MISSING_INPUT   "input file required"

#define STR_LEN( s )  "" s, (sizeof s - 1)


typedef rgn::header pic_header;

static inline
void report_error( const char* path, unsigned err )
{
	more::perror( PROGRAM, path, err );
}

static inline
const pic_header* picture_from_region( const char* path )
{
	enum
	{
		size_pre_region = 10 + 1 +  1
		                     + 1 + 10
		                     + 1,
		
		size_post_region = 1,
	};
	
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		report_error( path, errno );
		
		return NULL;
	}
	
	const pic_header* picture = NULL;
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	size_t file_size = st.st_size;
	size_t pict_size = size_pre_region + file_size + size_post_region;
	
	if ( nok  ||  (! S_ISREG( st.st_mode )  &&  (errno = ESPIPE)) )
	{
		report_error( path, errno );
	}
	else if ( void* buffer = malloc( pict_size ) )
	{
		void* region_buffer = (char*) buffer + size_pre_region;
		
		ssize_t n = read( fd, region_buffer, file_size );
		
		const rgn::header& rgn = *(const rgn::header*) region_buffer;
		
		if ( n < 0 )
		{
			report_error( path, errno );
		}
		else if ( n != file_size )
		{
			report_error( path, ENOSPC );
		}
		else if ( ! valid( rgn, file_size ) )
		{
			report_error( path, EINVAL );
		}
		else
		{
			size_t rgn_size = rgn.size;
			
			pic_header& pic = *(pic_header*) buffer;
			
			pic.size = pict_size - (file_size - rgn_size);
			
			pic.bbox.top  = 0;
			pic.bbox.left = 0;
			
			pic.bbox.bottom = rgn.bbox.bottom + rgn.bbox.top;
			pic.bbox.right  = rgn.bbox.right + rgn.bbox.left;
			
			char* p = (char*) buffer + 10;
			
			p = (char*) mempcpy( p, STR_LEN( "\x11" "\x01"
			                                 "\x01" "\x00\x0a"
			                                        "\x80\x01"
			                                        "\x80\x01"
			                                        "\x7F\xFF"
			                                        "\x7F\xFF"
			                                 "\x81" ) );
			
			p += rgn_size;
			
			*p = '\xFF';
			
			picture = &pic;
			
			buffer = NULL;
		}
		
		free( buffer );
	}
	else
	{
		report_error( "malloc", errno );
	}
	
	close( fd );
	
	return picture;
}

int main( int argc, char** argv )
{
	const char* output_path = NULL;
	
	char** args = argv + 1;
	
	const char* src = *args;
	
	if ( src == NULL )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_INPUT "\n" ) );
		return 2;
	}
	
	if ( argc >= 3  &&  strcmp( *args, "-o" ) == 0 )
	{
		++args;
		output_path = *args++;
		
		src = *args;
	}
	else
	{
		size_t len = strlen( src );
		
		void* p = alloca( len + sizeof ".PICT" );
		
		output_path = (char*) p;
		
		p = mempcpy( p, src,     len            );
		p = mempcpy( p, ".PICT", sizeof ".PICT" );  // includes final NUL byte
	}
	
	const pic_header* pic = picture_from_region( src );
	
	if ( ! pic )
	{
		report_error( "picture_from_region", errno );
		return 1;
	}
	
	size_t size = pic->size;
	
	ssize_t wrote = 0;
	
	int write_errnum = 0;
	
	int closed = 0;
	
	int fd = open( output_path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( fd >= 0 )
	{
		wrote = write( fd, pic, size );
		
		write_errnum = errno;
		
		closed = close( fd );
	}
	
	free( (void*) pic );
	
	int errnum = fd < 0        ? errno
	           : wrote < 0     ? write_errnum
	           : wrote != size ? ENOSPC
	           : closed < 0    ? errno
	           :                 0;
	
	if ( errnum )
	{
		report_error( output_path, errnum );
		
		return 1;
	}
	
	return 0;
}
