/*	==========
 *	OSUtils.cp
 *	==========
 */

#ifndef NITROGEN_OSUTILS_H
#include "Nitrogen/OSUtils.h"
#endif

// Nitrogen
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	void DTInstall( DeferredTask& dtTaskPtr )
	{
		OnlyOnce< RegisterDeferredTaskManagerErrors >();
		
		ThrowOSStatus( ::DTInstall( &dtTaskPtr ) );
	}
	
	void RegisterDeferredTaskManagerErrors()
	{
		RegisterOSStatus< vTypErr >();
	}
	
}

