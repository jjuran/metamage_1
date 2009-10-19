/*
	
	Patch.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCHSTUB_HH
#define SILVER_PATCHSTUB_HH

#include "Silver/Traps.hh"


namespace Silver
{
	
	template < UInt16                                         trapWord,
	           typename TrapTraits< trapWord >::PatchProcPtr  patch >
	//
	struct PatchChainLink
	{
		static typename TrapTraits< trapWord >::ProcPtr proc;
	};
	
	template < UInt16                                         trapWord,
	           typename TrapTraits< trapWord >::PatchProcPtr  patch >
	//
	typename TrapTraits< trapWord >::ProcPtr PatchChainLink< trapWord, patch >::proc;
	
	
	template < int Arity > struct PatchAdapter {};
	
	template <>
	struct PatchAdapter< 0 >
	{
		template < UInt16 trapWord, typename TrapTraits< trapWord >::PatchProcPtr patch >
		struct Glue
		{
			typedef typename TrapTraits< trapWord >::result_type R;
			
			typedef PatchChainLink< trapWord, patch > Next;
			
			static pascal R Function()
			{
				return patch( Next::proc );
			}
		};
	};
	
	template <>
	struct PatchAdapter< 1 >
	{
		template < UInt16 trapWord, typename TrapTraits< trapWord >::PatchProcPtr patch >
		struct Glue
		{
			typedef typename TrapTraits< trapWord >::result_type R;
			typedef typename TrapTraits< trapWord >::param0_type P0;
			
			typedef PatchChainLink< trapWord, patch > Next;
			
			static pascal R Function( P0 p0 )
			{
				return patch( p0, Next::proc );
			}
		};
	};
	
	template <>
	struct PatchAdapter< 2 >
	{
		template < UInt16 trapWord, typename TrapTraits< trapWord >::PatchProcPtr patch >
		struct Glue
		{
			typedef typename TrapTraits< trapWord >::result_type R;
			typedef typename TrapTraits< trapWord >::param0_type P0;
			typedef typename TrapTraits< trapWord >::param1_type P1;
			
			typedef PatchChainLink< trapWord, patch > Next;
			
			static pascal R Function( P0 p0, P1 p1 )
			{
				return patch( p0, p1, Next::proc );
			}
		};
	};
	
	template <>
	struct PatchAdapter< 3 >
	{
		template < UInt16 trapWord, typename TrapTraits< trapWord >::PatchProcPtr patch >
		struct Glue
		{
			typedef typename TrapTraits< trapWord >::result_type R;
			typedef typename TrapTraits< trapWord >::param0_type P0;
			typedef typename TrapTraits< trapWord >::param1_type P1;
			typedef typename TrapTraits< trapWord >::param2_type P2;
			
			typedef PatchChainLink< trapWord, patch > Next;
			
			static pascal R Function( P0 p0, P1 p1, P2 p2 )
			{
				return patch( p0, p1, p2, Next::proc );
			}
		};
	};
	
}

#endif

