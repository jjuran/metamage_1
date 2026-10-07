/*
	bytes.hh
	--------
*/

#ifndef RLE1_BYTES_HH
#define RLE1_BYTES_HH


namespace rle1
{
	
	typedef   signed char codon_t;
	typedef unsigned char byte_t;
	typedef unsigned long size_t;
	
	size_t   compressed_bytes_size( const byte_t*  p, size_t n );
	size_t uncompressed_bytes_size( const codon_t* p, size_t n );
	
	void   compress_bytes( codon_t* out, const byte_t* in, size_t n );
	void decompress_bytes( byte_t* out, const codon_t* in, size_t n );
	
}

#endif
