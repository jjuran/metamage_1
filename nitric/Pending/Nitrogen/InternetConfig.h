/*	================
 *	InternetConfig.h
 *	================
 */

#ifndef NITROGEN_INTERNETCONFIG_H
#define NITROGEN_INTERNETCONFIG_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __INTERNETCONFIG__
#include FRAMEWORK_HEADER(HIServices,InternetConfig.h)
#endif

#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif


namespace Nitrogen
{
	
	void RegisterInternetConfigErrors();
	
	template <>  struct OwnedDefaults< ICFontRecordHandle   > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICCharTableHandle    > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICAppSpecHandle      > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICAppSpecListHandle  > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICFileSpecHandle     > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICMapEntryHandle     > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICServiceEntryHandle > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICServicesHandle     > : OwnedDefaults< Handle >  {};
	
	// 1179
	Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                  ConstStr255Param  key,
	                                  ICAttr&           attr );
	
	Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                  ConstStr255Param  key );
	
}

#endif

