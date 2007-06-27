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
	
	dummy::filename_result get_filename( dummy::file_spec, overload );
	
	template < class FileSpec >
	inline std::string get_filename_string( const FileSpec& file )
	{
		return Nucleus::Convert< std::string >( get_filename( file, overload() ) );
	}
	
	dummy::file_spec get_preceding_directory          ( dummy::file_spec, overload );
	dummy::file_spec get_parent_directory_of_directory( dummy::file_spec, overload );
	
	
	bool item_is_file     ( dummy::file_catalog_record, overload );
	bool item_is_directory( dummy::file_catalog_record, overload );
	
	bool item_exists     ( dummy::file_spec, overload );
	bool file_exists     ( dummy::file_spec, overload );
	bool directory_exists( dummy::file_spec, overload );
	
	bool item_exists     ( dummy::file_spec, dummy::file_catalog_record&, overload );
	bool file_exists     ( dummy::file_spec, dummy::file_catalog_record&, overload );
	bool directory_exists( dummy::file_spec, dummy::file_catalog_record&, overload );
	
	void delete_file     ( dummy::file_spec, overload );
	void delete_file_only( dummy::file_spec, overload );
	
	void delete_empty_directory     ( dummy::file_spec, overload );
	void delete_empty_directory_only( dummy::file_spec, overload );
	
	template < class FileSpec > struct system_root_getter;
	
	template < class FileSpec >
	inline FileSpec system_root()
	{
		return system_root_getter< FileSpec >()();
	}
	
	
	template < class Filename, class FileSpec >
	inline Filename get_filename( const FileSpec& file )
	{
		return get_filename( file, overload() );
	}
	
	template < class DirSpec, class FileSpec >
	inline DirSpec get_preceding_directory( const FileSpec& file )
	{
		return get_preceding_directory( file, overload() );
	}
	
	template < class DirSpec >
	inline DirSpec get_parent_directory_of_directory( const DirSpec& dir )
	{
		return get_parent_directory_of_directory( dir, overload() );
	}
	
	template < class FileCatalogRecord >
	inline bool item_is_file( const FileCatalogRecord& cat )
	{
		return item_is_file( cat, overload() );
	}
	
	template < class FileCatalogRecord >
	inline bool item_is_directory( const FileCatalogRecord& cat )
	{
		return item_is_directory( cat, overload() );
	}
	
	template < class FileSpec >
	inline bool item_exists( const FileSpec& file )
	{
		return item_exists( file, overload() );
	}
	
	template < class FileSpec >
	inline bool file_exists( const FileSpec& file )
	{
		return file_exists( file, overload() );
	}
	
	template < class FileSpec >
	inline bool directory_exists( const FileSpec& file )
	{
		return directory_exists( file, overload() );
	}
	
	template < class FileSpec, class FileCatalogRecord >
	inline bool item_exists( const FileSpec& file, FileCatalogRecord& cat )
	{
		return item_exists( file, cat, overload() );
	}
	
	template < class FileSpec, class FileCatalogRecord >
	inline bool file_exists( const FileSpec& file, FileCatalogRecord& cat )
	{
		return file_exists( file, cat, overload() );
	}
	
	template < class FileSpec, class FileCatalogRecord >
	inline bool directory_exists( const FileSpec& file, FileCatalogRecord& cat )
	{
		return directory_exists( file, cat, overload() );
	}
	
	template < class FileSpec >
	inline void delete_file( const FileSpec& file )
	{
		return delete_file( file, overload() );
	}
	
	template < class FileSpec >
	inline void delete_file_only( const FileSpec& file )
	{
		return delete_file_only( file, overload() );
	}
	
	template < class FileSpec >
	inline void delete_empty_directory( const FileSpec& file )
	{
		return delete_empty_directory( file, overload() );
	}
	
	template < class FileSpec >
	inline void delete_empty_directory_only( const FileSpec& file )
	{
		return delete_empty_directory_only( file, overload() );
	}
	
}

#endif

