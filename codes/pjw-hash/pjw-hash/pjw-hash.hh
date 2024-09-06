/*
	pjw-hash.hh
	-----------
*/

#ifndef PJWHASH_PJWHASH_HH
#define PJWHASH_PJWHASH_HH


namespace pjw_hash
{
	
	typedef unsigned char byte_t;
	typedef unsigned long size_t;
	
#if __LP64__
	
	typedef unsigned int  hash_t;
	
#else
	
	typedef unsigned long hash_t;
	
#endif
	
#ifdef __MC68K__
#pragma parameter function( __A0 )
#endif
	
	hash_t function( const char* s );
	
#ifdef __MC68K__
#pragma parameter function( __A0, __D0 )
#endif
	
	hash_t function( const byte_t* p, size_t n_bytes );
	
	inline
	hash_t function( const void* p, size_t n_bytes )
	{
		return function( (const byte_t*) p, n_bytes );
	}
	
}

#endif
