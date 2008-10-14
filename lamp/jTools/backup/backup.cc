/*	=========
 *	backup.cc
 *	=========
 */

// Standard C++
#include <functional>
#include <vector>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// Iota
#include "iota/strings.hh"

// MoreFunctional
#include "PointerToFunction.hh"

// POSeven
#include "POSeven/Directory.hh"
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/extras/pump.hh"
#include "POSeven/functions/fchmod.hh"
#include "POSeven/functions/fstat.hh"
#include "POSeven/functions/mkdir.hh"
#include "POSeven/functions/rename.hh"
#include "POSeven/functions/stat.hh"
#include "POSeven/functions/symlink.hh"
#include "POSeven/functions/utime.hh"
#include "POSeven/types/exit_t.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace O = Orion;
	
	
	using namespace io::path_descent_operators;
	
	
	static const std::string& mkdir_path( const std::string& path )
	{
		if ( !io::directory_exists( path ) )
		{
			mkdir_path( io::get_preceding_directory( path ) );
			
			p7::mkdir( path );
		}
		
		return path;
	}
	
	
	static bool filter_file( const std::string& path )
	{
		std::string filename = io::get_filename( path );
		
		return filename == "Icon\r"
		    || filename == ".DS_Store";
	}
	
	static bool filter_directory( const std::string& path )
	{
		std::string filename = io::get_filename( path );
		
		return filename == "CVS"
		    || filename == "CVSROOT";
	}
	
	static bool filter_item( const std::string& path )
	{
		return filter_file( path ) || filter_directory( path );
	}
	
	
	static void copy_file( const std::string& source, const std::string& dest )
	{
		if ( filter_file( source ) )
		{
			return;
		}
		
		//p7::copyfile( source, dest );
		
		NN::Owned< p7::fd_t > in  = p7::open( source, p7::o_rdonly );
		NN::Owned< p7::fd_t > out = p7::open( dest,   p7::o_wronly | p7::o_creat | p7::o_excl, p7::mode_t( 0400 ) );
		
		p7::pump( in, out );
		
		// Lock the backup file to prevent accidents
		p7::fchmod( out, p7::mode_t( 0400 ) );
		
		p7::close( out );
		
		// Copy the modification date
		p7::utime( dest, p7::fstat( in ).st_mtime );
	}
	
	static void recursively_copy_directory( const std::string& source, const std::string& dest );
	
	static void recursively_copy( const std::string& source, const std::string& dest )
	{
		if ( io::file_exists( source ) )
		{
			copy_file( source, dest );
		}
		else
		{
			recursively_copy_directory( source, dest );
		}
	}
	
	static void recursively_copy_into( const std::string& source, const std::string& dest )
	{
		recursively_copy( source, dest / io::get_filename( source ) );
	}
	
	static void recursively_copy_directory_contents( const std::string& source, const std::string& dest )
	{
		typedef p7::directory_contents_container directory_container;
		
		directory_container contents = io::directory_contents( source );
		
		std::for_each( contents.begin(),
		               contents.end(),
		               std::bind2nd( more::ptr_fun( recursively_copy_into ),
		                             dest ) );
	}
	
	static void recursively_copy_directory( const std::string& source, const std::string& dest )
	{
		if ( filter_directory( source ) )
		{
			return;
		}
		
		p7::mkdir( dest );
		
		recursively_copy_directory_contents( source, dest );
	}
	
	
	static void compare_files( const std::string& a, const std::string& b )
	{
		NN::Owned< p7::fd_t > a_fd = p7::open( a, p7::o_rdonly );
		NN::Owned< p7::fd_t > b_fd = p7::open( b, p7::o_rdonly );
		
		const std::size_t buffer_size = 4096;
		
		char a_buffer[ buffer_size ];
		char b_buffer[ buffer_size ];
		
		while ( true )
		{
			ssize_t a_read = p7::read( a_fd, a_buffer, buffer_size );
			ssize_t b_read = p7::read( b_fd, b_buffer, buffer_size );
			
			ssize_t minimum = std::min( a_read, b_read );
			
			if ( a_read != b_read  ||  !std::equal( a_buffer, a_buffer + minimum, b_buffer ) )
			{
				std::printf( "%s has changed\n", a.c_str() );
				
				break;
			}
			
			if ( minimum < buffer_size )
			{
				break;
			}
		}
	}
	
	static void recursively_compare_directories( const std::string& a, const std::string& b );
	
	static void recursively_compare( const std::string& a, const std::string& b )
	{
		bool a_is_dir = io::directory_exists( a );
		bool b_is_dir = io::directory_exists( b );
		
		if ( bool matched = a_is_dir == b_is_dir )
		{
			if ( a_is_dir )
			{
				recursively_compare_directories( a, b );
			}
			else
			{
				compare_files( a, b );
			}
		}
		else
		{
			// file vs. directory
			std::printf( "%s changed from %s to %s\n",
			              a.c_str(),      b_is_dir ? "directory" : "file",
			                                    a_is_dir ? "directory" : "file" );
		}
	}
	
	static void odd_item( const std::string& path, bool new_vs_old )
	{
		std::printf( "%s is %s\n", path.c_str(), new_vs_old ? "new" : "old" );
	}
	
	inline void new_item( const std::string& path )
	{
		odd_item( path, true );
	}
	
	inline void old_item( const std::string& path )
	{
		odd_item( path, false );
	}
	
	static void recursively_compare_directory_contents( const std::string& a_dir, const std::string& b_dir )
	{
		typedef p7::directory_contents_container directory_container;
		
		directory_container a_contents = io::directory_contents( a_dir );
		directory_container b_contents = io::directory_contents( b_dir );
		
		std::vector< std::string > a;
		std::vector< std::string > b;
		
		std::copy( a_contents.begin(), a_contents.end(), std::back_inserter( a ) );
		std::copy( b_contents.begin(), b_contents.end(), std::back_inserter( b ) );
		
		std::sort( a.begin(), a.end() );
		std::sort( b.begin(), b.end() );
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		Iter aa = a.begin();
		Iter bb = b.begin();
		
		while ( aa != a.end()  &&  bb != b.end() )
		{
			while ( aa != a.end()  &&  filter_item( *aa ) )
			{
				++aa;
			}
			
			while ( bb != b.end()  &&  filter_item( *bb ) )
			{
				++bb;
			}
			
			if ( aa == a.end()  ||  bb == b.end() )
			{
				break;
			}
			
			// Redefining a and b, but as different types
			const std::string& a = *aa;
			const std::string& b = *bb;
			
			std::string a_name = io::get_filename( a );
			std::string b_name = io::get_filename( b );
			
			if ( a_name == b_name )
			{
				recursively_compare( a, b );
				
				++aa;
				++bb;
			}
			else if ( a_name < b_name )
			{
				// new item
				new_item( a );
				++aa;
			}
			else
			{
				// deleted item
				old_item( a_dir / b_name );
				++bb;
			}
		}
		
		while ( aa != a.end() )
		{
			// new items
			if ( !filter_item( *aa ) )
			{
				new_item( *aa );
			}
			
			++aa;
		}
		
		while ( bb != b.end() )
		{
			// deleted items
			if ( !filter_item( *bb ) )
			{
				old_item( a_dir / io::get_filename( *bb ) );
			}
			
			++bb;
		}
	}
	
	static void recursively_compare_directories( const std::string& a, const std::string& b )
	{
		// compare any relevant metadata, like Desktop comment
		
		recursively_compare_directory_contents( a, b );
	}
	
	
	static std::string home_dir_pathname()
	{
		if ( const char* home = std::getenv( "HOME" ) )
		{
			return home;
		}
		
		return "/";
	}
	
	static std::string get_backups_root_pathname()
	{
		std::string home = home_dir_pathname();
		
		const char* backups = "Library/Backups";
		
		return mkdir_path( home / backups );
	}
	
	
	static void back_up_target( const std::string& target, const std::string& storage )
	{
		std::string in_progress = mkdir_path( storage ) / "(In Progress)";
		
		p7::mkdir( in_progress );
		
		time_t mod_time = p7::stat( in_progress ).st_mtime;
		
		struct tm backup_time;
		
		gmtime_r( &mod_time, &backup_time );
		
		recursively_copy_directory_contents( target, in_progress );
		
		char name[ sizeof "2008-10-02 01:30:00" ];  // 19 + 1 = 20
		
		std::sprintf( name,
		              "%.4d-%.2d-%.2d %.2d;%.2d;%.2d", backup_time.tm_year % 4096 + 1900,
		                                               backup_time.tm_mon  %   16 +    1,
		                                               backup_time.tm_mday %   32 +    1,
		                                               backup_time.tm_hour %   32,
		                                               backup_time.tm_min  %   64,
		                                               backup_time.tm_sec  %   64 );
		
		std::string new_path = storage / name;
		
		p7::rename( in_progress, new_path );
		
		std::string active = storage / "Active";
		
		::unlink( active.c_str() );
		
		p7::symlink( name, active );
	}
	
	
	static int Usage()
	{
		p7::write( p7::stderr_fileno, STR_LEN( "Usage: backup src dest\n" ) );
		
		return 2;
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		bool backing_up = false;
		bool comparing  = false;
		
		O::BindOption( "--backup", backing_up );
		O::BindOption( "--compare", comparing );
		
		O::GetOptions( argc, argv );
		
		char const *const *free_args = O::FreeArguments();
		
		std::size_t n_args = O::FreeArgumentCount();
		
		const char* default_path = "Default";
		
		const char* path = n_args >= 1 ? free_args[0] : default_path;
		
		std::string backups_root = get_backups_root_pathname();
		
		std::string backup_path = backups_root / path;
		
		backup_path += ".backup";
		
		if ( !io::directory_exists( backup_path ) )
		{
			std::fprintf( stderr, "backup: no such sync path '%s'\n"
			                      "(No such directory %s)\n",path,
			                      backup_path.c_str() );
			
			return 1;
		}
		
		std::string backup_target = backup_path / "Target";  // should be a link
		std::string backup_storage = backup_path / "Storage";
		
		if ( comparing )
		{
			recursively_compare_directories( backup_target, backup_storage / "Active" );
		}
		
		if ( backing_up )
		{
			back_up_target( backup_target, backup_storage );
		}
		
		return 0;
	}

}

