// io/slurp.hh

#ifndef IO_SLURP_HH
#define IO_SLURP_HH

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif


namespace io
{
	
	inline ptrdiff_t distance( const void* begin, const void* end )
	{
		return (uintptr_t) end - (uintptr_t) begin;
	}
	
	
	template < class details >
	class slurp_getter
	{
		private:
			typedef typename details::file_spec   file_spec;
			typedef typename details::stream      stream;
			typedef typename details::byte_count  byte_count;
			
			Nucleus::Owned< stream > itsInput;
		
		public:
			slurp_getter( const file_spec& file ) : itsInput( io::open_for_reading( file ) )  {}
			
			byte_count size() const  { return io::get_file_size( itsInput ); }
			
			void operator()( void *begin, void *end ) const
			{
				(void) io::read( itsInput, (char*) begin, distance( begin, end ) );
			}
	};
	
	template < class Flattener, class FileSpec >
	typename Flattener::Result slurp_file( const FileSpec& file )
	{
		slurp_getter< io::filespec_traits< FileSpec > > getter( file );
		
		return Flattener().Get( getter );
	}
	
}

#endif

