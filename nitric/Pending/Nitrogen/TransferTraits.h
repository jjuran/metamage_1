// TransferTraits.h

#ifndef NITROGEN_TRANSFERTRAITS_H
#define NITROGEN_TRANSFERTRAITS_H

// Nitrogen Core
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif


namespace Nitrogen
{
	
	template < class T > struct Transfer_Traits
	{
		static const bool mayCopyConstSource = false;
		
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

