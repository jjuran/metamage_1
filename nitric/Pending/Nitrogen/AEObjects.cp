// AEObjects.cp

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif

namespace Nitrogen {
	
	class AETokenEditor
	{
		private:
			Owned<AEDesc, TokenDisposer>& desc;
			AEDesc workingCopy;
		public:
			AETokenEditor( Owned<AEDesc, TokenDisposer>& desc ) : desc(desc), workingCopy(desc.Release())  {}
			~AETokenEditor()  { desc = Owned<AEDesc, TokenDisposer>::Seize(workingCopy); }
			
			AEDesc& Get()  { return workingCopy; }
			operator AEDesc&()  { return Get(); }
	};
	
	void AEObjectInit()
	{
		ThrowOSStatus( ::AEObjectInit() );
	}
	
	Owned<AEDesc, TokenDisposer>
	AEResolve( const AEDesc& objectSpecifier, AEResolveCallbackFlags callbackFlags )
	{
		AEDesc token;
		ThrowOSStatus( ::AEResolve( &objectSpecifier, callbackFlags, &token ) );
		return Owned<AEDesc, TokenDisposer>::Seize( token );
	}
	
}

