// AEObjects.h

#ifndef NITROGEN_AEOBJECTS_H
#define NITROGEN_AEOBJECTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEOBJECTS__
#include FRAMEWORK_HEADER(AE,AEObjects.h)
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif

namespace Nitrogen {
	
	struct TokenDisposer : public std::unary_function< AEDesc, void >
	{
		// parameter can't be const
		void operator()( AEDesc token ) const
		{
			::AEDisposeToken( &token );
		}
	};
	
	template <>
	struct LivelinessTraits< AEDesc, TokenDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
	
	struct AEResolveCallbackFlagsTag  {};
	typedef FlagType< AEResolveCallbackFlagsTag, short, kAEIDoMinimum > AEResolveCallbackFlags;
	
	void AEObjectInit();
	
	Owned<AEDesc, TokenDisposer>
	AEResolve( const AEDesc& objectSpecifier, AEResolveCallbackFlags callbackFlags );
	
	inline void AEDisposeToken( Owned<AEDesc, TokenDisposer> )  {}
	
}

#endif
