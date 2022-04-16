/*
	time.hh
	-------
*/

#ifndef TIME_HH
#define TIME_HH

struct timeval;

void time( timeval* tv );

timeval timeval_sub( const timeval& a, const timeval& b );

bool timeval_less( const timeval& a, const timeval& b );

static inline
bool operator <( const timeval& a, const timeval& b )
{
	return timeval_less( a, b );
}

static inline
bool operator >( const timeval& a, const timeval& b )
{
	return timeval_less( b, a );
}

static inline
bool operator <=( const timeval& a, const timeval& b )
{
	return ! timeval_less( b, a );
}

// (100 * 1M us/s) / (100 * 60.15 t/s) = 20000000/1203 us/t (approx. 16625)

const unsigned long long tick_microseconds = 1000 * 1000 * 100 / 6015;

#endif
