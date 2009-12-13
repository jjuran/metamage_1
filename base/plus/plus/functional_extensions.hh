/*
	functional_extensions.hh
	------------------------
	
	Joshua Juran
*/

#ifndef PLUS_FUNCTIONALEXTENSIONS_HH
#define PLUS_FUNCTIONALEXTENSIONS_HH

// STL extensions
#ifdef __MWERKS__
#include <functional_ext>
#else
#include <ext/functional>
#endif


namespace plus
{
#ifdef __MWERKS__
	
	namespace functional_extensions = std;
	
#else
	
	namespace functional_extensions = __gnu_cxx;
	
#endif
	
	using functional_extensions::compose1;
	using functional_extensions::compose2;
	using functional_extensions::identity;
}

#endif

