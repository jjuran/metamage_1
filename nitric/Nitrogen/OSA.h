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
	typedef IDType< OSAID_Tag, ::OSAID, kOSANullScript > OSAID;
	
	struct OSAModeFlagsTag;
	typedef FlagType< OSAModeFlagsTag, long, kOSAModeNull > OSAModeFlags;
	
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
	
	template <>
	struct Disposer< OSASpec > : public std::unary_function< OSASpec, void >,
	                             private DefaultDestructionOSStatusPolicy
	{
		void operator()( const OSASpec& osaSpec ) const
		{
			OnlyOnce< RegisterOSAErrors >();
			
			HandleDestructionOSStatus( ::OSADispose( osaSpec.component, osaSpec.id ) );
		}
	};
	
	Owned< OSASpec > OSALoad( Shared< ComponentInstance >  scriptingComponent,
	                          const AEDesc&                scriptData,
	                          OSAModeFlags                 modeFlags = OSAModeFlags( kOSAModeNull ) );
	
	Owned< AEDesc > OSAStore( ComponentInstance  scriptingComponent,
	                          OSAID              scriptID,
	                          DescType           desiredType = typeOSAGenericStorage,
	                          OSAModeFlags       modeFlags   = OSAModeFlags( kOSAModeNull ) );
	
	inline Owned< AEDesc > OSAStore( const OSASpec&  script,
	                                 DescType        desiredType = typeOSAGenericStorage,
	                                 OSAModeFlags    modeFlags   = OSAModeFlags( kOSAModeNull ) )
	{
		return OSAStore( script.component, script.id, desiredType, modeFlags );
	}
	
	Owned< OSASpec > OSAExecute( Shared< ComponentInstance > scriptingComponent,
	                             OSAID compiledScriptID,
	                             OSAID contextID = OSAID( kOSANullScript ),
	                             OSAModeFlags modeFlags = OSAModeFlags( kOSAModeNull ) );
	
	inline Owned< OSASpec > OSAExecute( const OSASpec&  script,
	                                    OSAID           contextID = OSAID( kOSANullScript ),
	                                    OSAModeFlags    modeFlags = OSAModeFlags( kOSAModeNull ) )
	{
		return OSAExecute( script.component, script.id, contextID, modeFlags );
	}
	
	Owned< AEDesc > OSADisplay( ComponentInstance  scriptingComponent,
	                            OSAID              scriptValueID,
	                            DescType           desiredType = typeChar,
	                            OSAModeFlags       modeFlags   = OSAModeFlags( kOSAModeNull ) );
	
	inline Owned< AEDesc > OSADisplay( const OSASpec&  scriptValue,
	                                   DescType        desiredType = typeChar,
	                                   OSAModeFlags    modeFlags   = OSAModeFlags( kOSAModeNull ) )
	{
		return OSADisplay( scriptValue.component,
		                   scriptValue.id,
		                   desiredType,
		                   modeFlags );
	}
	
	inline void OSADispose( Owned< OSASpec > )  {}
	
	Owned< OSASpec > OSACompile( Shared< ComponentInstance >  scriptingComponent, 
	                             const AEDesc&                sourceData, 
	                             OSAModeFlags                 modeFlags = OSAModeFlags( kOSAModeNull ) );
	
	Owned< OSASpec > OSACompile( Shared< ComponentInstance >  scriptingComponent, 
	                             const AEDesc&                sourceData, 
	                             OSAModeFlags                 modeFlags, 
	                             Owned< OSASpec >             previousScriptID );
	
	
	inline Owned< ComponentInstance >
	OpenGenericScriptingComponent()
	{
		return Nitrogen::OpenDefaultComponent( kOSAComponentType,
		                                       kOSAGenericScriptingComponentSubtype );
	}
	
}

#endif

