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

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// Relix
#include "relix/recurse.hh"

// Iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// plus
#include "plus/pointer_to_function.hh"
#include "plus/var_string.hh"
#include "plus/string/concat.hh"

// poseven
#include "poseven/extras/pump.hh"
#include "poseven/functions/dup.hh"
#include "poseven/functions/fchmod.hh"
#include "poseven/functions/fdopendir.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/fstatat.hh"
#include "poseven/functions/futimens.hh"
#include "poseven/functions/mkdir.hh"
#include "poseven/functions/mkdirat.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/readlinkat.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/symlinkat.hh"
#include "poseven/functions/unlinkat.hh"
#include "poseven/functions/write.hh"
#include "poseven/sequences/directory_contents.hh"
#include "poseven/types/exit_t.hh"

// pfiles
#include "pfiles/common.hh"

// Io
#include "io/walk.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_no_op   = '0',
	Option_2_dirs  = '2',
	Option_dry_run = 'n',
	Option_verbose = 'v',
	
	Option_last_byte = 255,
	
	Option_up,
	Option_down,
	Option_delete,
};

static command::option options[] =
{
	{ "up",      Option_up      },
	{ "down",    Option_down    },
	{ "null",    Option_no_op   },
	{ "bidi",    Option_2_dirs  },
	{ "delete",  Option_delete  },
	{ "dry-run", Option_dry_run },
	{ "verbose", Option_verbose },
	
	{ NULL }
};

static bool globally_verbose = false;
static bool global_dry_run = false;

static bool globally_up   = false;
static bool globally_down = false;

static bool globally_deleting = false;

static bool null = false;


