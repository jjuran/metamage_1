/*
	mbin.cc
	-------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <string.h>

// chars
#include "conv/mac_utf8.hh"

// macbinary
#include "macbinary.hh"


#pragma exceptions off


#define ARRAY_LEN(a)  (sizeof (a) / sizeof (a)[0])

#define STR_LEN(s)  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( msg "\n" ) )


enum
{
	Success,
	Failure,
	
	FS_error = 30,
	IO_error = 40,
	
	Usage_error = 50,  // mnemonic:  Mac OS paramErr = -50
};

enum command_selector
{
	Cmd_none,
	Cmd_test,
	Cmd_stat,
	Cmd_name,
	Cmd_format,
};

static const char* command_names[] =
{
	"",
	"test",
	"stat",
	"name",
	"format",
};

static
command_selector lookup_command( const char* name )
{
	for ( int i = 0;  i < ARRAY_LEN( command_names );  ++i )
	{
		if ( strcmp( name, command_names[ i ] ) == 0 )
		{
			return command_selector( i );
		}
	}
	
	return Cmd_none;
}

int main( int argc, char** argv )
{
	if ( argc != 3 )
	{
		WARN( "usage: mbin test|stat|name|format file.mbin" );
		
		return Usage_error;
	}
	
	const char* subcommand = argv[ 1 ];
	const char* input_path = argv[ 2 ];
	
	command_selector command = lookup_command( subcommand );
	
	if ( ! command )
	{
		fprintf( stderr, "mbin: %s: unrecognized command\n", subcommand );
		return Usage_error;
	}
	
	int fd = open( input_path, O_RDONLY );
	
	if ( fd < 0 )
	{
		perror( input_path );
		return FS_error;
	}
	
	int buffer[ 512 / sizeof (int) ];  // ensure alignment
	
	ssize_t n_read = read( fd, buffer, sizeof buffer );
	
	if ( n_read < 0 )
	{
		perror( input_path );
		return IO_error;
	}
	
	if ( n_read >= sizeof (macbinary::header) )
	{
		const macbinary::header& h = *(macbinary::header*) buffer;
		
		if ( int8_t v = version( h ) )
		{
			const char* filename = command == Cmd_stat ? "Filename:  "
			                     : command == Cmd_name ? ""
			                     :                       NULL;
			
			const char* format = command == Cmd_stat   ? "Format:    MacBinary "
			                   : command == Cmd_format ? ""
			                   :                         NULL;
			
			const uint8_t* name = h.filename;
			
			uint8_t len = *name++;
			
			if ( filename )
			{
				char buffer[ 63 * 3 + 1 ];  // 190, max Str63 UTF-8 length + 1
				
				const size_t size = conv::utf8_from_mac( buffer,
				                                         sizeof buffer,
				                                         (const char*) name,
				                                         len );
				buffer[ size ] = '\0';
				
				printf( "%s%s\n", filename, buffer );
			}
			
			if ( format )
			{
				printf( "%s%d%s\n", format, v & 0x7f, &"+"[ v > 0 ] );
			}
			
			return Success;
		}
	}
	
	fprintf( stderr, "%s: not MacBinary\n", input_path );
	
	return Failure;
}
