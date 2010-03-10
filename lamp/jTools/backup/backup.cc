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

// Io
#include "io/walk.hh"

// poseven
#include "poseven/Directory.hh"
#include "poseven/Pathnames.hh"
#include "poseven/extras/pump.hh"
#include "poseven/functions/fchmod.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/mkdir.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/rename.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/symlink.hh"
#include "poseven/functions/utime.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	
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
		
		n::owned< p7::fd_t > in  = p7::open( source, p7::o_rdonly );
		n::owned< p7::fd_t > out = p7::open( dest,   p7::o_wronly | p7::o_creat | p7::o_excl, p7::_400 );
		
		p7::pump( in, out );
		
		// Lock the backup file to prevent accidents
		p7::fchmod( out, p7::_400 );
		
		p7::close( out );
		
		// Copy the modification date
		p7::utime( dest, p7::fstat( in ).st_mtime );
	}
	
	class directory_maker
	{
		private:
			const std::string& its_source;
			const std::string& its_dest;
		
		public:
			directory_maker( const std::string& source,
			                 const std::string& dest ) : its_source( source ),
			                                             its_dest  ( dest   )
			{
			}
			
			bool operator()( const std::string& path, unsigned depth ) const
			{
				if ( filter_directory( path ) )
				{
					return false;
				}
				
				std::string new_path = its_dest;
				
				new_path.append( path.begin() + its_source.size(), path.end() );
				
				p7::mkdir( new_path );
				
				return true;
			}
	};
	
	class file_copier
	{
		private:
			const std::string& its_source;
			const std::string& its_dest;
		
		public:
			file_copier( const std::string& source,
			             const std::string& dest ) : its_source( source ),
			                                         its_dest  ( dest   )
			{
			}
			
			void operator()( const std::string& path, unsigned depth ) const
			{
				if ( filter_file( path ) )
				{
					return;
				}
				
				std::string new_path = its_dest;
				
				new_path.append( path.begin() + its_source.size(), path.end() );
				
				copy_file( path, new_path );
			}
	};
	
	
	static void compare_files( const std::string& a, const std::string& b )
	{
		n::owned< p7::fd_t > a_fd = p7::open( a, p7::o_rdonly );
		n::owned< p7::fd_t > b_fd = p7::open( b, p7::o_rdonly );
		
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
	
	static inline void new_item( const std::string& path )
	{
		odd_item( path, true );
	}
	
	static inline void old_item( const std::string& path )
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
			
			const std::string& a_name = *aa;
			const std::string& b_name = *bb;
			
			int cmp = std::strcmp( a_name.c_str(), b_name.c_str() );
			
			if ( cmp == 0 )
			{
				recursively_compare( a_dir / a_name, b_dir / b_name );
				
				++aa;
				++bb;
			}
			else if ( cmp < 0 )
			{
				// new item
				new_item( a_dir / a_name );
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
				new_item( a_dir / *aa );
			}
			
			++aa;
		}
		
		while ( bb != b.end() )
		{
			// deleted items
			if ( !filter_item( *bb ) )
			{
				old_item( a_dir / *bb );
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
		
		io::recursively_walk_subtrees( target,
		                               directory_maker( target, in_progress ),
		                               file_copier    ( target, in_progress ),
		                               io::walk_noop() );
		
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
		
		o::bind_option_to_variable( "--backup", backing_up );
		o::bind_option_to_variable( "--compare", comparing );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		std::size_t n_args = o::free_argument_count();
		
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

