/*
	adler32.hh
	----------
*/

#ifndef ADLER32_ADLER32_HH
#define ADLER32_ADLER32_HH


namespace adler32
{
	
	typedef unsigned char byte_t;
	typedef unsigned long size_t;
	
#if __LP64__
	
	typedef unsigned int  hash_t;
	
#else
	
	typedef unsigned long hash_t;
	
#endif
	
	hash_t checksum( const byte_t* data, size_t n_bytes );
	
	inline
	hash_t checksum( const void* data, size_t n_bytes )
	{
		return checksum( (const byte_t*) data, n_bytes );
	}
	
}

#endif
