/*	======================
 *	FunctionalExtensions.h
 *	======================
 */

#ifndef NITROGEN_FUNCTIONALEXTENSIONS_H
#define NITROGEN_FUNCTIONALEXTENSIONS_H

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

