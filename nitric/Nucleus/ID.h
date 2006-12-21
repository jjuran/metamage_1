// ID.h

#ifndef NUCLEUS_ID_H
#define NUCLEUS_ID_H

#ifndef NUCLEUS_ENUMERATION_H
#include "Nucleus/Enumeration.h"
#endif


namespace Nucleus
{
	
	template < class Tag, class Int >
	struct ID
	{
		typedef typename Enumeration< Tag, Int >::Type Type;
	};
	
}

#endif

