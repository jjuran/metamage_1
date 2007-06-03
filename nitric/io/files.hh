// io/files.hh
// -----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef IO_FILES_HH
#define IO_FILES_HH

// Standard C++
#include <string>

// Nucleus
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif

// Io
#ifndef IO_IO_HH
#include "io/io.hh"
#endif



namespace io
{
	
	namespace dummy
	{
		
		class filename_parameter  {};
		class filename_result     {};
		
		class file_catalog_record  {};
		
	}
	
	dummy::filename_result get_filename( dummy::file_spec );
	
	template < class FileSpec >
	inline std::string get_filename_string( const FileSpec& file )
	{
		return Nucleus::Convert< std::string >( get_filename( file ) );
	}
	
	dummy::file_spec get_preceding_directory          ( dummy::file_spec );
	dummy::file_spec get_parent_directory_of_directory( dummy::file_spec );
	
	
	bool item_is_file     ( dummy::file_catalog_record );
	bool item_is_directory( dummy::file_catalog_record );
	
	bool item_exists     ( dummy::file_spec );
	bool file_exists     ( dummy::file_spec );
	bool directory_exists( dummy::file_spec );
	
	bool item_exists     ( dummy::file_spec, dummy::file_catalog_record& );
	bool file_exists     ( dummy::file_spec, dummy::file_catalog_record& );
	bool directory_exists( dummy::file_spec, dummy::file_catalog_record& );
	
	void delete_file     ( dummy::file_spec );
	void delete_file_only( dummy::file_spec );
	
	void delete_empty_directory     ( dummy::file_spec );
	void delete_empty_directory_only( dummy::file_spec );
	
	template < class FileSpec > struct system_root_getter;
	
	template < class FileSpec >
	inline FileSpec system_root()
	{
		return system_root_getter< FileSpec >()();
	}
	
}

#endif

