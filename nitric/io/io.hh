// io/io.hh

#ifndef IO_IO_HH
#define IO_IO_HH

namespace io
{
	
	class end_of_input {};
	
	class no_input_pending {};
	
	
	inline ptrdiff_t distance( const void* begin, const void* end )
	{
		return (uintptr_t) end - (uintptr_t) begin;
	}
	
	
	template < class FileSpec > struct filespec_traits;
	
	namespace dummy
	{
		
		class file_spec     {};
		class stream        {};
		class owned_stream  {};
		class byte_count    {};
		
	}
	
	dummy::owned_stream open_for_reading( dummy::file_spec );
	dummy::owned_stream open_for_writing( dummy::file_spec );
	
	dummy::byte_count get_file_size( dummy::stream );
	
	dummy::byte_count read( dummy::stream, char*, dummy::byte_count );
	
	dummy::byte_count write( dummy::stream, const char*, dummy::byte_count );
	
}

#endif

