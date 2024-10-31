/*
	slurp.cc
	--------
*/


#include "poseven/extras/slurp.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/extras/read_all.hh"
#include "poseven/functions/fstat.hh"
#include "poseven/functions/open.hh"


namespace poseven
{
	
	plus::string slurp( fd_t fd )
	{
		struct stat st = fstat( fd );
		
		if ( ! S_ISREG( st.st_mode ) )
		{
			plus::var_string result;
			
			const size_t block_size = 4096;
			
			char buffer[ block_size ];
			
			while ( ssize_t bytes = read_all( fd, buffer, block_size ) )
			{
				result.append( buffer, bytes );
			}
			
			return result;
		}
		
		const size_t size = st.st_size;
		
		plus::string result;
		
		char* p = result.reset( size );
		
		const ssize_t n_read = read_all( fd, p, size );
		
		return result.substr( 0, n_read );
	}
	
	plus::string slurp( const char* path )
	{
		return slurp( open( path, o_rdonly ) );
	}
	
}
