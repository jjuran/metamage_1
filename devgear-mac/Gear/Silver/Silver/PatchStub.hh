/*
	
	Patch.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCHSTUB_HH
#define SILVER_PATCHSTUB_HH

#include "Silver/Traps.hh"


namespace Silver
{
	
	template < class                                               ProcPtr,
	           typename PatchProc_Traits< ProcPtr >::PatchProcPtr  patch >
	struct PatchChainLink
	{
		static ProcPtr proc;
	};
	
	template < class                                               ProcPtr,
	           typename PatchProc_Traits< ProcPtr >::PatchProcPtr  patch >
	//
	ProcPtr PatchChainLink< ProcPtr, patch >::proc;
	
	template < class PatchProcPtr, PatchProcPtr foo > class PatchStub;
	
	template < class R, R (*patch)( R(*)() ) >
	class PatchStub< R (*)( R(*)() ), patch >
	{
		protected:
			typedef pascal R (*ProcPtr)();
			
			typedef PatchChainLink< ProcPtr, patch > Next;
		
		public:
			static pascal R Function()
			{
				return patch( Next::proc );
			}
	};
	
	template < class R, class P0, R (*patch)( P0, R(*)(P0) ) >
	class PatchStub< R (*)( P0, R(*)(P0) ), patch >
	{
		protected:
			typedef pascal R (*ProcPtr)( P0 );
			
			typedef PatchChainLink< ProcPtr, patch > Next;
		
		public:
			static pascal R Function( P0 p0 )
			{
				return patch( p0, Next::proc );
			}
	};
	
	template < class R, class P0, class P1, R (*patch)( P0, P1, R(*)(P0, P1) ) >
	class PatchStub< R (*)( P0, P1, R(*)(P0, P1) ), patch >
	{
		protected:
			typedef pascal R (*ProcPtr)( P0, P1 );
			
			typedef PatchChainLink< ProcPtr, patch > Next;
		
		public:
			static pascal R Function( P0 p0, P1 p1 )
			{
				return patch( p0, p1, Next::proc );
			}
	};
	
	template < class R, class P0, class P1, class P2, R (*patch)( P0, P1, P2, R(*)(P0, P1, P2) ) >
	class PatchStub< R (*)( P0, P1, P2, R(*)(P0, P1, P2) ), patch >
	{
		protected:
			typedef pascal R (*ProcPtr)( P0, P1, P2 );
			
			typedef PatchChainLink< ProcPtr, patch > Next;
		
		public:
			static pascal R Function( P0 p0, P1 p1, P2 p2 )
			{
				return patch( p0, p1, p2, Next::proc );
			}
	};
	
}

#endif

