/*
	rominfo.cc
	----------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>

// iota
#include "iota/strings.hh"

// gear
#include "gear/hexadecimal.hh"


#pragma exceptions off


struct rom_info
{
	unsigned checksum;
	unsigned fortytwo;
	unsigned char type;
	unsigned char version;
};

int main( int argc, char **argv )
{
	int rom_fd = open( "/sys/mac/rom", O_RDONLY );
	
	if ( rom_fd < 0 )
	{
		return 1;
	}
	
	struct stat rom_stat;
	
	int status = fstat( rom_fd, &rom_stat );
	
	if ( status < 0 )
	{
		return 2;
	}
	
	const size_t rom_size = rom_stat.st_size;
	
	void* rom_base = mmap( NULL, rom_size, PROT_READ, MAP_SHARED, rom_fd, 0 );
	
	const rom_info& info = *(rom_info*) rom_base;
	
	char base_hex[8];
	char size_hex[8];
	char checksum[8];
	char fortytwo[8];
	
	char mac_type[1] = { '0' + info.type };
	
	char rom_vers[2] =
	{
		gear::encoded_hex_char( info.version >> 4 ),
		gear::encoded_hex_char( info.version      )
	};
	
	gear::encode_32_bit_hex( (unsigned) rom_base, base_hex );
	gear::encode_32_bit_hex( (unsigned) rom_size, size_hex );
	gear::encode_32_bit_hex( info.checksum, checksum );
	gear::encode_32_bit_hex( info.fortytwo, fortytwo );
	
	struct iovec output[] =
	{
		{ (void*) STR_LEN(      "ROM base: " ) },
		{ base_hex, sizeof base_hex },
		{ (void*) STR_LEN( "\n" "ROM size: " ) },
		{ size_hex, sizeof size_hex },
		{ (void*) STR_LEN( "\n" "Checksum: " ) },
		{ checksum, sizeof checksum },
		{ (void*) STR_LEN( "\n" "often 2a: " ) },
		{ fortytwo, sizeof fortytwo },
		{ (void*) STR_LEN( "\n" "Mac type: " ) },
		{ mac_type, sizeof mac_type },
		{ (void*) STR_LEN( "\n" "ROM vers: " ) },
		{ rom_vers, sizeof rom_vers },
		{ (void*) STR_LEN( "\n" ) }
	};
	
	const int n_written = writev( STDOUT_FILENO, output, sizeof output / sizeof output[0] );
	
	return 0;
}
