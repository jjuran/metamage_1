/*	======================
 *	FunctionalExtensions.h
 *	======================
 */

#ifndef TEMPLATES_FUNCTIONALEXTENSIONS_H
#define TEMPLATES_FUNCTIONALEXTENSIONS_H

// STL extensions
#ifdef __MWERKS__
#include <functional_ext>
#else
#include <ext/functional>
#endif


namespace Nitrogen
{
#ifdef __MWERKS__
	
	namespace STLExtensions = std;
	
#else
	
	namespace STLExtensions = __gnu_cxx;
	
#endif
}

#endif

