/*
	sha256.hh
	---------
*/

#ifndef SHA256_SHA256_HH
#define SHA256_SHA256_HH

// POSIX
#include <sys/types.h>

// sha256
#include "sha256/state.hh"


namespace crypto
{
	
	void sha256_init( sha256_state& state );
	
	void sha256_digest_block( sha256_state& state, void const* data );
	
	void sha256_finish( sha256_state&  state,
	                    void const*    data,
	                    size_t         n_bytes,
	                    int            n_more_bits = 0 );
	
	class sha256_engine
	{
		private:
			sha256_state state;
		
		public:
			sha256_engine()
			{
				sha256_init( state );
			}
			
			void digest_block( const void* data )  // 64 bytes
			{
				sha256_digest_block( state, data );
			}
			
			const sha256_hash& finish( const void*  data,
			                           size_t       n_bytes,
			                           int          n_more_bits = 0 )
			{
				sha256_finish( state, data, n_bytes, n_more_bits );
				
				return state.digest;
			}
	};
	
	sha256_hash sha256( const void* data, size_t n_bytes, int n_more_bits = 0 );
	
}

#endif
