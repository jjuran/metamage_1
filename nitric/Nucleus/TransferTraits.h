// Nucleus/TransferTraits.h
// ------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_TRANSFERTRAITS_H
#define NUCLEUS_TRANSFERTRAITS_H

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
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
	template < class T, class Disposer > struct Transfer_Traits< ::nucleus::owned< T, Disposer > >
	{
		static const bool mayCopyConstSource = false;
		
		typedef ::nucleus::owned< T, Disposer > Type;
		
		typedef ::nucleus::ownership_transfer< T, Disposer > Transfer;
	};
	
}

#endif

