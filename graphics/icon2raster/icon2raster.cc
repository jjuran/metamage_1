/*
	icon2raster.cc
	--------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/bit_iterator.hh"
#include "iota/endian.hh"

// more-posix
#include "more/perror.hh"

// raster
#include "raster/raster.hh"
#include "raster/skif.hh"


#define PROGRAM  "icon2raster"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


static inline
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
void icon2raster( const char* masked_icon, const char* path )
{
	using iota::bit_iterator;
	
	uint32_t buffer[ 32 * 32 ];
	
	uint32_t* p = buffer;
	
	bit_iterator face( masked_icon       );
	bit_iterator mask( masked_icon + 128 );
	
	const bit_iterator face_end = mask;
	
	while ( face != face_end )
	{
		/*
			For 'ICN#' data, OFF bits are white and ON bits are black.
		*/
		
		const uint32_t pixel = ! *mask ? 0x00000000
		                     : ! *face ? 0xFFFFFFFF
		                     :           0xFF000000;
		
		*p++ = pixel;
		
		++face;
		++mask;
	}
	
	using raster::raster_desc;
	using raster::raster_model;
	using raster::Model_RGB;
	
	const size_t width  = 32;
	const size_t height = 32;
	const size_t weight = 32;
	const size_t stride = width * weight / 8;
	
	const raster_model model = Model_RGB;
	
	const uint32_t image_size = height * stride;
	
	raster_desc desc =
	{
		raster::kSKIFFileType,
		0,
		width,
		height,
		stride,
		weight,
		model,
		0,
		0,
		{ raster::ARGB },
	};
	
	int fd = open( path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( fd < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	ssize_t n_written = write( fd, buffer, image_size );
	
	if ( n_written < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	n_written = write( fd, &desc, sizeof desc );
	
	if ( n_written < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	uint32_t footer_size = sizeof desc + sizeof (uint32_t) * 2;
	
	const uint32_t disk_block_size = 4096;
	const uint32_t k               = disk_block_size - 1;
	
	// Round up the total file size to a multiple of the disk block size.
	const uint32_t total_size = (image_size + footer_size + k) & ~k;
	
	footer_size = total_size - image_size;
	
	const off_t footer_addr = total_size - sizeof footer_size;
	
	const size_t code_len = sizeof (uint32_t);
	
	n_written = pwrite( fd, "SKIF", code_len, footer_addr - code_len );
	n_written = pwrite( fd, &footer_size, sizeof footer_size, footer_addr );
	
	if ( n_written < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	int nok = close( fd );
	
	if ( nok < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
}

int main( int argc, char** argv )
{
	const char* output_path = "/dev/fd/1";
	
	char** args = argv + 1;
	
	if ( argc >= 3  &&  strcmp( *args, "-o" ) == 0 )
	{
		++args;
		output_path = *args++;
	}
	else if ( isatty( STDOUT_FILENO ) )
	{
		WARN( "stdout is a tty, either redirect or use -o" );
		return 2;
	}
	
	const char* src = *args;
	
	if ( src == NULL )
	{
		WARN( "input file required" );
		return 2;
	}
	
	int fd = open( src, O_RDONLY );
	
	if ( fd < 0 )
	{
		report_error( src, errno );
		return 1;
	}
	
	char buffer[ 512 ];
	
	ssize_t n_read = read( fd, buffer, sizeof buffer );
	
	if ( n_read < 0 )
	{
		report_error( src, errno );
		return 1;
	}
	
	if ( n_read != 256 )
	{
		WARN( "input file size must be exactly 256 bytes" );
		return 1;
	}
	
	icon2raster( buffer, output_path );
	
	close( fd );
	
	return 0;
}
