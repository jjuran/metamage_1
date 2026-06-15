/*
	d1904z.hh
	---------
*/

#ifndef D1904Z_HH
#define D1904Z_HH

// Standard C
#include <time.h>


time_t        time_from_d1904( unsigned long local_1904  );
unsigned long d1904_from_time( time_t        global_1970 );

unsigned long d1904z_from_d1904( unsigned long local_1904  );
unsigned long d1904_from_d1904z( unsigned long global_1904 );

#endif
