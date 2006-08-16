// Selector.h

#ifndef NUCLEUS_SELECTOR_H
#define NUCLEUS_SELECTOR_H

#ifndef NUCLEUS_ENUMERATION_H
#include "Nucleus/Enumeration.h"
#endif


namespace Nucleus
{
	
	template < class Tag, class Int >
	struct Selector
	{
		typedef Enumeration< Int >::Type Type;
	};
	
}

#endif

