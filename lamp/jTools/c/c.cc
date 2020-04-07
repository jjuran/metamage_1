/*
	c.cc
	----
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>

// Standard C++
#include <algorithm>

// command
#include "command/get_option.hh"

// chars
#include "charsets/MacRoman.hh"
#include "encoding/utf8.hh"

// poseven
#include "poseven/functions/execvp.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/mmap.hh"
#include "poseven/functions/munmap.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_force    = 'f',
	Option_macRoman = 'm',
};

static command::option options[] =
{
	{ "",    Option_force    },
	{ "mac", Option_macRoman },
	
	{ NULL }
};

static bool macRoman_output = false;
static bool forced = false;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_force:
				forced = true;
				break;
			
			case Option_macRoman:
				macRoman_output = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	const off_t max_file_size = 30000;
	
	
	struct chars_info
	{
		unsigned n_ascii;
		unsigned n_CR;
		unsigned n_LF;
		unsigned n_CRLF;
		unsigned n_ctrl;      // not counting TAB, LF, CR, space
		unsigned n_utf8;      // characters, not bytes
		unsigned n_bad_utf8;  // bytes that are not UTF-8
		unsigned n_non_mac;   // Unicode characters not in MacRoman
	};
	
	static chars_info get_chars_info( const char* begin, const char* end )
	{
		chars_info result = { 0 };
		
		unsigned char last_char = 0;
		
		while ( begin < end )
		{
			const unsigned char c = *begin;
			
			if ( c < 0x80 )
			{
				++result.n_ascii;
				
				if ( c == 0x0D )
				{
					++result.n_CR;
				}
				else if ( c == 0x0A )
				{
					if ( last_char == 0x0D )
					{
						--result.n_CR;
						++result.n_CRLF;
					}
					else
					{
						++result.n_LF;
					}
				}
				else if ( (c < 0x20  &&  c != 0x09)  ||  c == 0x7F )
				{
					++result.n_ctrl;
				}
				
				last_char = c;
			}
			else
			{
				chars::unichar_t uc = chars::get_next_code_point_from_utf8( begin, end );
				
				if ( !~uc )
				{
					++result.n_bad_utf8;
				}
				else
				{
					++result.n_utf8;
					
					if ( !chars::extended_ascii_from_unicode( uc, chars::MacRoman_encoder_map ) )
					{
						++result.n_non_mac;
					}
					
					continue;  // don't reincrement begin
				}
			}
			
			++begin;
		}
		
		return result;
	}
	
	int Main( int argc, char** argv )
	{
		// Check for sufficient number of args
		if ( argc <= 1 )
		{
			return 0;
		}
		
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		// Print each file in turn.  Return whether any errors occurred.
		int exit_status = 0;
		
		while ( *args != NULL )
		{
			const char* path = *args++;
			
			try
			{
				n::owned< p7::fd_t > fd = p7::open( path, p7::o_rdonly );
				
				struct stat sb = p7::fstat( fd );
				
				if ( sb.st_size == 0 )
				{
					continue;
				}
				
				if ( sb.st_size > max_file_size )
				{
					throw p7::errno_t( EFBIG );
				}
				
				n::owned< p7::mmap_t > m = p7::mmap( sb.st_size,
				                                     p7::prot_read | p7::prot_write,
				                                     p7::map_private,
				                                     fd );
				
				char* begin = (char*) m.get().addr;
				
				char* end = begin + sb.st_size;
				
				if ( sb.st_size >= 2 )
				{
					// mmap() result is always word-aligned
					const chars::unichar_t bom = *(unsigned short*) begin;
					
					const char* utf16_error = bom == 0xFEFF ? "UTF-16 (native-endian) not supported"
					                        : bom == 0xFFFE ? "UTF-16 (cross-endian) not supported"
					                        :                 NULL;
					
					if ( utf16_error )
					{
						p7::perror( "error", path, utf16_error );
						
						exit_status = 1;
						
						continue;
					}
				}
				
				chars_info info = get_chars_info( begin, end );
				
				if ( info.n_ctrl  &&  !forced )
				{
					p7::perror( "error", path, "contains control characters" );
					
					exit_status = 1;
					
					continue;
				}
				
				const unsigned n_newline_conventions = info.n_CR   != 0
				                                     + info.n_LF   != 0
				                                     + info.n_CRLF != 0;
				
				if ( n_newline_conventions > 1 )
				{
					p7::perror( "warning", path, "contains inconsistent newlines" );
				}
				
				const bool is_ascii = info.n_ascii == sb.st_size;
				
				const bool is_MacRoman = info.n_bad_utf8 != 0;
				
				const bool is_utf8 = !is_ascii  &&  !is_MacRoman;
				
				if ( is_utf8  &&  info.n_non_mac )
				{
					p7::perror( "warning", path, "contains Unicode characters absent in MacRoman" );
				}
				
				if ( is_MacRoman != macRoman_output )
				{
					int pipe_ends[ 2 ];
					
					p7::throw_posix_result( pipe( pipe_ends ) );
					
					p7::pid_t pid = POSEVEN_VFORK();
					
					if ( pid == 0 )
					{
						dup2( pipe_ends[0], p7::stdin_fileno );
						
						close( pipe_ends[0] );
						close( pipe_ends[1] );
						
						const char* filter = macRoman_output ? "utf82mac"
						                                     : "mac2utf8";
						
						const char* args[] = { filter, NULL };
						
						p7::execvp( args );
					}
					
					// FIXME:  Deal with CRLF newlines
					std::replace( begin, end, '\r', '\n' );
					
					const p7::fd_t writer = p7::fd_t( pipe_ends[1] );
					
					p7::write( writer, begin, sb.st_size );
					
					close( pipe_ends[0] );
					close( pipe_ends[1] );
					
					p7::wait();
				}
				else
				{
					p7::write( p7::stdout_fileno, begin, sb.st_size );
				}
			}
			catch ( const p7::errno_t& error )
			{
				p7::perror( "c", path, error );
				
				exit_status = 1;
				
				continue;
			}
		}
		
		return exit_status;
	}
	
}
