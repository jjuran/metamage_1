// io/walk.hh

#ifndef IO_WALK_HH
#define IO_WALK_HH

// Standard C++
#include <string>

// Nucleus
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif

// Io
#ifndef IO_FILES_HH
#include "io/files.hh"
#endif



namespace io
{
	
	template < class FileSpec > struct directory_contents_traits;
	
	namespace dummy
	{
		
		class directory_stream {};
		
	}
	
	dummy::directory_stream directory_contents( dummy::file_spec );
	
	void recursively_delete                   ( dummy::file_spec );
	void recursively_delete_directory         ( dummy::file_spec );
	void recursively_delete_directory_contents( dummy::file_spec );
	
	template < class FileSpec >
	void recursively_delete( FileSpec item )
	{
		if ( file_exists( item ) )
		{
			delete_file( item );
		}
		else
		{
			recursively_delete_directory( item );
		}
	}
	
	template < class DirSpec >
	void recursively_delete_directory( DirSpec dir )
	{
		recursively_delete_directory_contents( dir );
		
		delete_empty_directory( dir );
	}
	
	template < class DirSpec >
	void recursively_delete_directory_contents( DirSpec item )
	{
		typedef typename filespec_traits< DirSpec >::file_spec file_spec;
		
		typedef typename directory_contents_traits< DirSpec >::container_type directory_container;
		
		directory_container contents = directory_contents( item );
		
		std::for_each( contents.begin(),
		               contents.end(),
		               std::ptr_fun( static_cast< void (*)(file_spec) >( recursively_delete ) ) );
	}
	
	template < class F1, class F2 >
	void walk_tree( dummy::file_spec, F1 visit_before, F2 visit_after );
	
	template < class F1, class F2, class F3 >
	void walk_tree( dummy::file_spec, F1 visit_dir_down,
	                                  F2 visit_file,
	                                  F3 visit_dir_up );
	
}

#endif

