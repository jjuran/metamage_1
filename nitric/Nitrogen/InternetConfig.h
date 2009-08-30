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
	
	using ::ICInstance;
	
	template <>
	struct Disposer< ICInstance > : std::unary_function< ICInstance, void >,
	                                DefaultDestructionOSStatusPolicy
	{
		void operator()( ICInstance inst ) const
		{
			HandleDestructionOSStatus( ::ICStop( inst ) );
		}
	};
	
	class ICVersionType_Tag {};
	typedef SelectorType< ICVersionType_Tag, long, kICComponentVersion > ICVersionType;
	
	template <>  struct OwnedDefaults< ICFontRecordHandle   > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICCharTableHandle    > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICAppSpecHandle      > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICAppSpecListHandle  > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICFileSpecHandle     > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICMapEntryHandle     > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICServiceEntryHandle > : OwnedDefaults< Handle >  {};
	template <>  struct OwnedDefaults< ICServicesHandle     > : OwnedDefaults< Handle >  {};
	
	Owned< ICInstance > ICStart( OSType signature );
	
	void ICStop( Owned< ICInstance > inst );
	
	void ICGetVersion( ICInstance inst, long versionType, void* outVersion );
	
	template < long versionType >
	struct ICVersion_Traits
	{
		// default:  kICComponentVersion
		typedef UInt32 Type;
	};
	
	template <>
	struct ICVersion_Traits< kICNumVersion >
	{
		typedef NumVersion Type;
	};
	
	template < long versionType >
	typename ICVersion_Traits< versionType >::Type ICGetVersion( ICInstance inst )
	{
		typename ICVersion_Traits< versionType >::Type result;
		
		ICGetVersion( inst, versionType, &result );
		
		return result;
	}
	
	// 1179
	Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                  ConstStr255Param  key,
	                                  ICAttr&           attr );
	
	Owned< Handle > ICFindPrefHandle( ICInstance        instance,
	                                  ConstStr255Param  key );
	
}

#endif

