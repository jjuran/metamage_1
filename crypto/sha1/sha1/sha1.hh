/*
	sha1.hh
	-------
*/

#ifndef SHA1_SHA1_HH
#define SHA1_SHA1_HH

// POSIX
#include <sys/types.h>

// sha1
#include "sha1/state.hh"


namespace crypto
{
	
	void sha1_init( sha1_state& state );
	
	void sha1_digest_block( sha1_state& state, void const* data );  // 64 bytes
	
	void sha1_finish( sha1_state&   state,
	                  void const*   data,
	                  size_t        n_bytes,
	                  int           n_more_bits = 0 );
	
	class sha1_engine
	{
		private:
			sha1_state state;
		
		public:
			sha1_engine()
			{
				sha1_init( state );
			}
			
			void digest_block( const void* data )  // 64 bytes
			{
				sha1_digest_block( state, data );
			}
			
			const sha1_digest& finish( const void*  data,
			                           size_t       n_bytes,
			                           int          n_more_bits = 0 )
			{
				sha1_finish( state, data, n_bytes, n_more_bits );
				
				return state.digest;
			}
	};
	
	sha1_digest sha1( const void* data, size_t n_bytes, int n_more_bits = 0 );
	
}

#endif
