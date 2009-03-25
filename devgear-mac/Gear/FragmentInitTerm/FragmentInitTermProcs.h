/*	=======================
 *	FragmentInitTermProcs.h
 *	=======================
 */


#include <CodeFragments.h>

#ifndef FRAGMENT_INIT_TERM_PROCS
#ifdef __MWERKS__
	
	// MSL Runtime
	extern pascal OSErr __initialize( const CFragInitBlock* initBlock );
	extern pascal void  __terminate ();
	
	#define FRAGMENT_INIT_TERM_PROCS  { &__initialize, &__terminate }
	
#else
	
	#define FRAGMENT_INIT_TERM_PROCS
	
#endif
#endif

#ifndef FRAGMENT_INIT_TERM_PROCS_ARRAY
#define FRAGMENT_INIT_TERM_PROCS_ARRAY  { FRAGMENT_INIT_TERM_PROCS }
#endif

