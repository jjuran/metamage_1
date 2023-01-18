/*
	rom-checksum.cc
	---------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// more-posix
#include "more/perror.hh"

// iota
#include "iota/endian.hh"

// gear
#include "gear/hexadecimal.hh"


#pragma exceptions off


#define PROGRAM  "rom-checksum"

#define MISMATCH_PREFIX  "12345678 in file DOES NOT MATCH computed "

#define MESSAGE  "12345678\n"


int main( int argc, char** argv )
{
	int in = STDIN_FILENO;
	
	const char* rom_path = "<stdin>";
	
	if ( argc >= 1 + 1 )
	{
		rom_path = argv[ 1 ];
		
		in = open( rom_path, O_RDONLY );
		
		if ( in < 0 )
		{
			more::perror( PROGRAM, rom_path );
			
			return 2;
		}
	}
	
	struct stat st;
	
	int nok = fstat( in, &st );
	
	if ( nok )
	{
		more::perror( PROGRAM, "fstat" );
		
		return 2;
	}
	
	if ( !S_ISREG( st.st_mode ) )
	{
		more::perror( PROGRAM, rom_path, "not a regular file" );
		
		return 2;
	}
	
	const size_t rom_size = st.st_size;
	
	if ( rom_size == 0 )
	{
		more::perror( PROGRAM, rom_path, "empty file" );
		
		return 1;
	}
	
	if ( rom_size & 0xFFFF )
	{
		more::perror( PROGRAM, rom_path, "size is not a multiple of 64K" );
		
		return 1;
	}
	
	void* rom = mmap( NULL, rom_size, PROT_READ, MAP_PRIVATE, in, 0 );
	
	if ( rom == NULL )
	{
		more::perror( PROGRAM, "mmap" );
		
		return 2;
	}
	
	const uint32_t* p4 = (uint32_t*) rom;
	
	const uint32_t stored_sum = iota::u32_from_big( *p4++ );
	
	size_t main_segment_size = rom_size;
	
	if ( rom_size == 4 * 1024 * 1024 )
	{
		main_segment_size = 3 * 1024 * 1024;  // skip PPC extra megabyte
	}
	
	// Halve the size once for 16-bit pointer math, and again for midpoint.
	
	const uint16_t* p2  = (uint16_t*) p4;
	const uint16_t* mid = (uint16_t*) rom + main_segment_size / 2 / 2;
	const uint16_t* end = (uint16_t*) rom + main_segment_size / 2;
	
	uint32_t sum = 0;
	
	while ( p2 < mid )
	{
		sum += iota::u16_from_big( *p2++ );
	}
	
	// Check the sum at the halfway mark in case this is a Mac Classic ROM.
	
	if ( sum != stored_sum )
	{
		while ( p2 < end )
		{
			sum += iota::u16_from_big( *p2++ );
		}
	}
	
	char buffer[] = MISMATCH_PREFIX MESSAGE;
	
	char* one = buffer;
	char* two = buffer + sizeof MISMATCH_PREFIX - 1;
	
	gear::encode_32_bit_HEX( sum, two );
	
	char* message = two;
	
	int length = sizeof MESSAGE - 1;
	
	int exit_status;
	
	if (( exit_status = sum != stored_sum ))
	{
		gear::encode_32_bit_HEX( stored_sum, one );
		
		message = one;
		
		length = sizeof buffer - 1;
	}
	
	write( STDOUT_FILENO, message, length );
	
	return exit_status;
}
