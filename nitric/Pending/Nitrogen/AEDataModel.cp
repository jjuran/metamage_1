// AEDataModel.cp

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen {
	
	class AEDescEditor
	{
		private:
			Owned<AEDesc>& desc;
			AEDesc workingCopy;
		public:
			AEDescEditor( Owned<AEDesc>& desc ) : desc( desc ), workingCopy( desc.Release() )  {}
			~AEDescEditor()  { desc = Owned<AEDesc>::Seize( workingCopy ); }
			
			AEDesc& Get()  { return workingCopy; }
			operator AEDesc&()  { return Get(); }
	};
	
}

