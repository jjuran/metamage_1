/*
	
	Patch.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCH_HH
#define SILVER_PATCH_HH

#include "Silver/PatchUtils.hh"
#include "Silver/PatchStub.hh"
#include "Silver/Traps.hh"


namespace Silver
{
	
	template < UInt16 trapWord, typename TrapTraits< trapWord >::PatchProcPtr patch >
	class TrapPatch
	:
		private PatchAdapter< TrapTraits< trapWord >::arity >::Glue< trapWord, patch >
	{
		public:
			static void Install()
			{
				Next::proc = ApplyTrapPatch( trapWord, Function );
			}
			
			static void Remove()
			{
				RemoveTrapPatch( trapWord, Next::proc );
				
				Next::proc = NULL;
			}
	};
	
}

#endif

