/*	=========
 *	buffer.cc
 *	=========
 */

// Standard C++
#include <functional>

// Standard C/C++
#include <cstddef>
#include <cstdio>

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// Lamp
#include "lamp/winio.h"

// POSeven
#include "POSeven/extras/pump.hh"
#include "POSeven/functions/execvp.hh"
#include "POSeven/functions/ioctl.hh"
#include "POSeven/functions/lseek.hh"
#include "POSeven/functions/vfork.hh"
#include "POSeven/functions/wait.hh"
#include "POSeven/functions/write.hh"
#include "POSeven/Open.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace O = Orion;
	
	
	static p7::fd_t global_window_fd = p7::stdin_fileno;
	
	
	class invalid_point {};
	
	
	static Point read_point( const char* string, const char* format )
	{
		int h, v;
		
		int scanned = std::sscanf( string, format, &h, &v );
		
		if ( scanned != 2 )
		{
			throw invalid_point();
		}
		
		Point position;
		
		position.h = h;
		position.v = v;
		
		return position;
	}
	
	inline Point read_position( const char* string )
	{
		return read_point( string, "%d,%d" );
	}
	
	inline Point read_size( const char* string )
	{
		return read_point( string, "%dx%d" );
	}
	
	
	// ioctl() wrappers
	
	inline void set_window_title( p7::fd_t window, const char* title )
	{
		p7::ioctl( window, WIOCSTITLE, title );
	}
	
	inline void set_window_position( p7::fd_t window, Point position )
	{
		p7::ioctl( window, WIOCSPOS, &position );
	}
	
	inline void set_window_size( p7::fd_t window, Point size )
	{
		p7::ioctl( window, WIOCSSIZE, &size );
	}
	
	inline void set_window_visibility( p7::fd_t window, int visibility )
	{
		p7::ioctl( window, WIOCSVIS, &visibility );
	}
	
	
	// option handlers
	
	static void set_position( const char* position_string )
	{
		Point position = read_position( position_string );
		
		set_window_position( global_window_fd, position );
	}
	
	static void set_size( const char* size_string )
	{
		Point size = read_size( size_string );
		
		set_window_size( global_window_fd, size );
	}
	
	static void set_visibility( const char* visibility_string )
	{
		int visibility = std::atoi( visibility_string );
		
		set_window_visibility( global_window_fd, visibility );
	}
	
	static void show( const char* )
	{
		set_window_visibility( global_window_fd, true );
	}
	
	static void hide( const char* )
	{
		set_window_visibility( global_window_fd, false );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		/*
		O::BindOptionTrigger( "--pos", std::ptr_fun( set_position ) );
		
		O::BindOptionTrigger( "--size", std::ptr_fun( set_size ) );
		
		O::BindOptionTrigger( "--vis", std::ptr_fun( set_visibility ) );
		
		O::BindOptionTrigger( "--show", std::ptr_fun( show ) );
		O::BindOptionTrigger( "--hide", std::ptr_fun( hide ) );
		*/
		
		bool should_wait = false;
		
		bool in_place = false;
		
		const char* title = NULL;
		
		const char* output_file = "/dev/null";
		
		const char* device = "/dev/new/buffer";
		
		O::BindOption( "-d", device      );
		O::BindOption( "-o", output_file );
		O::BindOption( "-t", title       );
		O::BindOption( "-w", should_wait );
		
		O::BindOption( "--in-place", in_place );
		
		O::AliasOption( "-d", "--dev"   );
		O::AliasOption( "-o", "--out"   );
		O::AliasOption( "-t", "--title" );
		O::AliasOption( "-w", "--wait"  );
		
		O::GetOptions( argc, argv );
		
		char const *const *freeArgs = O::FreeArguments();
		
		if ( *freeArgs == NULL )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: buffer input-file\n" ) );
			
			return 1;
		}
		
		const char* input_file = freeArgs[ 0 ];
		
		if ( input_file[0] == '-' && input_file[1] == '\0' )
		{
			input_file = "/dev/fd/0";
		}
		
		if ( in_place )
		{
			output_file = input_file;
		}
		
		if ( title == NULL )
		{
			title = input_file;
		}
		
		NN::Owned< p7::fd_t > input = p7::open( input_file, p7::o_rdonly );
		
		NN::Owned< p7::fd_t > window = p7::open( device, p7::o_rdwr );
		
		if ( title != NULL )
		{
			set_window_title( window, title );
		}
		
		p7::pump( input, window );
		
		p7::close( input );
		
		p7::lseek( window, 0 );
		
		NN::Owned< p7::fd_t > output = p7::open( output_file, p7::o_wronly | p7::o_creat | p7::o_trunc_lazy, 0666 );
		
		p7::pid_t pid = p7::vfork();
		
		if ( pid == 0 )
		{
			// New child, so we're not a process group leader
			
			setsid();
			
			p7::ioctl( window, TIOCSCTTY, NULL );
			
			dup2( window, 0 );
			dup2( output, 1 );
			
			const char* window_argv[] = { "/bin/cat", NULL };
			
			p7::execvp( window_argv );
		}
		
		if ( should_wait )
		{
			return NN::Convert< p7::exit_t >( p7::wait() );
		}
		
		return p7::exit_success;
	}
	
}

