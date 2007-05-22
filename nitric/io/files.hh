// io/files.hh

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
		
	}
	
	dummy::filename_result get_filename( dummy::file_spec );
	
	template < class FileSpec >
	inline std::string get_filename_string( const FileSpec& file )
	{
		return Nucleus::Convert< std::string >( get_filename( file ) );
	}
	
	dummy::file_spec get_preceding_directory          ( dummy::file_spec );
	dummy::file_spec get_parent_directory_of_directory( dummy::file_spec );
	
}

#endif

