/*
	source.hh
	---------
*/

#ifndef VLIB_SOURCE_HH
#define VLIB_SOURCE_HH


namespace vlib
{
	
	struct source_spec
	{
		const char*  file;
		unsigned     line;
		
		source_spec( const char* path ) : file( path ), line( 1 )
		{
		}
	};
	
}

#endif