static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		using namespace tool;
		
		switch ( opt )
		{
			case Option_2_dirs:
				globally_up = true;
				globally_down = true;
				break;
			
			case Option_down:
				globally_down = true;
				break;
			
			case Option_up:
				globally_up = true;
				break;
			
			case Option_no_op:
				null = true;
				break;
			
			case Option_dry_run:
				global_dry_run = true;
				break;
			
			case Option_verbose:
				globally_verbose = true;
				break;
			
			case Option_delete:
				globally_deleting = true;
				break;
			
			default:
				std::abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	using namespace io::path_descent_operators;
	
	
	static plus::string global_base_root;
	static plus::string global_local_root;
	static plus::string global_remote_root;
	
	static bool globally_locking_files = false;
	
	
	static mode_t get_mode( p7::fd_t dir_fd, const char* path )
	{
		struct stat sb;
		
		const bool exists = p7::fstatat( dir_fd, path, sb, p7::at_symlink_nofollow );
		
		return exists ? sb.st_mode : 0;
	}
	
	static inline n::owned< p7::fd_t > open_dir( p7::fd_t dirfd, const char* path )
	{
		return p7::openat( dirfd, path, p7::o_rdonly | p7::o_directory | p7::o_nofollow );
	}
	
	static inline n::owned< p7::fd_t > open_dir( const char* path )
	{
		return p7::open( path, p7::o_rdonly | p7::o_directory );
	}
	
	static inline n::owned< p7::fd_t > open_dir( const plus::string& path )
	{
		return open_dir( path.c_str() );
	}
	
	static const plus::string& mkdir_path( const plus::string& path )
	{
		if ( !io::directory_exists( path ) )
		{
			mkdir_path( io::get_preceding_directory( path ) );
			
			p7::mkdir( path.c_str() );
		}
		
		return path;
	}
	
	
	struct filter_node
	{
		const char* name;
	};
	
	static const filter_node global_filter_nodes[] =
	{
		{ ".DS_Store" },
		{ ".git"      },
		{ "CVS"       },
		{ "CVSROOT"   },
		{ "Icon\r"    },
	};
	
	static bool operator==( const filter_node& node, const char* name )
	{
		return strcmp( node.name, name ) == 0;
	}
	
	
	static bool filter_item( const char* filename )
	{
		const filter_node *const begin = global_filter_nodes;
		const filter_node *const end   = begin + sizeof global_filter_nodes / sizeof global_filter_nodes[0];
		
		const filter_node* it = std::find( begin, end, filename );
		
		return it != end;
	}
	
	
	static inline void store_modification_dates( p7::fd_t fd, time_t local, time_t remote )
	{
		// Store the modification dates of the local and remote files as the
		// modification and backup/archive/checkpoint dates of the base file.
		
	#ifdef UTIME_ARCHIVE
		
		struct timespec times[2] =  { { remote, UTIME_ARCHIVE }, { local, 0 } };
		
		p7::futimens( fd, times );
		
	#endif
	}
	
	static void copy_file( p7::fd_t olddirfd, const char* name, p7::fd_t newdirfd )
	{
		//p7::copyfile( source, dest );
		
		// Lock backup files to prevent accidents
		const p7::mode_t mode = globally_locking_files ? p7::_400 : p7::_600;
		
		n::owned< p7::fd_t > in  = p7::openat( olddirfd, name, p7::o_rdonly | p7::o_nofollow );
		n::owned< p7::fd_t > out = p7::openat( newdirfd, name, p7::o_wronly | p7::o_nofollow | p7::o_creat | p7::o_excl, mode );
		
		p7::pump( in, out );
		
		p7::close( out );
	}
	
	static void copy_symlink( p7::fd_t olddirfd, const char* name, p7::fd_t newdirfd )
	{
		const plus::string target = p7::readlinkat( olddirfd, name );
		
		int unlinked = ::unlinkat( newdirfd, name, 0 );
		
		if ( unlinked < 0  &&  errno != ENOENT )
		{
			p7::throw_errno( errno );
		}
		
		p7::symlinkat( target, newdirfd, name );
	}
	
	static void recursively_copy_directory( p7::fd_t olddirfd, const char* name, p7::fd_t newdirfd );
	
	static void recursively_copy( p7::fd_t olddirfd, const char* name, p7::fd_t newdirfd )
	{
		const mode_t mode = get_mode( olddirfd, name );
		
		if ( S_ISREG( mode ) )
		{
			if ( !filter_item( name ) )
			{
				copy_file( olddirfd, name, newdirfd );
			}
		}
		else if ( S_ISLNK( mode ) )
		{
			copy_symlink( olddirfd, name, newdirfd );
		}
		else
		{
			relix::recurse( &recursively_copy_directory, olddirfd, name, newdirfd );
		}
	}
	
	static void recursively_copy( p7::fd_t olddirfd, const plus::string& name, p7::fd_t newdirfd )
	{
		recursively_copy( olddirfd, name.c_str(), newdirfd );
	}
	
	typedef std::pair< p7::fd_t, p7::fd_t > pair_of_fds;
	
	class recursive_copier
	{
		private:
			p7::fd_t  old_dirfd;
			p7::fd_t  new_dirfd;
		
		public:
			recursive_copier( p7::fd_t old_fd, p7::fd_t new_fd )
			:
				old_dirfd( old_fd ),
				new_dirfd( new_fd )
			{
			}
			
			void operator()( const char* name )
			{
				recursively_copy( old_dirfd, name, new_dirfd );
			}
	};
	
	static p7::directory_contents_container dirfd_contents( p7::fd_t fd )
	{
		return p7::directory_contents_container( p7::fdopendir( p7::dup( fd ) ) );
	}
	
	static void recursively_copy_directory_contents( p7::fd_t olddirfd, p7::fd_t newdirfd )
	{
		typedef p7::directory_contents_container directory_container;
		
		directory_container contents = dirfd_contents( olddirfd );
		
		std::for_each( contents.begin(),
		               contents.end(),
		               recursive_copier( olddirfd, newdirfd ) );
	}
	
	static void recursively_copy_directory( p7::fd_t olddirfd, const char* name, p7::fd_t newdirfd )
	{
		if ( filter_item( name ) )
		{
			return;
		}
		
		p7::mkdirat( newdirfd, name );
		
		recursively_copy_directory_contents( open_dir( olddirfd, name ),
		                                     open_dir( newdirfd, name ) );
	}
	
	static void compare_3_files( p7::fd_t  a,
	                             p7::fd_t  b,
	                             p7::fd_t  c,
	                             bool&     a_matches_b,
	                             bool&     b_matches_c,
	                             bool&     c_matches_a )
	{
		const std::size_t buffer_size = 4096;
		
		// Avoid large local allocations to prevent stack overruns
		static char a_buffer[ buffer_size ];
		static char b_buffer[ buffer_size ];
		static char c_buffer[ buffer_size ];
		
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
	
	
	static void sync_files( p7::fd_t     a_dirfd,
	                        p7::fd_t     b_dirfd,
	                        p7::fd_t     c_dirfd,
	                        const char*  subpath,
	                        const char*  filename,
	                        bool         b_exists )
	{
		if ( globally_verbose )
		{
			//std::printf( "%s\n", subpath );
		}
		
		n::owned< p7::fd_t > a_fd = p7::openat( a_dirfd, filename, p7::o_rdonly | p7::o_nofollow );
		n::owned< p7::fd_t > c_fd = p7::openat( c_dirfd, filename, p7::o_rdonly | p7::o_nofollow );
		
		n::owned< p7::fd_t > b_fd;
		
		const time_t a_time = p7::fstat( a_fd ).st_mtime;
		const time_t c_time = p7::fstat( c_fd ).st_mtime;
		
		if ( b_exists )
		{
			b_fd = p7::openat( b_dirfd, filename, p7::o_rdonly | p7::o_nofollow );
			
			const struct stat b_stat = p7::fstat( b_fd );
			
		#ifdef __RELIX__
			
			if ( b_stat.st_mtime == a_time  &&  b_stat.st_checktime == c_time )
			{
				return;
			}
			
		#endif
			
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
			store_modification_dates( b_fd, a_time, c_time );
			
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
				
				std::printf( "### %s %s\n", subpath, status );
				
				return;
			}
			
			// A and C do not both match B, and A and C do not both differ from B.
			// One matches (i.e. is unchanged) and one differs.  A copy is required.
			
			const bool doable = a_matches_b ? globally_down : globally_up;
			
			std::printf( "%s %s\n", a_matches_b ? doable ? "--->"
			                                             : "---|"
			                                    : doable ? "<---"
			                                             : "|---", subpath );
			
			if ( !doable || global_dry_run )
			{
				return;
			}
			
			p7::fd_t               from_fd = a_matches_b ? c_fd : a_fd;
			n::owned< p7::fd_t >&  to_fd   = a_matches_b ? a_fd : c_fd;
			
			p7::fd_t to_dirfd = a_matches_b ? a_dirfd : c_dirfd;
			
			p7::close( to_fd );
			
			to_fd = p7::openat( to_dirfd, filename, p7::o_rdwr | p7::o_trunc | p7::o_nofollow );
			
			off_t from_offset = 0;
			
			p7::pump( from_fd, &from_offset, to_fd );
		}
		else
		{
			std::printf( "%s %s\n", b_exists ? "----" : "+--+", subpath );
		}
		
		if ( global_dry_run )
		{
			return;
		}
		
		// A and C match, but B is out of date
		
		// copy a to b
		
		if ( b_exists )
		{
			p7::fchmod( b_fd, p7::_600 );  // unlock
			
			p7::close( b_fd );
		}
		
		b_fd = p7::openat( b_dirfd, filename, p7::o_rdwr | p7::o_trunc | p7::o_creat | p7::o_nofollow, p7::_400 );
		
		off_t from_offset = 0;
		
		p7::pump( a_fd, &from_offset, b_fd );
		
		store_modification_dates( b_fd, a_time, c_time );
		
		if ( b_exists )
		{
			p7::fchmod( b_fd, p7::_400 );  // lock
		}
	}
	
	static void relink( const plus::string& target, p7::fd_t dir_fd, const char* filename )
	{
		p7::unlinkat (         dir_fd, filename );
		p7::symlinkat( target, dir_fd, filename );
	}
	
	static void recursively_sync_directories( p7::fd_t     a_dirfd,
	                                          p7::fd_t     b_dirfd,
	                                          p7::fd_t     c_dirfd,
	                                          const char*  subpath );
	
	static void recursively_sync( p7::fd_t     a_dirfd,
	                              p7::fd_t     b_dirfd,
	                              p7::fd_t     c_dirfd,
	                              const char*  subpath,
	                              const char*  filename )
	{
		const mode_t a_mode = get_mode( a_dirfd, filename );
		const mode_t b_mode = get_mode( b_dirfd, filename );
		const mode_t c_mode = get_mode( c_dirfd, filename );
		
		const bool a_is_dir = S_ISDIR( a_mode );
		const bool b_is_dir = S_ISDIR( b_mode );
		const bool c_is_dir = S_ISDIR( c_mode );
		
		if ( bool matched = a_is_dir == c_is_dir  &&  (!b_mode || a_is_dir == b_is_dir) )
		{
			if ( a_is_dir )
			{
				if ( !b_mode )
				{
					p7::mkdirat( b_dirfd, filename );
				}
				
				recursively_sync_directories( open_dir( a_dirfd, filename ),
				                              open_dir( b_dirfd, filename ),
				                              open_dir( c_dirfd, filename ), subpath );
			}
			else
			{
				const bool a_is_link = S_ISLNK( a_mode );
				const bool b_is_link = S_ISLNK( b_mode );
				const bool c_is_link = S_ISLNK( c_mode );
				
				if ( !a_is_link  &&  !c_is_link )
				{
					if ( b_is_link )
					{
						p7::unlinkat( b_dirfd, filename );
						
						(void) p7::openat( b_dirfd, filename, p7::o_wronly | p7::o_creat, p7::_666 );
					}
					
					sync_files( a_dirfd, b_dirfd, c_dirfd, subpath, filename, b_mode );
				}
				else if ( a_is_link  &&  c_is_link )
				{
					const plus::string a_target = p7::readlinkat( a_dirfd, filename );
					const plus::string c_target = p7::readlinkat( c_dirfd, filename );
					
					const plus::string b_target = b_is_link ? p7::readlinkat( b_dirfd, filename ) : plus::string();
					
					if ( a_target == c_target )
					{
						if ( b_target != a_target )
						{
							std::printf( "%s %s\n", b_mode ? "----" : "+--+", subpath );
							
							if ( !global_dry_run )
							{
								if ( b_mode )
								{
									p7::unlinkat( b_dirfd, filename );
								}
								
								p7::symlinkat( a_target, b_dirfd, filename );
							}
						}
					}
					else if ( b_target == c_target )
					{
						std::printf( "%s %s\n", globally_up ? "<---" : "|---", subpath );
						
						if ( globally_up  &&  !global_dry_run )
						{
							relink( a_target, c_dirfd, filename );
							relink( a_target, b_dirfd, filename );
						}
					}
					else if ( b_target == a_target )
					{
						std::printf( "%s %s\n", globally_down ? "--->" : "---|", subpath );
						
						if ( globally_down  &&  !global_dry_run )
						{
							relink( c_target, a_dirfd, filename );
							relink( c_target, b_dirfd, filename );
						}
					}
					else
					{
						std::printf( "### %s %s\n", subpath, "symlink conflict" );
					}	
				}
				else
				{
					std::printf( "### %s %s\n", subpath, "symlink conversion" );
				}
			}
		}
		else if ( b_mode )
		{
			// file vs. directory
			if ( a_is_dir != b_is_dir )
			{
				std::printf( "### %s changed from %s to %s\n",
				                  subpath,        b_is_dir ? "directory" : "file",
				                                        a_is_dir ? "directory" : "file" );
			}
			
			if ( c_is_dir != b_is_dir )
			{
				std::printf( "### %s changed from %s to %s\n",
				                  subpath,        b_is_dir ? "directory" : "file",
				                                        c_is_dir ? "directory" : "file" );
			}
		}
		else
		{
			std::printf( "### Add conflict in %s (file vs. directory)\n", subpath );
		}
	}
	
	static void recursively_sync( p7::fd_t             a_dirfd,
	                              p7::fd_t             b_dirfd,
	                              p7::fd_t             c_dirfd,
	                              const plus::string&  subpath,
	                              const plus::string&  filename )
	{
		void (*f)( p7::fd_t,
		           p7::fd_t,
		           p7::fd_t,
		           const char*,
		           const char* ) = &recursively_sync;
		
		relix::recurse( f, a_dirfd,
		                   b_dirfd,
		                   c_dirfd,
		                   subpath.c_str(),
		                   filename.c_str() );
	}
	
	template < class In, class Out, class Pred >
	static Out copy_if( In begin, In end, Out result, Pred f )
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
	static Out copy_unless( In begin, In end, Out result, Pred f )
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
	
	template < class Sequence >
	static void compare_sequences( const Sequence& a,
	                               const Sequence& b,
	                               Sequence& a_only,
	                               Sequence& b_only,
	                               Sequence& both )
	{
		compare_sequences( a.begin(), a.end(),
		                   b.begin(), b.end(),
		                   std::back_inserter( a_only ),
		                   std::back_inserter( b_only ),
		                   std::back_inserter( both ) );
	}
	
	template < class Source, class Dest >
	static void copy_unless_filtered( const Source& source, Dest& dest )
	{
		copy_unless( source.begin(),
		             source.end(),
		             std::back_inserter( dest ),
		             plus::ptr_fun( filter_item ) );
	}
	
	template < class Sequence >
	static void sort( Sequence& sequence )
	{
		std::sort( sequence.begin(), sequence.end() );
	}
	
	static void recursively_delete( const plus::string& path )
	{
		io::recursively_delete( path );
	}
	
	static void recursively_sync_directory_contents( p7::fd_t             a_dirfd,
	                                                 p7::fd_t             b_dirfd,
	                                                 p7::fd_t             c_dirfd,
	                                                 const plus::string&  subpath )
	{
		typedef p7::directory_contents_container directory_container;
		
		directory_container a_contents = dirfd_contents( a_dirfd );
		directory_container b_contents = dirfd_contents( b_dirfd );
		directory_container c_contents = dirfd_contents( c_dirfd );
		
		std::vector< plus::string > a;
		std::vector< plus::string > b;
		std::vector< plus::string > c;
		
		copy_unless_filtered( a_contents, a );
		copy_unless_filtered( b_contents, b );
		copy_unless_filtered( c_contents, c );
		
		sort( a );
		sort( b );
		sort( c );
		
		std::vector< plus::string > a_added;
		std::vector< plus::string > a_removed;
		std::vector< plus::string > a_static;
		
		std::vector< plus::string > c_added;
		std::vector< plus::string > c_removed;
		std::vector< plus::string > c_static;
		
		compare_sequences( a,
		                   b,
		                   a_added,
		                   a_removed,
		                   a_static );
		
		compare_sequences( c,
		                   b,
		                   c_added,
		                   c_removed,
		                   c_static );
		
		std::vector< plus::string > a_created;
		std::vector< plus::string > c_created;
		std::vector< plus::string > mutually_added;
		
		std::vector< plus::string > a_deleted;
		std::vector< plus::string > c_deleted;
		std::vector< plus::string > mutually_deleted;
		
		std::vector< plus::string > mutually_static;
		
		compare_sequences( a_added,
		                   c_added,
		                   a_created,
		                   c_created,
		                   mutually_added );
		
		compare_sequences( a_removed,
		                   c_removed,
		                   a_deleted,
		                   c_deleted,
		                   mutually_deleted );
		
		compare_sequences( a_static.begin(), a_static.end(),
		                   c_static.begin(), c_static.end(),
		                   null_iterator(),
		                   null_iterator(),
		                   std::back_inserter( mutually_static ) );
		
		const char* path = subpath.c_str();
		
		typedef std::vector< plus::string >::const_iterator Iter;
		
		for ( Iter it = a_created.begin();  it != a_created.end();  ++ it )
		{
			const plus::string& filename = *it;
			
			const bool doable = globally_up;
			
			std::printf( "%s %s%s\n", doable ? "<+++" : "|+++", path, filename.c_str() );
			
			if ( doable && !global_dry_run )
			{
				globally_locking_files = false;
				
				recursively_copy( a_dirfd, filename, c_dirfd );
				
				globally_locking_files = true;
				
				recursively_copy( a_dirfd, filename, b_dirfd );
			}
		}
		
		for ( Iter it = c_created.begin();  it != c_created.end();  ++ it )
		{
			const plus::string& filename = *it;
			
			const bool doable = globally_down;
			
			std::printf( "%s %s%s\n", doable ? "+++>" : "+++|", path, filename.c_str() );
			
			if ( doable && !global_dry_run )
			{
				globally_locking_files = false;
				
				recursively_copy( c_dirfd, filename, a_dirfd );
				
				globally_locking_files = true;
				
				recursively_copy( c_dirfd, filename, b_dirfd );
			}
		}
		
		for ( Iter it = mutually_added.begin();  it != mutually_added.end();  ++ it )
		{
			const plus::string& filename = *it;
			
			std::printf( "++++ %s%s\n", path, filename.c_str() );
			
			recursively_sync( a_dirfd,
			                  b_dirfd,
			                  c_dirfd, subpath + filename, filename );
		}
		
		for ( Iter it = a_deleted.begin();  it != a_deleted.end();  ++ it )
		{
			const plus::string& filename = *it;
			
			const bool doable = globally_up && globally_deleting;
			
			plus::string child_subpath = subpath + filename;
			
			std::printf( "%s %s%s\n", doable ? "<--X" : "|--X", path, filename.c_str() );
			
			if ( doable && !global_dry_run )
			{
				plus::string b_path = global_base_root   / child_subpath;
				plus::string c_path = global_remote_root / child_subpath;
				
				recursively_delete( c_path );
				recursively_delete( b_path );
			}
		}
		
		for ( Iter it = c_deleted.begin();  it != c_deleted.end();  ++ it )
		{
			const plus::string& filename = *it;
			
			const bool doable = globally_down && globally_deleting;
			
			plus::string child_subpath = subpath + filename;
			
			std::printf( "%s %s%s\n", doable ? "X-->" : "X--|", path, filename.c_str() );
			
			if ( doable && !global_dry_run )
			{
				plus::string a_path = global_local_root / child_subpath;
				plus::string b_path = global_base_root  / child_subpath;
				
				recursively_delete( a_path );
				recursively_delete( b_path );
			}
		}
		
		for ( Iter it = mutually_deleted.begin();  it != mutually_deleted.end();  ++ it )
		{
			const plus::string& filename = *it;
			
			plus::string child_subpath = subpath + filename;
			
			std::printf( "X--X %s%s\n", path, filename.c_str() );
			
			if ( !global_dry_run )
			{
				plus::string b_path = global_base_root / child_subpath;
				
				recursively_delete( b_path );
			}
		}
		
		for ( Iter it = mutually_static.begin();  it != mutually_static.end();  ++ it )
		{
			const plus::string& filename = *it;
			
			recursively_sync( a_dirfd,
			                  b_dirfd,
			                  c_dirfd, subpath + filename, filename );
		}
		
		// added/nil:  simple add -- just do it
		// added/added:  mutual add -- check for conflicts
		// static/static:  check for conflicts
		// deleted/static:  single delete -- check for conflicts
		// deleted/deleted:  mutual delete -- just do it
	}
	
	static void recursively_sync_directories( p7::fd_t     a_dirfd,
	                                          p7::fd_t     b_dirfd,
	                                          p7::fd_t     c_dirfd,
	                                          const char*  subpath )
	{
		// compare any relevant metadata, like Desktop comment
		
		if ( globally_verbose )
		{
			std::printf( "%s\n", subpath );
		}
		
		plus::string subpath_dir = plus::concat( subpath, STR_LEN( "/" ) );
		
		recursively_sync_directory_contents( a_dirfd, b_dirfd, c_dirfd, subpath_dir );
	}
	
	
	static plus::string home_dir_pathname()
	{
		if ( const char* home = std::getenv( "HOME" ) )
		{
			return home;
		}
		
		return "/";
	}
	
	static plus::string get_jsync_root_pathname()
	{
		plus::string home = home_dir_pathname();
		
		const char* jsync = "Library/JSync";
		
		return mkdir_path( home / jsync );
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( !globally_up && !globally_down && !null )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "jsync: nothing to do; specify --up, --down, or -2,""\n"
			                                       "jsync: or pass -0 go through the motions anyway."  "\n" ) );
			
			return p7::exit_failure;
		}
		
		const char* default_path = "Default";
		
		const char* path = argn >= 1 ? args[0] : default_path;
		
		plus::string jsync_root = get_jsync_root_pathname();
		
		plus::var_string jsync_path = jsync_root / path;
		
		jsync_path += ".jsync";
		
		struct stat jsync_stat;
		
		const bool jsync_exists = p7::stat( jsync_path, jsync_stat );
		
		const bool jsync_is_dir = jsync_exists  &&  p7::s_isdir( jsync_stat );
		
		if ( !jsync_is_dir )
		{
			std::fprintf( stderr, "jsync: no such sync path '%s'\n"
			                      "(No such directory %s)\n",path,
			                      jsync_path.c_str() );
			
			return 1;
		}
		
		global_local_root  = jsync_path / "Sandbox";  // should be a link
		global_remote_root = jsync_path / "Remote";   // should be a link
		global_base_root   = jsync_path / "Base";
		
		recursively_sync_directory_contents( open_dir( global_local_root  ),
		                                     open_dir( global_base_root   ),
		                                     open_dir( global_remote_root ),
		                                     plus::string::null );
		
		return 0;
	}
	
}
