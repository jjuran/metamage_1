// OSA.h

#ifndef NITROGEN_OSA_H
#define NITROGEN_OSA_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __OSA__
#include FRAMEWORK_HEADER(OpenScripting,OSA.h)
#endif

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_COMPONENTS_H
#include "Nitrogen/Components.h"
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
	
	struct OSAIDTag  {};
	typedef IDType< OSAIDTag, ::OSAID, ::kOSANullScript > OSAID;
	
	struct OSAModeFlagsTag;
	typedef FlagType< OSAModeFlagsTag, long, ::kOSAModeNull > OSAModeFlags;
	
	inline ComponentType    OSAComponentType                   ()  { return ComponentType   ::Make( kOSAComponentType                    ); }
	inline ComponentSubType OSAGenericScriptingComponentSubtype()  { return ComponentSubType::Make( kOSAGenericScriptingComponentSubtype ); }
	
	using ::kOSAFileType;
	using ::kOSASuite;
	
	inline OSAID OSANullScript()  { return OSAID::Make( kOSANullScript ); }
	
	inline OSAModeFlags OSAModeNull()  { return OSAModeFlags::Make( kOSAModeNull ); }
	
	inline ResType OSAScriptResourceType()  { return ResType::Make( kOSAScriptResourceType ); }
	
	inline DescType TypeOSAGenericStorage()  { return DescType::Make( typeOSAGenericStorage ); }
	
	struct ManagedOSAID 
	{
		ManagedOSAID() : component(), id()  {}
		ManagedOSAID( Shared< ComponentInstance > component, OSAID id ) 
		:
			component( component ), 
			id       ( id        )
		{}
		
		operator OSAID() const  { return id; }
		
		friend bool operator==( const ManagedOSAID& a, const ManagedOSAID& b )
		{
			return a.component.Get() == b.component.Get()
				&& a.id              == b.id;
		}
		friend bool operator!=( const ManagedOSAID& a, const ManagedOSAID& b )  { return !( a == b ); }
		
		Shared< ComponentInstance > component;
		OSAID id;
	};
	typedef ManagedOSAID OSASpec;
	
	template <>
	struct Disposer< ManagedOSAID > : public std::unary_function< ManagedOSAID, void >
	{
		void operator()( const ManagedOSAID& id ) const
		{
			::OSADispose( id.component, id.id );
		}
	};
	
	Owned< ManagedOSAID >
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
		const ManagedOSAID& script, 
		DescType desiredType = TypeOSAGenericStorage(), 
		OSAModeFlags modeFlags = OSAModeNull() )
	{
		return OSAStore( script.component, script.id, desiredType, modeFlags );
	}
	
	Owned< ManagedOSAID >
	OSAExecute(
		Shared< ComponentInstance > scriptingComponent, 
		OSAID compiledScriptID, 
		OSAID contextID = OSANullScript(), 
		OSAModeFlags modeFlags = OSAModeNull()
	);
	
	inline
	Owned< ManagedOSAID >
	OSAExecute(
		const ManagedOSAID& script, 
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
		const ManagedOSAID& scriptValue, 
		DescType desiredType = TypeChar(), 
		OSAModeFlags modeFlags = OSAModeNull() )
	{
		return OSADisplay( scriptValue.component, scriptValue.id, desiredType, modeFlags );
	}
	
	inline void OSADispose( Owned< ManagedOSAID > )  {}
	
	Owned< ManagedOSAID >
	OSACompile(
		Shared< ComponentInstance > scriptingComponent, 
		const AEDesc& sourceData, 
		OSAModeFlags modeFlags = OSAModeNull(), 
		Owned< ManagedOSAID > previousScriptID = Owned< ManagedOSAID >()
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

