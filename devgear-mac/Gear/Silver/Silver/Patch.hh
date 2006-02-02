/*
	
	Patch.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PATCH_HH
#define SILVER_PATCH_HH

#include "Silver/PatchUtils.hh"


namespace Silver
{
	
	template < class Trap >
	struct Patch
	{
		typedef Trap::ProcPtr ProcPtr;
		static short TrapWord()  { return Trap::TrapWord(); }
		
		Patch( ProcPtr proc ) : proc( proc )  {}
		
		ProcPtr Apply()  { return ApplyTrapPatch( TrapWord(), proc ); }
		
		ProcPtr proc;
	};
	
	template < class Patch >
	class PatchApplied
	{
		typedef Patch::ProcPtr ProcPtr;
		
		private:
			ProcPtr original;
		
		public:
			static short TrapWord()  { return Patch::TrapWord(); }
			
			PatchApplied()  {}
			PatchApplied( Patch patch ) : original( patch.Apply() )  {}
			
			void Remove()  { RemoveTrapPatch( TrapWord(), original ); }
			
			ProcPtr Original() const  { return original; }
	};
	
	template < class Patch >
	class TemporaryPatchApplied : public PatchApplied< Patch >
	{
		public:
			TemporaryPatchApplied( Patch patch ) : PatchApplied( patch )  {}
			~TemporaryPatchApplied()  { Remove(); }
	};
	
}

#endif

