// TransferTraits.h

#ifndef NUCLEUS_TRANSFERTRAITS_H
#define NUCLEUS_TRANSFERTRAITS_H

// Nitrogen Core
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif


namespace Nucleus
{
	
	template < class T > struct Transfer_Traits
	{
		static const bool mayCopyConstSource = true;
		
		typedef T Type;
		
		typedef const T& Transfer;
	};
	
	// FIXME:  This should be moved to Owned.h
	template < class T, class Disposer > struct Transfer_Traits< Owned< T, Disposer > >
	{
		static const bool mayCopyConstSource = false;
		
		typedef Owned< T, Disposer > Type;
		
		typedef OwnershipTransfer< T, Disposer > Transfer;
	};
	
}

#endif

