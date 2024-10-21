/*
	load.cc
	-------
*/


#include "poseven/extras/load.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/extras/read_all.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/open.hh"


namespace poseven
{
	
	static inline
	plus::string load( fd_t fd )
	{
		struct stat st = fstat( fd );
		
		if ( ! S_ISREG( st.st_mode ) )
		{
			throw_errno( ESPIPE );
		}
		
		const size_t size = st.st_size;
		
		plus::string result;
		
		char* p = result.reset( size );
		
		const ssize_t n_read = read_all( fd, p, size );
		
		return result.substr( 0, n_read );
	}
	
	plus::string load( const char* path )
	{
		return load( open( path, o_rdonly | o_nonblock ) );
	}
	
}
