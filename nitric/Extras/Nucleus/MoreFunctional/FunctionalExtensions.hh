/*	=======================
 *	FunctionalExtensions.hh
 *	=======================
 */

#ifndef MOREFUNCTIONAL_FUNCTIONALEXTENSIONS_HH
#define MOREFUNCTIONAL_FUNCTIONALEXTENSIONS_HH

// STL extensions
#ifdef __MWERKS__
#include <functional_ext>
#else
#include <ext/functional>
#endif


namespace more
{
#ifdef __MWERKS__
	
	namespace functional_extensions = std;
	
#else
	
	namespace functional_extensions = __gnu_cxx;
	
#endif
	
	using functional_extensions::compose1;
}

#endif

