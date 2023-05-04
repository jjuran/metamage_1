/*
	MSL_exit.c
	----------
*/

#ifdef __MWERKS__

// MSL
#include "abort_exit.h"

// <unistd.h>
extern void _exit( int status );


/*
	Override __atexit() to fail linking if it's called.
*/

int MSL_private_atexit_is_undefined();

int __atexit( void (*func)(void) )
{	
	// Not supported because it isn't used
	return MSL_private_atexit_is_undefined();
}

// Override __exit() to call POSIX _exit().

void __exit( int status )
{
	_exit( status );  // Doesn't return
}

#if defined( __MC68K__ )  &&  !__A5__

/*
	We need to override exit() for 68K MacRelix tools,
	since MSL specifically makes a point of omitting
	the call to __destroy_global_chain() in A4 code.
*/

extern int  atexit( void (*func)(void) );
extern void exit( int status );
extern void __destroy_global_chain( void );

#define max_funcs	64

static void (*atexit_funcs[ max_funcs ])(void);
static long	atexit_curr_func = 0;

void (*__stdio_exit)(void) = 0;


int atexit( void (*func)(void) )
{
	if ( atexit_curr_func == max_funcs )
	{
		return -1;
	}
	
	atexit_funcs[ atexit_curr_func++ ] = func;
	
	return 0;
}


void exit( int status )
{
	while ( atexit_curr_func > 0 )
	{
		(*atexit_funcs[ --atexit_curr_func ])();
	}
	
	__destroy_global_chain();
	
	if ( __stdio_exit )
	{
		(*__stdio_exit)();
		
		__stdio_exit = 0;
	}
	
	_exit( status );
}

#endif  // #if defined( __MC68K__ )  &&  !__A5__

#endif  // #ifdef __MWERKS__
