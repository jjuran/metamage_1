/*
	
	Patch.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCHSTUB_HH
#define SILVER_PATCHSTUB_HH

#include "Silver/Traps.hh"


namespace Silver
{
	
	template < UInt16                                     trap,
	           typename TrapTraits< trap >::PatchProcPtr  patch >
	//
	struct PatchChainLink
	{
		static typename TrapTraits< trap >::ProcPtr proc;
	};
	
	template < UInt16                                     trap,
	           typename TrapTraits< trap >::PatchProcPtr  patch >
	//
	typename TrapTraits< trap >::ProcPtr PatchChainLink< trap, patch >::proc;
	
	
	template < int Arity > struct PatchAdapter {};
	
	template <>
	struct PatchAdapter< 0 >
	{
		template < UInt16 trap, typename TrapTraits< trap >::PatchProcPtr patch >
		struct Glue
		{
			typedef typename TrapTraits< trap >::result_type R;
			
			typedef PatchChainLink< trap, patch > Next;
			
			static pascal R Function()
			{
				return patch( Next::proc );
			}
		};
	};
	
	template <>
	struct PatchAdapter< 1 >
	{
		template < UInt16 trap, typename TrapTraits< trap >::PatchProcPtr patch >
		struct Glue
		{
			typedef typename TrapTraits< trap >::result_type R;
			typedef typename TrapTraits< trap >::param0_type P0;
			
			typedef PatchChainLink< trap, patch > Next;
			
			static pascal R Function( P0 p0 )
			{
				return patch( p0, Next::proc );
			}
		};
	};
	
	template <>
	struct PatchAdapter< 2 >
	{
		template < UInt16 trap, typename TrapTraits< trap >::PatchProcPtr patch >
		struct Glue
		{
			typedef typename TrapTraits< trap >::result_type R;
			typedef typename TrapTraits< trap >::param0_type P0;
			typedef typename TrapTraits< trap >::param1_type P1;
			
			typedef PatchChainLink< trap, patch > Next;
			
			static pascal R Function( P0 p0, P1 p1 )
			{
				return patch( p0, p1, Next::proc );
			}
		};
	};
	
	template <>
	struct PatchAdapter< 3 >
	{
		template < UInt16 trap, typename TrapTraits< trap >::PatchProcPtr patch >
		struct Glue
		{
			typedef typename TrapTraits< trap >::result_type R;
			typedef typename TrapTraits< trap >::param0_type P0;
			typedef typename TrapTraits< trap >::param1_type P1;
			typedef typename TrapTraits< trap >::param2_type P2;
			
			typedef PatchChainLink< trap, patch > Next;
			
			static pascal R Function( P0 p0, P1 p1, P2 p2 )
			{
				return patch( p0, p1, p2, Next::proc );
			}
		};
	};
	
}

#endif

