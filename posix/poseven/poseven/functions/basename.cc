/*
	basename.cc
	-----------
*/

#include "poseven/functions/basename.hh"

// gear
#include "gear/find.hh"


namespace poseven
{
	
	plus::string basename( const plus::string& path )
	{
		const std::size_t path_size = path.size();
		
		if ( path_size == 0 )
		{
			return ".";
		}
		
		const char* const data = path.data();
		
		const char* begin = data;
		const char* end   = data + path_size;
		
		while ( end != begin  &&  end[ -1 ] == '/' )
		{
			--end;
		}
		
		if ( end == begin )
		{
			// entire string consists of '/' bytes
			
			return "/";
		}
		
		if ( const char* last_slash = gear::find_last_match( begin, end, '/' ) )
		{
			begin = last_slash + 1;
		}
		
		return plus::string( path, begin - data, end - begin );
	}
	
}

