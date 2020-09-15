/*
	pack_body.hh
	------------
*/

#ifdef TOTAL
#define TALLY( x )  (TOTAL) += x
#else
#define TALLY( x )  /**/
#endif

#if defined( WRITE_1 )  &&  ! defined( WRITE_N )
#error "WRITE_1 defined, but WRITE_N isn't -- define both or neither"
#endif

#if defined( WRITE_N )  &&  ! defined( WRITE_1 )
#error "WRITE_N defined, but WRITE_1 isn't -- define both or neither"
#endif

#ifndef WRITE_1
#define WRITE_1( c )  /**/
#endif

#ifndef WRITE_N
#define WRITE_N( src, n )  /**/
#endif

{
	typedef unsigned char byte_t;
	
	while ( src < end )
	{
		/*
			Search for repeating pairs of identical bytes to compress.
		*/
		
		const byte_t* pair_x2 = find_next_aligned_2pair( src, end );
		
		if ( pair_x2 != src )
		{
			/*
				Either we found repeating pairs or hit the end, but either way
				we have preceding non-repeating data.
			*/
			
			size_t n = (pair_x2 ? pair_x2 : end) - src;
			
			/*
				Runs of non-repeating data are limited to 256 bytes.  Each
				run has two bytes of overhead.
			*/
			
			while ( n >= 256 )
			{
				WRITE_1( 0x00 );
				WRITE_1( 0x80 );
				
				WRITE_N( src, 256 );
				
				src += 256;
				n   -= 256;
				
				TALLY( 258 );
			}
			
			if ( n )
			{
				WRITE_1( 0x00   );
				WRITE_1( -n / 2 );
				
				WRITE_N( src, n );
				
				src += n;
				
				TALLY( 2 + n );
			}
			
			/*
				If we're out of data to compress, exit the loop.
			*/
			
			if ( ! pair_x2 )  break;
		}
		
		/*
			Find where the repeating pairs end.
		*/
		
		const byte_t* repeat_end = advance_repeated_pairs( pair_x2, end );
		
		src = repeat_end;
		
		size_t n = repeat_end - pair_x2;
		
		const char c = *pair_x2;
		
		/*
			Long runs of repeating bytes are limited to 65536 bytes.  Long runs
			are encoded in four bytes.
		*/
		
		while ( n >= 65536 )
		{
			n -= 65536;
			
			WRITE_1( 0x00 );
			WRITE_1( 0x7F );
			WRITE_1( 0xFF );
			WRITE_1( c    );
			
			TALLY( 4 );
		}
		
		/*
			The shortest possible long run is 514 bytes.  For anything less,
			use one or two short runs.
		*/
		
		if ( n > 512 )
		{
			WRITE_1( 0x00 );
			WRITE_1( (n/2 - 1) >> 8 );
			WRITE_1(  n/2 - 1       );
			WRITE_1( c );
			
			TALLY( 4 );
			
			continue;
		}
		
		/*
			For runs between 260 and 512 bytes, use two short runs of which the
			first is 256.
			
			A run of 258 repeating bytes is a special case.  We can't use the
			same formula as for 260 bytes, because with 258, subtracting 256
			leaves a difference of two bytes, which can't be represented as a
			short run.  Make the first short run 254 bytes instead.
		*/
		
		if ( n > 258 )
		{
			WRITE_1( 0x7F );
			WRITE_1( c    );
			
			TALLY( 2 );
			
			n -= 256;
		}
		else if ( n == 258 )
		{
			WRITE_1( 0x7E );  // 254
			WRITE_1( c    );
			
			TALLY( 2 );
			
			n = 258 - 254;
		}
		
		if ( n )
		{
			WRITE_1( n/2 - 1 );
			WRITE_1( c       );
			
			TALLY( 2 );
		}
	}
}

#undef TALLY
#undef TOTAL
#undef WRITE_1
#undef WRITE_N
