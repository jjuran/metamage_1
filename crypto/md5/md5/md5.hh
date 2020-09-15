/*
	md5.hh
	------
*/

#ifndef MD5_MD5_HH
#define MD5_MD5_HH

// POSIX
#include <sys/types.h>

// md5
#include "md5/state.hh"


namespace crypto
{
	
	void md5_init( md5_state& state );
	
	void md5_digest_block( md5_state& state, void const* data );  // 64 bytes
	
	void md5_finish( md5_state&   state,
	                 void const*  data,
	                 size_t       n_bytes,
	                 int          n_more_bits = 0 );
	
	class md5_engine
	{
		private:
			md5_state state;
		
		public:
			md5_engine()
			{
				md5_init( state );
			}
			
			void digest_block( const void* data )  // 64 bytes
			{
				md5_digest_block( state, data );
			}
			
			const md5_digest& finish( const void*  data,
			                          size_t       n_bytes,
			                          int          n_more_bits = 0 )
			{
				md5_finish( state, data, n_bytes, n_more_bits );
				
				return state.digest;
			}
	};
	
	md5_digest md5( const void* data, size_t n_bytes, int n_more_bits = 0 );
	
}

#endif
