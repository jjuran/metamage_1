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
	
	static const ComponentType    kOSAComponentType                    = ComponentType   ::Make( ::kOSAComponentType );
	static const ComponentSubType kOSAGenericScriptingComponentSubtype = ComponentSubType::Make( ::kOSAGenericScriptingComponentSubtype );
	
	using ::kOSAFileType;
	using ::kOSASuite;
	
	static const OSAID kOSANullScript = OSAID::Make( ::kOSANullScript );
	
	static const OSAModeFlags kOSAModeNull = OSAModeFlags::Make( ::kOSAModeNull );
	
	static const ResType kOSAScriptResourceType = ResType::Make( ::kOSAScriptResourceType );
	
	static const DescType typeOSAGenericStorage = DescType::Make( ::typeOSAGenericStorage );
	
	struct ManagedOSAID 
	{
		ManagedOSAID() : component(), id()  {}
		ManagedOSAID( Shared< ComponentInstance > component, OSAID id ) 
		  : component( component ), id( id )  {}
		
		operator OSAID() const  { return id; }
		
		Shared< ComponentInstance > component;
		OSAID id;
	};
	
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
		OSAModeFlags modeFlags = kOSAModeNull
	);
	
	Owned< AEDesc >
	OSAStore(
		ComponentInstance scriptingComponent, 
		OSAID scriptID, 
		DescType desiredType = typeOSAGenericStorage, 
		OSAModeFlags modeFlags = kOSAModeNull
	);
	
	inline
	Owned< AEDesc >
	OSAStore(
		const ManagedOSAID& script, 
		DescType desiredType = typeOSAGenericStorage, 
		OSAModeFlags modeFlags = kOSAModeNull)
	{
		return OSAStore( script.component, script.id, desiredType, modeFlags );
	}
	
	Owned< ManagedOSAID >
	OSAExecute(
		Shared< ComponentInstance > scriptingComponent, 
		OSAID compiledScriptID, 
		OSAID contextID = kOSANullScript, 
		OSAModeFlags modeFlags = kOSAModeNull
	);
	
	inline
	Owned< ManagedOSAID >
	OSAExecute(
		const ManagedOSAID& script, 
		OSAID contextID = kOSANullScript, 
		OSAModeFlags modeFlags = kOSAModeNull)
	{
		return OSAExecute( script.component, script.id, contextID, modeFlags );
	}
	
	Owned< AEDesc >
	OSADisplay(
		ComponentInstance scriptingComponent, 
		OSAID scriptValueID, 
		DescType desiredType = typeChar, 
		OSAModeFlags modeFlags = kOSAModeNull
	);
	
	inline
	Owned< AEDesc >
	OSADisplay(
		const ManagedOSAID& scriptValue, 
		DescType desiredType = typeChar, 
		OSAModeFlags modeFlags = kOSAModeNull)
	{
		return OSADisplay( scriptValue.component, scriptValue.id, desiredType, modeFlags );
	}
	
	inline void OSADispose( Owned< ManagedOSAID > )  {}
	
	Owned< ManagedOSAID >
	OSACompile(
		Shared< ComponentInstance > scriptingComponent, 
		const AEDesc& sourceData, 
		OSAModeFlags modeFlags = kOSAModeNull, 
		Owned< ManagedOSAID > previousScriptID = Owned< ManagedOSAID >()
	);
	
	
	inline
	Owned<ComponentInstance>
	OpenGenericScriptingComponent()
	{
		return OpenDefaultComponent(
			kOSAComponentType, 
			kOSAGenericScriptingComponentSubtype
		);
	}
	
}

#endif

