// io/walk.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef IO_WALK_HH
#define IO_WALK_HH

// Standard C++
#include <algorithm>

// Io
#ifndef IO_FILES_HH
#include "io/files.hh"
#endif



namespace io
{
	
	template < class FileSpec > struct directory_contents_traits;
	
	namespace you_missed_some_includes  // dummy
	{
		
		class directory_stream {};
		
	}
	
	dummy::directory_stream directory_contents( dummy::file_spec );
	
	struct walk_noop
	{
		template < class Param >
		bool operator()( const Param&, unsigned depth ) const
		{
			return true;
		}
	};
	
	template < class DirSpec, class F1, class F2, class F3 >
	void recursively_walk_subtrees( const DirSpec&  dir,
	                                const F1&       visit_dir_before,
	                                const F2&       visit_file,
	                                const F3&       visit_dir_after,
	                                unsigned        depth = 0 );
	
	template < class FileSpec, class F1, class F2, class F3 >
	void recursively_walk_tree( const FileSpec&  spec,
	                            const F1&        visit_dir_before,
	                            const F2&        visit_file,
	                            const F3&        visit_dir_after,
	                            unsigned         depth = 0 )
	{
		if ( file_exists( spec, overload() ) )
		{
			visit_file( spec, depth );
		}
		else if ( visit_dir_before( spec, depth ) )
		{
			recursively_walk_subtrees( spec,
			                           visit_dir_before,
			                           visit_file,
			                           visit_dir_after,
			                           depth );
			
			visit_dir_after( spec, depth );
		}
	}
	
	template < class DirSpec, class F1, class F2, class F3 >
	void recursively_walk_subtrees( const DirSpec&  dir,
	                                const F1&       visit_dir_before,
	                                const F2&       visit_file,
	                                const F3&       visit_dir_after,
	                                unsigned        depth = 0 )
	{
		typedef typename filespec_traits< DirSpec >::file_spec file_spec;
		
		typedef typename directory_contents_traits< DirSpec >::container_type directory_container;
		
		directory_container contents = directory_contents( dir, overload() );
		
		++depth;
		
		typedef typename directory_container::const_iterator Iter;
		
		for ( Iter it = contents.begin();  it != contents.end();  ++it )
		{
			recursively_walk_tree( path_descent( dir, *it ),
			                       visit_dir_before,
			                       visit_file,
			                       visit_dir_after,
			                       depth );
		}
	}
	
	void recursively_delete                   ( dummy::file_spec );
	void recursively_delete_directory         ( dummy::file_spec );
	void recursively_delete_directory_contents( dummy::file_spec );
	
	template < class FileSpec >
	struct file_deleter
	{
		void operator()( const FileSpec& spec, unsigned depth ) const
		{
			delete_file( spec );
		}
	};
	
	template < class FileSpec >
	struct directory_deleter
	{
		void operator()( const FileSpec& spec, unsigned depth ) const
		{
			delete_empty_directory( spec );
		}
	};
	
	template < class FileSpec >
	inline void recursively_delete( const FileSpec& item )
	{
		recursively_walk_tree( item,
		                       walk_noop(),
		                       file_deleter     < FileSpec >(),
		                       directory_deleter< FileSpec >() );
	}
	
	template < class DirSpec >
	void recursively_delete_directory( const DirSpec& dir )
	{
		recursively_delete_directory_contents( dir );
		
		delete_empty_directory( dir );
	}
	
	template < class DirSpec >
	void recursively_delete_directory_contents( const DirSpec& item )
	{
		typedef typename filespec_traits< DirSpec >::file_spec file_spec;
		
		recursively_walk_subtrees( item,
		                           walk_noop(),
		                           file_deleter     < file_spec >(),
		                           directory_deleter< file_spec >() );
	}
	
}

#endif

