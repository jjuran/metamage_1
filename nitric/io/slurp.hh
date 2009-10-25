// io/slurp.hh
// -----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef IO_SLURP_HH
#define IO_SLURP_HH

#ifndef IO_IO_HH
#include "io/io.hh"
#endif


namespace io
{
	
	template < class details >
	class slurp_getter
	{
		private:
			typedef typename details::stream      stream;
			typedef typename details::byte_count  byte_count;
			
			stream its_input;
		
		public:
			slurp_getter( const stream& input ) : its_input( input )
			{
			}
			
			byte_count size() const
			{
				return get_file_size( its_input, overload() );
			}
			
			void operator()( void *begin, void *end ) const
			{
				(void) read( its_input, (char*) begin, distance( begin, end ) );
			}
	};
	
	template < class Flattener, class Stream >
	typename Flattener::Result slurp_input( const Stream& stream )
	{
		slurp_getter< io::iostream_traits< Stream > > getter( stream );
		
		return Flattener::Get( getter );
	}
	
	template < class Flattener, class FileSpec >
	typename Flattener::Result slurp_file( const FileSpec& file )
	{
		return slurp_input< Flattener >( open_for_reading( file, overload() ).get() );
	}
	
}

#endif

