/*
	microseconds.hh
	---------------
*/

#ifndef MACSYS_MICROSECONDS_HH
#define MACSYS_MICROSECONDS_HH


namespace mac {
namespace sys {
	
	void microseconds( unsigned long long* count );
	
	inline
	unsigned long long microseconds()
	{
		unsigned long long count;
		
		microseconds( &count );
		
		return count;
	}
	
}
}

#endif
