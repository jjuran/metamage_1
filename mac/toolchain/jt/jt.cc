/*
	jt.cc
	-----
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>

// iota
#include "iota/endian.hh"


typedef iota::big_endian_conversion< uint16_t >::type U_16;
typedef iota::big_endian_conversion< uint32_t >::type U_32;

struct jump_table_header
{
	U_32  space_above_A5;
	U_32  space_below_A5;
	U_32  jump_table_size;
	U_32  jump_table_offset;
};

struct jump_table_entry
{
	U_16  offset;
	U_16  opcode;
	U_16  segnum;
	U_16  A_trap;
};

static
int display_one_entry( int fd, unsigned entry_offset )
{
	jump_table_entry entry;
	
	ssize_t n = read( fd, &entry, sizeof entry );
	
	if ( n == 0 )
	{
		return -1;
	}
	
	if ( n < 0 )
	{
		perror( "read" );
		return 1;
	}
	
	if ( n != sizeof entry )
	{
		puts( "short read" );
		return 2;
	}
	
	uint16_t offset = entry.offset;
	uint16_t opcode = entry.opcode;
	uint16_t segnum = entry.segnum;
	uint16_t A_trap = entry.A_trap;
	
	if ( opcode != 0x3f3c )
	{
		fprintf( stderr, "Warning:  Opcode doesn't match expected MOVE.W\n" );
	}
	
	if ( A_trap != 0xa9f0 )
	{
		fprintf( stderr, "Warning:  A-trap doesn't match expected _LoadSeg\n" );
	}
	
	printf( "$%.4x in 'CODE' %u: (%u,A5)\n", offset + 4, segnum, entry_offset );
	
	return 0;
}

static
int display_jump_table( int fd )
{
	jump_table_header header;
	
	ssize_t n = read( fd, &header, sizeof header );
	
	if ( n < 0 )
	{
		perror( "read" );
		return 1;
	}
	
	if ( n != sizeof header )
	{
		puts( "short read" );
		return 2;
	}
	
	unsigned long above  = header.space_above_A5;
	unsigned long below  = header.space_below_A5;
	unsigned long size   = header.jump_table_size;
	unsigned long offset = header.jump_table_offset;
	
	if ( offset != 32 )
	{
		fprintf( stderr, "Warning:  Jump table offset is not 32 bytes\n" );
	}
	
	if ( above != offset + size )
	{
		fprintf( stderr, "Warning:  Above_A5 != offset + size\n" );
	}
	
	printf( "Above A5: %lu\n",  above  );
	printf( "Below A5: %lu\n",  below  );
	printf( "JT size: %lu\n",   size   );
	printf( "JT offset: %lu\n", offset );
	
	unsigned entry_offset = offset + 2;
	
	while ( display_one_entry( fd, entry_offset ) == 0 )
	{
		entry_offset += 8;
	}
	
	return 0;
}

static
int display_jump_table( const char* path )
{
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		perror( path );
		return 1;
	}
	
	int status = display_jump_table( fd );
	
	close( fd );
	
	return status;
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		const char* path = argv[ 1 ];
		
		return display_jump_table( path );
	}
	
	return 0;
}
