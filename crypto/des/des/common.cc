/*
	common.cc
	---------
*/

#include "des/common.hh"

// iota
#include "iota/endian.hh"


#define LENGTH( array )  (sizeof (array) / sizeof *(array))

#define IP_DESCEND_2( last )  last + 8, last
#define IP_DESCEND_4( last )  IP_DESCEND_2( last + 2 * 8 ), IP_DESCEND_2( last )
#define IP_DESCEND_8( last )  IP_DESCEND_4( last + 4 * 8 ), IP_DESCEND_4( last )

#define IP_ASCEND_2( first )  IP_DESCEND_8( first ), IP_DESCEND_8( first + 2 )
#define IP_ASCEND_4( first )  IP_ASCEND_2 ( first ), IP_ASCEND_2 ( first + 4 )

#define FP_ASCEND_4( low )  low + 32, low, low + 40, low + 8
#define FP_ASCEND_8( low )  FP_ASCEND_4( low ), FP_ASCEND_4( low + 16 )

#define FP_DESCEND_2( low )  FP_ASCEND_8 ( low + 1 ), FP_ASCEND_8 ( low )
#define FP_DESCEND_4( low )  FP_DESCEND_2( low + 2 ), FP_DESCEND_2( low )
#define FP_DESCEND_8( low )  FP_DESCEND_4( low + 4 ), FP_DESCEND_4( low )

#define E_INCR_3( first )  first & 0x1f, first + 1 & 0x1f, first + 2 & 0x1f
#define E_INCR_6( first )  E_INCR_3( first ), E_INCR_3( first + 3 )

#define E_ASCEND_2( first )  E_INCR_6  ( first ), E_INCR_6  ( first +  4 )
#define E_ASCEND_4( first )  E_ASCEND_2( first ), E_ASCEND_2( first +  8 )
#define E_ASCEND_8( first )  E_ASCEND_4( first ), E_ASCEND_4( first + 16 )


namespace des
{

static uint8_t PC1_left[ 28 ] =
{
	56, 48, 40, 32, 24, 16,  8,
	 0, 57, 49, 41, 33, 25, 17,
	 9,  1, 58, 50, 42, 34, 26,
	18, 10,  2, 59, 51, 43, 35,
};

static uint8_t PC1_right[ 28 ] =
{
	62, 54, 46, 38, 30, 22, 14,
	 6, 61, 53, 45, 37, 29, 21,
	13,  5, 60, 52, 44, 36, 28,
	20, 12,  4, 27, 19, 11,  3,
};

static uint8_t PC2[ 48 ] =
{
	13, 16, 10, 23,  0,  4,
	 2, 27, 14,  5, 20,  9,
	22, 18, 11,  3, 25,  7,
	15,  6, 26, 19, 12,  1,
	40, 51, 30, 36, 46, 54,
	29, 39, 50, 44, 32, 47,
	43, 48, 38, 55, 33, 52,
	45, 41, 49, 35, 28, 31,
};

static uint8_t rotation_schedule[ 16 ] =
{
	1, 1, 2, 2,
	2, 2, 2, 2,
	1, 2, 2, 2,
	2, 2, 2, 1,
};

static uint8_t initial_permutation_table[ 64 ] =
{
	IP_ASCEND_4( 1 ),
	IP_ASCEND_4( 0 ),
};

static uint8_t final_permutation_table[ 64 ] =
{
	FP_DESCEND_8( 0 ),
};

static uint8_t expansion_table[ 48 ] =
{
	E_ASCEND_8( 31 ),
};

static uint8_t P_box[ 32 ] =
{
	15,  6, 19, 20,  28, 11, 27, 16,
	 0, 14, 22, 25,   4, 17, 30,  9,
	 1,  7, 23, 13,  31, 26,  2,  8,
	18, 12, 29,  5,  21, 10,  3, 24,
};

static uint8_t S_boxes[ 8 ][ 4 ][ 16 ] =
{
	{
		{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
		{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
		{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
		{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },
	},
	{
		{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
		{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
		{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
		{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },
	},
	{
		{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
		{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
		{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
		{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },
	},
	{
		{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
		{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
		{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
		{ 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },
	},
	{
		{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
		{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
		{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
		{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },
	},
	{
		{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
		{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
		{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
		{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },
	},
	{
		{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
		{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
		{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
		{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },
	},
	{
		{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
		{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
		{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
		{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 },
	},
};

static inline
uint32_t ROL_28( uint32_t x, int n )
{
	const uint32_t mask = (1ul << 28) - 1;
	
	return (x << n & mask) | x >> (28 - n);
}

static
uint64_t permute( uint8_t const map[], int n_out, const uint64_t& x, int n_in )
{
	uint64_t result = 0;
	
	for ( int i = 0;  i < n_out;  ++i )
	{
		int out_bit = n_out - 1 - i;
		int in_bit  = n_in  - 1 - map[ i ];
		
		result <<= 1;
		
		result |= x >> in_bit & 1;
	}
	
	return result;
}

#define PERMUTE( map, x, n_in )  permute( map, LENGTH( map ), x, n_in )

void make_subkeys( uint64_t* subkeys, const uint64_t& key )
{
	uint32_t left  = PERMUTE( PC1_left,  key, 64 );
	uint32_t right = PERMUTE( PC1_right, key, 64 );
	
	for ( int i = 0;  i < 16;  ++i )
	{
		uint8_t n = rotation_schedule[ i ];
		
		left  = ROL_28( left,  n );
		right = ROL_28( right, n );
		
		subkeys[ i ] = PERMUTE( PC2, (left + 0ull) << 28 | right, 56 );
	}
}

void initial_permutation( uint64_t& result, const uint64_t& data )
{
	result = PERMUTE( initial_permutation_table, data, 64 );
}

void final_permutation( uint64_t& result, const uint64_t& data )
{
	result = PERMUTE( final_permutation_table, data, 64 );
}

static
uint32_t f( uint32_t data, const uint64_t& key )
{
	uint64_t block = PERMUTE( expansion_table, data, 32 );
	
	block ^= key;
	
	uint32_t result = 0;
	
	for ( int i = 0;  i < 8;  ++i )
	{
		result <<= 4;
		
		unsigned coords = block >> (7 - i) * 6 & 0x3f;
		
		unsigned row = (coords & 0x20) >> 4 | (coords & 0x01);
		unsigned col = coords >> 1 & 0x0f;
		
		result |= S_boxes[ i ][ row ][ col ];
	}
	
	return PERMUTE( P_box, result, 32 );
}

void crypt_block_core( const uint64_t subkeys[ 16 ], uint64_t& data )
{
	using iota::high_u32;
	using iota::low_u32;
	
	uint32_t left  = high_u32( data );
	uint32_t right =           data;
	
	for ( int i = 0;  i < 16;  ++i )
	{
		uint32_t next = left ^ f( right, subkeys[ i ] );
		
		left = right;
		right = next;
	}
	
	high_u32( data ) = right;
	low_u32 ( data ) = left;
}

}  // namespace des
