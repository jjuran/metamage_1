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
	class InternetConfigErrorsRegistrationDependency
	{
		public:
			InternetConfigErrorsRegistrationDependency();
	};
	
	using ::ICFontRecordHandle;
	using ::ICCharTableHandle;
	using ::ICAppSpecHandle;
	using ::ICAppSpecListHandle;
	using ::ICFileSpecHandle;
	using ::ICMapEntryHandle;
	using ::ICServiceEntryHandle;
	using ::ICServicesHandle;
  }

namespace Nucleus
{
	template <>  struct OwnedDefaults< Nitrogen::ICFontRecordHandle   > : OwnedDefaults< Nitrogen::Handle >  {};
	template <>  struct OwnedDefaults< Nitrogen::ICCharTableHandle    > : OwnedDefaults< Nitrogen::Handle >  {};
	template <>  struct OwnedDefaults< Nitrogen::ICAppSpecHandle      > : OwnedDefaults< Nitrogen::Handle >  {};
	template <>  struct OwnedDefaults< Nitrogen::ICAppSpecListHandle  > : OwnedDefaults< Nitrogen::Handle >  {};
	template <>  struct OwnedDefaults< Nitrogen::ICFileSpecHandle     > : OwnedDefaults< Nitrogen::Handle >  {};
	template <>  struct OwnedDefaults< Nitrogen::ICMapEntryHandle     > : OwnedDefaults< Nitrogen::Handle >  {};
	template <>  struct OwnedDefaults< Nitrogen::ICServiceEntryHandle > : OwnedDefaults< Nitrogen::Handle >  {};
	template <>  struct OwnedDefaults< Nitrogen::ICServicesHandle     > : OwnedDefaults< Nitrogen::Handle >  {};
}

namespace Nitrogen
{
	// 1179
	Nucleus::Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                           ConstStr255Param  key,
	                                           ICAttr&           attr );
	
	Nucleus::Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                           ConstStr255Param  key );
	
}

#endif

