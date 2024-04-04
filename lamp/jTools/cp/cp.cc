/*	=====
 *	cp.cc
 *	=====
 */

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// relix
#include "relix/copyfile.h"
#include "relix/pump.h"

// Orion
#include "Orion/Main.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


static
void report_error( const char* path, int errnum = errno )
{
	more::perror( "cp", path, errnum );
}

namespace tool
{

static const char* Basename( const char* pathname )
{
	const char* name_start = pathname;
	
	const char* p = pathname;
	
	while ( *p != '\0' )
	{
		if ( *p++ == '/' )
		{
			name_start = p;
		}
	}
	
	return name_start;
}

static int copyfile_or_pump( const char* src, const char* dest )
{
	int nok = ::copyfile( src, dest );
	
	if ( nok < 0  &&  (errno == EINVAL  ||  errno == EXDEV) )
	{
		int in = nok = open( src, O_RDONLY );
		
		if ( in >= 0 )
		{
			int out = nok = open( dest, O_WRONLY|O_CREAT, 0666 );
			
			if ( out >= 0 )
			{
				ssize_t pumped = pump( in, NULL, out, NULL, 0, 0 );
				
				pumped = pump( in, NULL, out, NULL, 0, 0 );
				
				nok = -(pumped != 0);
				
				close( out );
			}
			
			close( in );
		}
	}
	
	return nok;
}

int Main( int argc, char** argv )
{
	int fail = 0;
	
	// Check for sufficient number of args
	if ( argc < 3 )
	{
		if ( argc == 1 )
		{
			write( STDERR_FILENO, STR_LEN( "cp: missing file arguments\n" ) );
		}
		else
		{
			write( STDERR_FILENO, STR_LEN( "cp: missing destination file\n" ) );
		}
		
		return 1;
	}
	
	enum
	{
		max_path_len = 2047,
		
		path_buffer_len = max_path_len * 2 + 2,  // theoretical worst case
	};
	
	for ( int i = 1;  i < argc;  ++i )
	{
		if ( strlen( argv[ i ] ) > max_path_len )
		{
			report_error( argv[ i ], ENAMETOOLONG );
			
			return 1;
		}
	}
	
	struct stat sb;
	
	char path_buffer[ path_buffer_len ];
	
	if ( argc > 3 )
	{
		// Copy multiple files.
		
		// Last arg should be the destination directory.
		const char* destDir = argv[ argc - 1 ];
		
		int nok = stat( destDir, &sb );
		
		if ( nok < 0  &&  errno != ENOENT )
		{
			report_error( destDir );
			
			return 1;
		}
		
		bool is_dir = sb.st_mode & S_IFDIR;
		
		if ( ! is_dir )
		{
			#define COPYING  "cp: copying multiple files, but last argument ("
			#define NOT_DIR  ") is not a directory.\n"
			#define str_len( s )  s, strlen( s )
			
			write( STDERR_FILENO, STR_LEN( COPYING ) );
			write( STDERR_FILENO, str_len( destDir ) );
			write( STDERR_FILENO, STR_LEN( NOT_DIR ) );
			
			#undef COPYING
			#undef NOT_DIR
			#undef str_len
			
			return 1;
		}
		
		size_t destDir_len = strlen( destDir );
		
		char* p = (char*) mempcpy( path_buffer, destDir, destDir_len );
		
		if ( p[ -1 ] != '/' )
		{
			*p++ = '/';
		}
		
		// Try to copy each file.  Return whether any errors occurred.
		for ( size_t index = 1;  index < argc - 1;  ++index )
		{
			const char* sourcePath = argv[index];
			
			const char* base = Basename( sourcePath );
			
			size_t base_len = strlen( base );
			
			mempcpy( p, base, base_len + 1 );  // copy the trailing NUL
			
			int nok = copyfile_or_pump( sourcePath, path_buffer );
			
			if ( nok < 0 )
			{
				++fail;
			}
		}
	}
	else
	{
		// Copy single file.
		const char* sourcePath = argv[1];
		const char* destPath   = argv[2];
		
		int nok = lstat( sourcePath, &sb );
		
		if ( nok < 0 )
		{
			report_error( sourcePath );
			
			return 1;
		}
		
		nok = stat( destPath, &sb );
		
		if ( nok < 0  &&  errno != ENOENT )
		{
			report_error( destPath );
			
			return 1;
		}
		
		if ( nok == 0  &&  sb.st_mode & S_IFDIR )
		{
			/*
				The dest exists and is a directory.
				
				Copy this -> that/this
				set that = that/this
			*/
			
			size_t destPath_len = strlen( destPath );
			
			char* p = (char*) mempcpy( path_buffer, destPath, destPath_len );
			
			if ( p[ -1 ] != '/' )
			{
				*p++ = '/';
			}
			
			const char* base = Basename( sourcePath );
			
			size_t base_len = strlen( base );
			
			mempcpy( p, base, base_len + 1 );  // copy the trailing NUL
			
			destPath = path_buffer;
		}
		
		nok = copyfile_or_pump( sourcePath, destPath );
		
		if ( nok < 0 )
		{
			report_error( destPath );
			
			++fail;
		}
	}
	
	return fail;
}

}
