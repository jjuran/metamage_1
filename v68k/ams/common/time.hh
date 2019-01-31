/*
	time.hh
	-------
*/

#ifndef TIME_HH
#define TIME_HH

struct timeval;

// (100 * 1M us/s) / (100 * 60.15 t/s) = 20000000/1203 us/t (approx. 16625)

const unsigned long long tick_microseconds = 1000 * 1000 * 100 / 6015;

unsigned long long microseconds( const timeval& tv );

unsigned long long time_microseconds();

#endif
