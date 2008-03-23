// io/spew.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef IO_SPEW_HH
#define IO_SPEW_HH

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef IO_IO_HH
#include "io/io.hh"
#endif


namespace io
{
	
	class incomplete_spew {};
	
	template < class details >
	class spew_putter
	{
		private:
			typedef typename details::file_spec   file_spec;
			typedef typename details::stream      stream;
			typedef typename details::byte_count  byte_count;
			
			Nucleus::Owned< stream > itsOutput;
		
		public:
			spew_putter( const file_spec& file ) : itsOutput( io::open_for_writing< Nucleus::Owned< stream > >( file ) )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				byte_count bytesToWrite = distance( begin, end );
				
				byte_count bytesRead = io::write< byte_count, stream >( itsOutput, (const char*) begin, bytesToWrite );
				
				if ( bytesRead != bytesToWrite )
				{
					throw incomplete_spew();
				}
			}
	};
	
	template < class Flattener, class FileSpec >
	void spew_file( const FileSpec& file, typename Flattener::Parameter param )
	{
		spew_putter< io::filespec_traits< FileSpec > > putter( file );
		
		Flattener().Put( param, putter );
	}
	
}

#endif

