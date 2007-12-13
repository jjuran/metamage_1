/*	============
 *	setwindow.cc
 *	============
 */

// Standard C++
#include <functional>

// Standard C/C++
#include <cstddef>
#include <cstdio>

// POSIX
#include <sys/ioctl.h>

// Iota
#include "iota/strings.hh"

// Lamp
#include "lamp/winio.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/FileDescriptor.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace poseven
{
	
	inline void ioctl( fd_t fd, int command, int* argp )
	{
		throw_posix_result( ::ioctl( fd, command, argp ) );
	}
	
}

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

inline std::string get_window_title( p7::fd_t window )
{
	char title[ 256 ];
	
	p7::ioctl( window, WIOCGTITLE, (int*) title );
	
	return title;
}

inline void set_window_title( p7::fd_t window, const char* title )
{
	p7::ioctl( window, WIOCSTITLE, (int*) title );
}

inline Point get_window_position( p7::fd_t window )
{
	Point position;
	
	p7::ioctl( window, WIOCGPOS, (int*) &position );
	
	return position;
}

inline void set_window_position( p7::fd_t window, Point position )
{
	p7::ioctl( window, WIOCSPOS, (int*) &position );
}

inline Point get_window_size( p7::fd_t window )
{
	Point size;
	
	p7::ioctl( window, WIOCGSIZE, (int*) &size );
	
	return size;
}

inline void set_window_size( p7::fd_t window, Point size )
{
	p7::ioctl( window, WIOCSSIZE, (int*) &size );
}

inline bool get_window_visibility( p7::fd_t window )
{
	int visibility;
	
	p7::ioctl( window, WIOCGVIS, &visibility );
	
	return visibility;
}

inline void set_window_visibility( p7::fd_t window, int visibility )
{
	p7::ioctl( window, WIOCSVIS, &visibility );
}


// option handlers

static void get_title( const char* )
{
	std::string result = get_window_title( global_window_fd );
	
	result += "\n";
	
	p7::write( p7::stdout_fileno, result.data(), result.size() );
}

static void set_title( const char* title )
{
	set_window_title( global_window_fd, title );
}

static void get_position( const char* )
{
	Point position = get_window_position( global_window_fd );
	
	std::printf( "%d,%d\n", position.h, position.v );
}

static void set_position( const char* position_string )
{
	Point position = read_position( position_string );
	
	set_window_position( global_window_fd, position );
}

static void move( const char* movement_string )
{
	int dx, dy;
	
	int scanned = std::sscanf( movement_string, "%d,%d", &dx, &dy );
	
	if ( scanned == 2 )
	{
		Point position = get_window_position( global_window_fd );
		
		position.h += dx;
		position.v += dy;
		
		set_window_position( global_window_fd, position );
	}
}

static void get_size( const char* )
{
	Point size = get_window_size( global_window_fd );
	
	std::printf( "%dx%d\n", size.h, size.v );
}

static void set_size( const char* size_string )
{
	Point size = read_size( size_string );
	
	set_window_size( global_window_fd, size );
}

static void get_visibility( const char* )
{
	int visibility = get_window_visibility( global_window_fd );
	
	std::printf( "%d\n", visibility );
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


int O::Main( int argc, char const *const argv[] )
{
	O::BindOptionTrigger( "--get-title", std::ptr_fun( get_title ) );
	O::BindOptionTrigger( "--title",     std::ptr_fun( set_title ) );
	
	O::BindOptionTrigger( "--get-pos", std::ptr_fun( get_position ) );
	O::BindOptionTrigger( "--pos",     std::ptr_fun( set_position ) );
	
	O::BindOptionTrigger( "--move", std::ptr_fun( move ) );
	
	O::BindOptionTrigger( "--get-size", std::ptr_fun( get_size ) );
	O::BindOptionTrigger( "--size",     std::ptr_fun( set_size ) );
	
	//O::BindOptionTrigger( "--get-dim", std::ptr_fun( get_dimensions ) );
	//O::BindOptionTrigger( "--dim", std::ptr_fun( set_dimensions ) );
	
	O::BindOptionTrigger( "--get-vis", std::ptr_fun( get_visibility ) );
	O::BindOptionTrigger( "--vis",     std::ptr_fun( set_visibility ) );
	
	O::BindOptionTrigger( "--show", std::ptr_fun( show ) );
	O::BindOptionTrigger( "--hide", std::ptr_fun( hide ) );
	
	//O::AliasOption( "--title", "-t" );
	
	O::GetOptions( argc, argv );
	
	char const *const *freeArgs = O::FreeArguments();
	
	if ( *freeArgs != NULL )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "setwindow: extra arguments ignored\n" ) );
	}
	
	return 0;
}

