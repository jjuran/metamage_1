// OSA.h

#ifndef NITROGEN_OSA_H
#define NITROGEN_OSA_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __OSA__
#include FRAMEWORK_HEADER(OpenScripting,OSA.h)
#endif

#ifndef NITROGEN_APPLEEVENTS_H
#include "Nitrogen/AppleEvents.h"
#endif
#ifndef NITROGEN_COMPONENTS_H
#include "Nitrogen/Components.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif

#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_SHARED_H
#include "Nitrogen/Shared.h"
#endif

namespace Nitrogen {
	
	void RegisterOSAErrors();
	
	struct OSAID_Tag  {};
	typedef IDType< OSAID_Tag, ::OSAID, ::kOSANullScript > OSAID;
	
	struct OSAModeFlagsTag;
	typedef FlagType< OSAModeFlagsTag, long, ::kOSAModeNull > OSAModeFlags;
	
	inline ComponentType    OSAComponentType                   ()  { return ComponentType   ::Make( kOSAComponentType                    ); }
	inline ComponentSubType OSAGenericScriptingComponentSubtype()  { return ComponentSubType::Make( kOSAGenericScriptingComponentSubtype ); }
	
	inline OSType OSAFileType()  { return OSType::Make( kOSAFileType ); }
	
	inline AEEventClass OSASuite()  { return AEEventClass::Make( kOSASuite ); }
	
	inline OSAID OSANullScript()  { return OSAID::Make( kOSANullScript ); }
	
	inline OSAModeFlags OSAModeNull()  { return OSAModeFlags::Make( kOSAModeNull ); }
	
	inline ResType OSAScriptResourceType()  { return ResType::Make( kOSAScriptResourceType ); }
	
	inline DescType TypeOSAGenericStorage()  { return DescType::Make( typeOSAGenericStorage ); }
	
	struct OSASpec 
	{
		OSASpec() : component(), id()  {}
		OSASpec( Shared< ComponentInstance > component, OSAID id ) 
		:
			component( component ), 
			id       ( id        )
		{}
		
		operator OSAID() const  { return id; }
		
		friend bool operator==( const OSASpec& a, const OSASpec& b )
		{
			return a.component.Get() == b.component.Get()
				&& a.id              == b.id;
		}
		friend bool operator!=( const OSASpec& a, const OSASpec& b )  { return !( a == b ); }
		
		Shared< ComponentInstance > component;
		OSAID id;
	};
	typedef OSASpec ManagedOSAID;
	
	template <>
	struct Disposer< OSASpec > : public std::unary_function< OSASpec, void >
	{
		void operator()( const OSASpec& osaSpec ) const
		{
			::OSADispose( osaSpec.component, osaSpec.id );
		}
	};
	
	Owned< OSASpec >
	OSALoad(
		Shared< ComponentInstance > scriptingComponent, 
		const AEDesc& scriptData, 
		OSAModeFlags modeFlags = OSAModeNull()
	);
	
	Owned< AEDesc >
	OSAStore(
		ComponentInstance scriptingComponent, 
		OSAID scriptID, 
		DescType desiredType = TypeOSAGenericStorage(), 
		OSAModeFlags modeFlags = OSAModeNull()
	);
	
	inline
	Owned< AEDesc >
	OSAStore(
		const OSASpec& script, 
		DescType desiredType = TypeOSAGenericStorage(), 
		OSAModeFlags modeFlags = OSAModeNull() )
	{
		return OSAStore( script.component, script.id, desiredType, modeFlags );
	}
	
	Owned< OSASpec >
	OSAExecute(
		Shared< ComponentInstance > scriptingComponent, 
		OSAID compiledScriptID, 
		OSAID contextID = OSANullScript(), 
		OSAModeFlags modeFlags = OSAModeNull()
	);
	
	inline
	Owned< OSASpec >
	OSAExecute(
		const OSASpec& script, 
		OSAID contextID = OSANullScript(), 
		OSAModeFlags modeFlags = OSAModeNull() )
	{
		return OSAExecute( script.component, script.id, contextID, modeFlags );
	}
	
	Owned< AEDesc >
	OSADisplay(
		ComponentInstance scriptingComponent, 
		OSAID scriptValueID, 
		DescType desiredType = TypeChar(), 
		OSAModeFlags modeFlags = OSAModeNull()
	);
	
	inline
	Owned< AEDesc >
	OSADisplay(
		const OSASpec& scriptValue, 
		DescType desiredType = TypeChar(), 
		OSAModeFlags modeFlags = OSAModeNull() )
	{
		return OSADisplay( scriptValue.component, scriptValue.id, desiredType, modeFlags );
	}
	
	inline void OSADispose( Owned< OSASpec > )  {}
	
	Owned< OSASpec >
	OSACompile(
		Shared< ComponentInstance > scriptingComponent, 
		const AEDesc& sourceData, 
		OSAModeFlags modeFlags = OSAModeNull(), 
		Owned< OSASpec > previousScriptID = Owned< OSASpec >()
	);
	
	
	inline
	Owned<ComponentInstance>
	OpenGenericScriptingComponent()
	{
		return OpenDefaultComponent(
			OSAComponentType(), 
			OSAGenericScriptingComponentSubtype()
		);
	}
	
}

#endif

