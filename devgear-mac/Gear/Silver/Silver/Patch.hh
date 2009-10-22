/*
	
	Patch.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCH_HH
#define SILVER_PATCH_HH

#include "Silver/PatchUtils.hh"
#include "Silver/PatchAdapterGlue.hh"
#include "Silver/Traps.hh"


namespace Silver
{
	
	template < UInt16 trap, typename TrapTraits< trap >::PatchProcPtr patch >
	class TrapPatch
	:
		private PatchAdapter< TrapTraits< trap >::arity >::Glue< trap, patch >
	{
		public:
			static void Install()
			{
				Next::proc = ApplyTrapPatch( trap, Function );
			}
			
			static void Remove()
			{
				RemoveTrapPatch( trap, Next::proc );
				
				Next::proc = NULL;
			}
	};
	
}

#endif

