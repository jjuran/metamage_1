/*
	pntg2skif.cc
	------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// quickdraw
#include "qd/pack_bits.hh"

// rasterlib
#include "raster/raster.hh"
#include "raster/skif.hh"


#pragma exceptions off


#define PROGRAM  "pntg2skif"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


typedef unsigned char Byte;

static inline
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
void pntg2skif( const char* buffer, size_t n, const char* path )
{
	const Byte* p = (const Byte*) buffer;
	
	p += 512;
	
	using raster::raster_desc;
	using raster::raster_model;
	using raster::Model_monochrome_paint;
	
	const size_t width  = 576;
	const size_t height = 720;
	const size_t weight = 1;
	const size_t stride = width * weight / 8;
	
	const raster_model model = Model_monochrome_paint;
	
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
	};
	
	int fd = open( path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( fd < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	ssize_t n_written;
	
	for ( int i = 0;  i < 720;  ++i )
	{
		Byte buffer[ 72 ];
		
		Byte* dst = buffer;
		
		quickdraw::unpack_bits( p, dst, sizeof buffer );
		
		n_written = write( fd, buffer, sizeof buffer );
		
		if ( n_written < 0 )
		{
			report_error( path, errno );
			exit( 1 );
		}
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
	
	char buffer[ 53072 ];
	
	ssize_t n_read = read( fd, buffer, sizeof buffer );
	
	if ( n_read < 0 )
	{
		report_error( src, errno );
		return 1;
	}
	
	if ( n_read < 1952 )
	{
		WARN( "input file is too small" );
		return 1;
	}
	
	pntg2skif( buffer, n_read, output_path );
	
	close( fd );
	
	return 0;
}
