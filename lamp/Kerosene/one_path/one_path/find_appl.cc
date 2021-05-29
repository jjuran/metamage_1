/*
	find_appl.cc
	------------
	
	Joshua Juran
*/

#include "one_path/find_appl.hh"

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// iota
#include "iota/strings.hh"

// poseven
#include "poseven/functions/fdopendir.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/readlinkat.hh"
#include "poseven/sequences/directory_contents.hh"


namespace n = nucleus;
namespace p7 = poseven;


static const char list[]        = "list";
static const char sys_mac_vol[] = "/sys/mac/vol/";


plus::string find_appl( const char* appl )
{
	char ram_path[] = "ram/dt/appls/APPL/latest";
	
	memcpy( ram_path + STRLEN( "ram/dt/appls/" ), appl, STRLEN( "APPL" ) );
	
	const char* path = ram_path + STRLEN( "ram/" );
	
	n::owned< p7::fd_t > vol = p7::open( sys_mac_vol, p7::o_rdonly | p7::o_directory );
	
	try
	{
		return p7::readlinkat( vol, ram_path );
	}
	catch ( const p7::errno_t& err )
	{
		if ( err != ENOTDIR  &&  err != ENOENT  &&  err != EINVAL )
		{
			throw;
		}
	}
	
	n::owned< p7::fd_t > vol_list = p7::openat( vol, list, p7::o_rdonly | p7::o_directory );
	
	const p7::fd_t dir_fd = vol_list;
	
	n::owned< p7::dir_t > vol_list_dir = p7::fdopendir( vol_list );
	
	typedef p7::directory_contents_container directory_container;
	
	typedef directory_container::const_iterator const_iterator;
	
	directory_container contents = p7::directory_contents( vol_list_dir );
	
	const_iterator end = contents.end();
	
	for ( const_iterator it = contents.begin();  it != end;  ++it )
	{
		const char* name = *it;
		
		n::owned< p7::fd_t > list_i = p7::openat( dir_fd, name, p7::o_rdonly | p7::o_directory );
		
		try
		{
			return p7::readlinkat( list_i, path );
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT )
			{
				throw;
			}
		}
	}
	
	throw p7::errno_t( ENOENT );
}
