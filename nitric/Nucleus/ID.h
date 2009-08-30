// Nucleus/ID.h
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


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

