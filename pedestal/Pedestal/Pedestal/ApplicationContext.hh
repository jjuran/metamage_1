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
			MacToolboxInit           itsMacToolboxInit;
			MemoryInit               itsMemoryInit;
			Nitrogen::ResFileRefNum  itsResFileRefNum;
			VersRec                  itsVersion;
		
		public:
			ApplicationContext();
			
			Nitrogen::ResFileRefNum ResFileAccessPath() const  { return itsResFileRefNum; }
			const VersRec&          Version          () const  { return itsVersion;       }
	};
	
}

#endif

