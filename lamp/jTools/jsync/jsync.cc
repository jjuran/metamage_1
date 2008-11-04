/*	========
 *	jsync.cc
 *	========
 */

// Standard C++
#include <functional>
#include <vector>

// Standard C/C++
#include <cstdio>
#include <cstdlib>

// POSIX
#include <errno.h>

// Iota
#include "iota/strings.hh"

// MoreFunctional
#include "PointerToFunction.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/walk.hh"

// POSeven
#include "POSeven/Directory.hh"
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/extras/pump.hh"
#include "POSeven/functions/fchmod.hh"
#include "POSeven/functions/fstat.hh"
#include "POSeven/functions/mkdir.hh"
#include "POSeven/functions/lseek.hh"
#include "POSeven/functions/read.hh"
#include "POSeven/functions/stat.hh"
#include "POSeven/functions/utime.hh"
#include "POSeven/functions/write.hh"
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
	
	
	static bool globally_verbose = false;
	static bool global_dry_run = false;
	
	static bool globally_up   = false;
	static bool globally_down = false;
	
	static bool globally_deleting = false;
	
	static std::string global_base_root;
	static std::string global_local_root;
	static std::string global_remote_root;
	
	static bool globally_locking_files = false;
	
	
	static const std::string& mkdir_path( const std::string& path )
	{
		if ( !io::directory_exists( path ) )
		{
			mkdir_path( io::get_preceding_directory( path ) );
			
			p7::mkdir( path );
		}
		
		return path;
	}
	
	
	static std::string get_subpath( const std::string& root_path, const std::string& subdir_path )
	{
		ASSERT( subdir_path.length() >= root_path.length() );
		
		ASSERT( *root_path.rbegin() != '/' );
		
		if ( subdir_path.length() == root_path.length() )
		{
			return "";
		}
		
		std::string result( subdir_path.begin() + root_path.length() + 1, subdir_path.end() );
		
		ASSERT( *result.rbegin() != '/' );
		
		return result;
	}
	
	static std::string get_dir_subpath( const std::string& root_path, const std::string& subdir_path )
	{
		return get_subpath( root_path, subdir_path ) + '/';
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
	
	
	inline void copy_modification_date( p7::fd_t in, const std::string& out_file )
	{
		// Copy the modification date
		p7::utime( out_file, p7::fstat( in ).st_mtime );
	}
	
	static void copy_file( const std::string& source, const std::string& dest )
	{
		//p7::copyfile( source, dest );
		
		// Lock backup files to prevent accidents
		p7::mode_t mode = p7::mode_t( globally_locking_files ? 0400 : 0600 );
		
		NN::Owned< p7::fd_t > in  = p7::open( source, p7::o_rdonly );
		NN::Owned< p7::fd_t > out = p7::open( dest,   p7::o_wronly | p7::o_creat | p7::o_excl, mode );
		
		p7::pump( in, out );
		
		p7::close( out );
		
		copy_modification_date( in, dest );
	}
	
	static void recursively_copy_directory( const std::string& source, const std::string& dest );
	
	static void recursively_copy( const std::string& source, const std::string& dest )
	{
		if ( io::file_exists( source ) )
		{
			if ( !filter_file( source ) )
			{
				copy_file( source, dest );
			}
		}
		else
		{
			recursively_copy_directory( source, dest );
		}
	}
	
	typedef std::pair< std::string, std::string > pair_of_strings;
	
	static void recursively_copy_into( const char*             name,
	                                   const pair_of_strings&  dirs )
	{
		recursively_copy( dirs.first / name, dirs.second / name );
	}
	
	static void recursively_copy_directory_contents( const std::string& source, const std::string& dest )
	{
		typedef p7::directory_contents_container directory_container;
		
		directory_container contents = io::directory_contents( source );
		
		std::for_each( contents.begin(),
		               contents.end(),
		               std::bind2nd( more::ptr_fun( recursively_copy_into ),
		                             std::make_pair( source, dest ) ) );
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
	
	
	static void compare_3_files( p7::fd_t  a,
	                             p7::fd_t  b,
	                             p7::fd_t  c,
	                             bool&     a_matches_b,
	                             bool&     b_matches_c,
	                             bool&     c_matches_a )
	{
		const std::size_t buffer_size = 4096;
		
		char a_buffer[ buffer_size ];
		char b_buffer[ buffer_size ];
		char c_buffer[ buffer_size ];
		
		while ( a_matches_b || b_matches_c || c_matches_a )
		{
			ssize_t a_read = 0;
			ssize_t b_read = 0;
			ssize_t c_read = 0;
			
			if ( c_matches_a || a_matches_b )
			{
				a_read = p7::read( a, a_buffer, buffer_size );
			}
			
			if ( a_matches_b || b_matches_c )
			{
				b_read = p7::read( b, b_buffer, buffer_size );
			}
			
			if ( b_matches_c || c_matches_a )
			{
				c_read = p7::read( c, c_buffer, buffer_size );
			}
			
			if ( a_read + b_read + c_read == 0 )
			{
				break;
			}
			
			if ( a_matches_b )
			{
				a_matches_b = a_read == b_read  &&  std::equal( a_buffer, a_buffer + a_read, b_buffer );
			}
			
			if ( b_matches_c )
			{
				b_matches_c = c_read == b_read  &&  std::equal( c_buffer, c_buffer + c_read, b_buffer );
			}
			
			if ( c_matches_a )
			{
				c_matches_a = a_read == c_read  &&  std::equal( a_buffer, a_buffer + a_read, c_buffer );
			}
		}
	}
	
	
	static void sync_files( const std::string&  subpath,
	                        bool                b_exists )
	{
		std::string a = global_local_root  / subpath;
		std::string b = global_base_root   / subpath;
		std::string c = global_remote_root / subpath;
		
		//std::string subpath = get_subpath( global_local_root, a );
		
		if ( globally_verbose )
		{
			//std::printf( "%s\n", a.c_str() );
		}
		
		NN::Owned< p7::fd_t > a_fd = p7::open( a, p7::o_rdonly );
		NN::Owned< p7::fd_t > c_fd = p7::open( c, p7::o_rdonly );
		
		NN::Owned< p7::fd_t > b_fd;
		
		if ( b_exists )
		{
			b_fd = p7::open( b, p7::o_rdonly );
			
			time_t a_time = p7::fstat( a_fd ).st_mtime;
			time_t b_time = p7::fstat( b_fd ).st_mtime;
			time_t c_time = p7::fstat( c_fd ).st_mtime;
			
			if ( a_time == b_time  &&  c_time == b_time )
			{
				return;
			}
		}
		
		bool a_matches_b = b_exists;
		bool b_matches_c = b_exists;
		bool c_matches_a = true;
		
		compare_3_files( a_fd,
		                 b_fd,
		                 c_fd,
		                 a_matches_b,
		                 b_matches_c,
		                 c_matches_a );
		
		if ( a_matches_b && b_matches_c )
		{
			return;
		}
		
		if ( !c_matches_a )
		{
			// A and C are different from each other,
			// so at least one must have changed from B
			
			if ( !a_matches_b && !b_matches_c )
			{
				const char* status = b_exists ? "requires 3-way merge"
				                              : "added simultaneously with different contents";
				
				std::printf( "### %s %s\n", subpath.c_str(), status );
				
				return;
			}
			
			// A and C do not both match B, and A and C do not both differ from B.
			// One matches (i.e. is unchanged) and one differs.  A copy is required.
			
			const bool doable = a_matches_b ? globally_down : globally_up;
			
			std::printf( "%s %s\n", a_matches_b ? doable ? "--->"
			                                             : "---|"
			                                    : doable ? "<---"
			                                             : "|---", subpath.c_str() );
			
			if ( !doable || global_dry_run )
			{
				return;
			}
			
			p7::fd_t                from_fd = a_matches_b ? c_fd : a_fd;
			NN::Owned< p7::fd_t >&  to_fd   = a_matches_b ? a_fd : c_fd;
			const std::string&      to_path = a_matches_b ? a    : c;
			
			p7::lseek( from_fd, 0 );
			
			p7::close( to_fd );
			
			to_fd = p7::open( to_path, p7::o_rdwr | p7::o_trunc );
			
			p7::pump( from_fd, to_fd );
			
			copy_modification_date( from_fd, to_path );
		}
		else
		{
			std::printf( "%s %s\n", b_exists ? "----" : "+--+", subpath.c_str() );
		}
		
		if ( global_dry_run )
		{
			return;
		}
		
		// A and C match, but B is out of date
		
		// copy a to b
		p7::lseek( a_fd, 0 );
		
		if ( b_exists )
		{
			p7::fchmod( b_fd, p7::mode_t( 0600 ) );  // unlock
			
			p7::close( b_fd );
		}
		
		b_fd = p7::open( b, p7::o_rdwr | p7::o_trunc | p7::o_creat, p7::mode_t( 0400 ) );
		
		p7::pump( a_fd, b_fd );
		
		copy_modification_date( a_fd, b );
		
		if ( b_exists )
		{
			p7::fchmod( b_fd, p7::mode_t( 0400 ) );  // lock
		}
	}
	
	static void recursively_sync_directories( const std::string& subpath );
	
	static void recursively_sync( const std::string& subpath )
	{
		std::string a = global_local_root  / subpath;
		std::string b = global_base_root   / subpath;
		std::string c = global_remote_root / subpath;
		
		const bool b_exists = io::item_exists( b );
		
		bool a_is_dir = io::directory_exists( a );
		bool b_is_dir = io::directory_exists( b );
		bool c_is_dir = io::directory_exists( c );
		
		if ( bool matched = a_is_dir == c_is_dir  &&  (!b_exists || a_is_dir == b_is_dir) )
		{
			if ( a_is_dir )
			{
				if ( !b_exists )
				{
					p7::mkdir( b );
				}
				
				recursively_sync_directories( subpath );
			}
			else
			{
				sync_files( subpath, b_exists );
			}
		}
		else if ( b_exists )
		{
			// file vs. directory
			if ( a_is_dir != b_is_dir )
			{
				std::printf( "### %s changed from %s to %s\n",
				                  a.c_str(),      b_is_dir ? "directory" : "file",
				                                        a_is_dir ? "directory" : "file" );
			}
			
			if ( c_is_dir != b_is_dir )
			{
				std::printf( "### %s changed from %s to %s\n",
				                  c.c_str(),      b_is_dir ? "directory" : "file",
				                                        c_is_dir ? "directory" : "file" );
			}
		}
		else
		{
			std::printf( "### Add conflict in %s (file vs. directory)\n", a.c_str() );
		}
	}
	
	template < class In, class Out, class Pred >
	Out copy_if( In begin, In end, Out result, Pred f )
	{
		while ( begin != end )
		{
			if ( f( *begin ) )
			{
				*result++ = *begin;
			}
			
			++begin;
		}
		
		return result;
	}
	
	template < class In, class Out, class Pred >
	Out copy_unless( In begin, In end, Out result, Pred f )
	{
		while ( begin != end )
		{
			if ( !f( *begin ) )
			{
				*result++ = *begin;
			}
			
			++begin;
		}
		
		return result;
	}
	
	inline const std::string& identity( const std::string& s )
	{
		return s;
	}
	
	struct null_iterator
	{
		struct null
		{
			template < class Data >  void operator=( Data )  {}
		};
		
		typedef std::forward_iterator_tag iterator_category;
		
		typedef null  value_type;
		typedef null *pointer;
		typedef null& reference;
		
		typedef int difference_type;
		
		null_iterator operator++(     )  { return null_iterator(); }
		null_iterator operator++( int )  { return null_iterator(); }
		
		null operator*()  { return null(); }
	};
	
	template < class Type >
	static int compare( const Type& a, const Type& b )
	{
		return   a < b ? -1
		       : a > b ?  1
		       :          0;
	}
	
	template < class Iter, class A, class B, class Both >
	static void compare_sequences( Iter a_begin, Iter a_end,
	                               Iter b_begin, Iter b_end, A a_only,
	                                                         B b_only, Both both )
	{
		while ( a_begin != a_end  &&  b_begin != b_end )
		{
			typedef typename std::iterator_traits< Iter >::value_type value_type;
			
			const value_type& a = *a_begin;
			const value_type& b = *b_begin;
			
			switch ( compare( a, b ) )
			{
				case 0:
					*both++ = a;
					
					++a_begin;
					++b_begin;
					
					break;
				
				case -1:
					*a_only++ = a;
					
					a_begin++;
					break;
				
				case 1:
					*b_only++ = b;
					
					b_begin++;
					break;
				
			}
		}
		
		std::copy( a_begin, a_end, a_only );
		std::copy( b_begin, b_end, b_only );
	}
	
	static void recursively_sync_directory_contents( const std::string& subpath )
	{
		std::string a_dir = global_local_root  / subpath;
		std::string b_dir = global_base_root   / subpath;
		std::string c_dir = global_remote_root / subpath;
		
		typedef p7::directory_contents_container directory_container;
		
		directory_container a_contents = io::directory_contents( a_dir );
		directory_container b_contents = io::directory_contents( b_dir );
		directory_container c_contents = io::directory_contents( c_dir );
		
		std::vector< std::string > a;
		std::vector< std::string > b;
		std::vector< std::string > c;
		
		copy_unless( a_contents.begin(), a_contents.end(), std::back_inserter( a ), std::ptr_fun( filter_item ) );
		copy_unless( b_contents.begin(), b_contents.end(), std::back_inserter( b ), std::ptr_fun( filter_item ) );
		copy_unless( c_contents.begin(), c_contents.end(), std::back_inserter( c ), std::ptr_fun( filter_item ) );
		
		std::sort( a.begin(), a.end() );
		std::sort( b.begin(), b.end() );
		std::sort( c.begin(), c.end() );
		
		std::vector< std::string > a_added;
		std::vector< std::string > a_removed;
		std::vector< std::string > a_static;
		
		std::vector< std::string > c_added;
		std::vector< std::string > c_removed;
		std::vector< std::string > c_static;
		
		compare_sequences( a.begin(), a.end(),
		                   b.begin(), b.end(),
		                   std::back_inserter( a_added   ),
		                   std::back_inserter( a_removed ),
		                   std::back_inserter( a_static  ) );
		
		compare_sequences( c.begin(), c.end(),
		                   b.begin(), b.end(),
		                   std::back_inserter( c_added   ),
		                   std::back_inserter( c_removed ),
		                   std::back_inserter( c_static  ) );
		
		std::vector< std::string > a_created;
		std::vector< std::string > c_created;
		std::vector< std::string > mutually_added;
		
		std::vector< std::string > a_deleted;
		std::vector< std::string > c_deleted;
		std::vector< std::string > mutually_deleted;
		
		std::vector< std::string > mutually_static;
		
		compare_sequences( a_added.begin(), a_added.end(),
		                   c_added.begin(), c_added.end(),
		                   std::back_inserter( a_created ),
		                   std::back_inserter( c_created ),
		                   std::back_inserter( mutually_added ) );
		
		compare_sequences( a_removed.begin(), a_removed.end(),
		                   c_removed.begin(), c_removed.end(),
		                   std::back_inserter( a_deleted ),
		                   std::back_inserter( c_deleted ),
		                   std::back_inserter( mutually_deleted ) );
		
		compare_sequences( a_static.begin(), a_static.end(),
		                   c_static.begin(), c_static.end(),
		                   null_iterator(),
		                   null_iterator(),
		                   std::back_inserter( mutually_static ) );
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = a_created.begin();  it != a_created.end();  ++ it )
		{
			const std::string& filename = *it;
			
			const bool doable = globally_up;
			
			std::string child_subpath = subpath + filename;
			
			std::printf( "%s %s\n", doable ? "<+++" : "|+++", child_subpath.c_str() );
			
			if ( doable && !global_dry_run )
			{
				std::string a_path = global_local_root  / child_subpath;
				std::string b_path = global_base_root   / child_subpath;
				std::string c_path = global_remote_root / child_subpath;
				
				globally_locking_files = false;
				
				recursively_copy( a_path, c_path );
				
				globally_locking_files = true;
				
				recursively_copy( a_path, b_path );
			}
		}
		
		for ( Iter it = c_created.begin();  it != c_created.end();  ++ it )
		{
			const std::string& filename = *it;
			
			const bool doable = globally_down;
			
			std::string child_subpath = subpath + filename;
			
			std::printf( "%s %s\n", doable ? "+++>" : "+++|", child_subpath.c_str() );
			
			if ( doable && !global_dry_run )
			{
				std::string a_path = global_local_root  / child_subpath;
				std::string b_path = global_base_root   / child_subpath;
				std::string c_path = global_remote_root / child_subpath;
				
				globally_locking_files = false;
				
				recursively_copy( c_path, a_path );
				
				globally_locking_files = true;
				
				recursively_copy( c_path, b_path );
			}
		}
		
		for ( Iter it = mutually_added.begin();  it != mutually_added.end();  ++ it )
		{
			const std::string& filename = *it;
			
			std::string child_subpath = subpath + filename;
			
			std::printf( "++++ %s\n", child_subpath.c_str() );
			
			recursively_sync( child_subpath );
		}
		
		for ( Iter it = a_deleted.begin();  it != a_deleted.end();  ++ it )
		{
			const std::string& filename = *it;
			
			const bool doable = globally_up && globally_deleting;
			
			std::string child_subpath = subpath + filename;
			
			std::printf( "%s %s\n", doable ? "<--X" : "|--X", child_subpath.c_str() );
			
			if ( doable && !global_dry_run )
			{
				std::string b_path = global_base_root   / child_subpath;
				std::string c_path = global_remote_root / child_subpath;
				
				io::recursively_delete( c_path );
				io::recursively_delete( b_path );
			}
		}
		
		for ( Iter it = c_deleted.begin();  it != c_deleted.end();  ++ it )
		{
			const std::string& filename = *it;
			
			const bool doable = globally_down && globally_deleting;
			
			std::string child_subpath = subpath + filename;
			
			std::printf( "%s %s\n", doable ? "X-->" : "X--|", child_subpath.c_str() );
			
			if ( doable && !global_dry_run )
			{
				std::string a_path = global_local_root / child_subpath;
				std::string b_path = global_base_root  / child_subpath;
				
				io::recursively_delete( a_path );
				io::recursively_delete( b_path );
			}
		}
		
		for ( Iter it = mutually_deleted.begin();  it != mutually_deleted.end();  ++ it )
		{
			const std::string& filename = *it;
			
			std::string child_subpath = subpath + filename;
			
			std::printf( "X--X %s\n", child_subpath.c_str() );
			
			if ( !global_dry_run )
			{
				std::string b_path = global_base_root / child_subpath;
				
				io::recursively_delete( b_path );
			}
		}
		
		for ( Iter it = mutually_static.begin();  it != mutually_static.end();  ++ it )
		{
			const std::string& filename = *it;
			
			recursively_sync( subpath + filename );
		}
		
		// added/nil:  simple add -- just do it
		// added/added:  mutual add -- check for conflicts
		// static/static:  check for conflicts
		// deleted/static:  single delete -- check for conflicts
		// deleted/deleted:  mutual delete -- just do it
	}
	
	static void recursively_sync_directories( const std::string& subpath )
	{
		// compare any relevant metadata, like Desktop comment
		
		if ( globally_verbose )
		{
			std::printf( "%s\n", subpath.c_str() );
		}
		
		recursively_sync_directory_contents( subpath + '/' );
	}
	
	
	static std::string home_dir_pathname()
	{
		if ( const char* home = std::getenv( "HOME" ) )
		{
			return home;
		}
		
		return "/";
	}
	
	static std::string get_jsync_root_pathname()
	{
		std::string home = home_dir_pathname();
		
		const char* jsync = "Library/JSync";
		
		return mkdir_path( home / jsync );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		bool bidirectional = false;
		bool null          = false;
		
		O::BindOption( "-v", globally_verbose );
		O::BindOption( "-n", global_dry_run   );
		O::BindOption( "-v", globally_verbose );
		
		O::AliasOption( "-n", "--dry-run" );
		O::AliasOption( "-v", "--verbose" );
		
		O::BindOption( "--up",   globally_up   );
		O::BindOption( "--down", globally_down );
		
		O::BindOption( "--delete", globally_deleting );
		
		O::BindOption( "-0", null          );
		O::BindOption( "-2", bidirectional );
		
		O::AliasOption( "-2", "--bidi" );
		
		O::GetOptions( argc, argv );
		
		if ( bidirectional )
		{
			globally_up   =
			globally_down = true;
		}
		else if ( !globally_up && !globally_down && !null )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "jsync: nothing to do; specify --up, --down, or -2,""\n"
			                                       "jsync: or pass -0 go through the motions anyway."  "\n" ) );
			
			return p7::exit_failure;
		}
		
		char const *const *free_args = O::FreeArguments();
		
		std::size_t n_args = O::FreeArgumentCount();
		
		const char* default_path = "Default";
		
		const char* path = n_args >= 1 ? free_args[0] : default_path;
		
		std::string jsync_root = get_jsync_root_pathname();
		
		std::string jsync_path = jsync_root / path;
		
		jsync_path += ".jsync";
		
		if ( !io::directory_exists( jsync_path ) )
		{
			std::fprintf( stderr, "jsync: no such sync path '%s'\n"
			                      "(No such directory %s)\n",path,
			                      jsync_path.c_str() );
			
			return 1;
		}
		
		global_local_root  = jsync_path / "Sandbox";  // should be a link
		global_remote_root = jsync_path / "Remote";   // should be a link
		global_base_root   = jsync_path / "Base";
		
		recursively_sync_directory_contents( "" );
		
		return 0;
	}

}

