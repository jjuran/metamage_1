// io/io.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef IO_IO_HH
#define IO_IO_HH

// Standard C
#include <stddef.h>
#include <stdint.h>


namespace io
{
	
	class end_of_input {};
	
	class no_input_pending {};
	
	
	inline ptrdiff_t distance( const void* begin, const void* end )
	{
		return (uintptr_t) end - (uintptr_t) begin;
	}
	
	
	template < class FileSpec > struct filespec_traits;
	
	namespace you_missed_some_includes
	{
		
		class file_spec     {};
		class stream        {};
		class owned_stream  {};
		class byte_count    {};
		
	}
	
	namespace dummy = you_missed_some_includes;
	
	class overload {};
	
	dummy::owned_stream open_for_reading( dummy::file_spec, overload );
	dummy::owned_stream open_for_writing( dummy::file_spec, overload );
	
	dummy::byte_count get_file_size( dummy::stream, overload );
	
	dummy::byte_count read( dummy::stream, char*, dummy::byte_count, overload );
	
	dummy::byte_count write( dummy::stream, const char*, dummy::byte_count, overload );
	
	template < class OwnedStream, class FileSpec >
	inline OwnedStream open_for_reading( FileSpec file )
	{
		return open_for_reading( file, overload() );
	}
	
	template < class OwnedStream, class FileSpec >
	inline OwnedStream open_for_writing( FileSpec file )
	{
		return open_for_writing( file, overload() );
	}
	
	template < class ByteCount, class Stream >
	inline ByteCount get_file_size( Stream stream )
	{
		return get_file_size( stream, overload() );
	}
	
	template < class ByteCountOut, class Stream, class ByteCountIn >
	inline ByteCountOut read( Stream stream, char* data, ByteCountIn byteCount )
	{
		return read( stream, data, byteCount, overload() );
	}
	
	template < class ByteCountOut, class Stream, class ByteCountIn >
	inline ByteCountOut write( Stream stream, const char* data, ByteCountIn byteCount )
	{
		return write( stream, data, byteCount, overload() );
	}
	
}

#endif

