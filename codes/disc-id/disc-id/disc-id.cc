/*
	disc-id.cc
	----------
*/

#include "disc-id/disc-id.hh"


namespace disc_id
{

static inline
int digit_sum( start_t x )
{
	int sum = 0;

	while ( x > 0 )
	{
		sum += x % 10;
		
		x /= 10;
	}

	return sum;
}

hash_t hash( int n, const start_t* starts, start_t end )
{
	hash_t result = end << 8 | n;
	
	hash_t sum = 0;
	
	while ( n-- > 0 )
	{
		sum += digit_sum( *starts++ );
	}
	
	result |= sum % 0xFF << 24;
	
	return result;
}

}
