/*	=====================
 *	ApplicationContext.hh
 *	=====================
 */

#ifndef PEDESTAL_APPLICATIONCONTEXT_HH
#define PEDESTAL_APPLICATIONCONTEXT_HH

// Nitrogen
#include "Nitrogen/Resources.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	struct MacToolboxInit
	{
		MacToolboxInit();
	};
	
	struct MemoryInit
	{
		MemoryInit( std::size_t moreMasters );
	};
	
	class ApplicationContext
	{
		private:
			MacToolboxInit    fMacToolboxInit;
			MemoryInit        fMemoryInit;
			N::ResFileRefNum  fResFileRefNum;
			N::FSDirSpec      fLaunchDir;
			VersRec           fVersion;
		
		public:
			ApplicationContext();
			
			N::ResFileRefNum    ResFileAccessPath() const  { return fResFileRefNum; }
			const N::FSDirSpec& LaunchDirectory  () const  { return fLaunchDir;     }
			const VersRec&      Version          () const  { return fVersion;       }
	};
	
}

#endif

