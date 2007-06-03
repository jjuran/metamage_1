// io/spray.hh

#ifndef IO_SPRAY_HH
#define IO_SPRAY_HH

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef IO_IO_HH
#include "io/io.hh"
#endif


namespace io
{
	
	class incomplete_spray {};
	
	template < class details >
	class spray_putter
	{
		private:
			typedef typename details::file_spec   file_spec;
			typedef typename details::stream      stream;
			typedef typename details::byte_count  byte_count;
			
			Nucleus::Owned< stream > itsOutput;
		
		public:
			spray_putter( const file_spec& file ) : itsOutput( io::open_for_writing< Nucleus::Owned< stream > >( file ) )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				byte_count bytesToWrite = distance( begin, end );
				
				byte_count bytesRead = io::write< byte_count, stream >( itsOutput, (const char*) begin, bytesToWrite );
				
				if ( bytesRead != bytesToWrite )
				{
					throw incomplete_spray();
				}
			}
	};
	
	template < class Flattener, class FileSpec >
	void spray_file( const FileSpec& file, typename Flattener::Parameter param )
	{
		spray_putter< io::filespec_traits< FileSpec > > putter( file );
		
		Flattener().Put( param, putter );
	}
	
}

#endif

