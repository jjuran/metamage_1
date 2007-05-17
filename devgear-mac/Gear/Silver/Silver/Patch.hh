/*
	
	Patch.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCH_HH
#define SILVER_PATCH_HH

#include "Silver/PatchUtils.hh"
#include "Silver/PatchStub.hh"


namespace Silver
{
	
	template < UInt16 trapWord, typename TrapTraits< trapWord >::PatchProcPtr patch >
	class TrapPatch : private PatchStub< typename TrapTraits< trapWord >::PatchProcPtr, patch >
	{
		public:
			static void Install()
			{
				NextHandler() = ApplyTrapPatch( trapWord, Function );
			}
			
			static void Remove()
			{
				RemoveTrapPatch( trapWord, NextHandler() );
				
				NextHandler() = NULL;
			}
	};
	
}

#endif

