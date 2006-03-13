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

#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
#endif
#ifndef NUCLEUS_SHARED_H
#include "Nucleus/Shared.h"
#endif


namespace Nitrogen
{
	
	void RegisterOSAErrors();
	
	struct OSAID_Tag  {};
	typedef Nucleus::IDType< OSAID_Tag, ::OSAID, kOSANullScript > OSAID;
	
	struct OSAModeFlagsTag;
	typedef Nucleus::FlagType< OSAModeFlagsTag, long, kOSAModeNull > OSAModeFlags;
	
	struct OSASpec 
	{
		OSASpec() : component(), id()  {}
		
		OSASpec( Nucleus::Shared< ComponentInstance > component, OSAID id ) 
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
		
		Nucleus::Shared< ComponentInstance > component;
		OSAID id;
	};
	
}

namespace Nucleus
{
	
	template <>
	struct Disposer< Nitrogen::OSASpec > : public std::unary_function< Nitrogen::OSASpec, void >,
	                                       private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( const Nitrogen::OSASpec& osaSpec ) const
		{
			Nucleus::OnlyOnce< Nitrogen::RegisterOSAErrors >();
			HandleDestructionOSStatus( ::OSADispose( osaSpec.component, osaSpec.id ) );
		}
	};
	
}

namespace Nitrogen
{
	
	Nucleus::Owned< OSASpec > OSALoad( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                   const AEDesc&                         scriptData,
	                                   OSAModeFlags                          modeFlags = OSAModeFlags ());
	
	Nucleus::Owned< AEDesc > OSAStore( ComponentInstance  scriptingComponent,
	                                   OSAID              scriptID,
	                                   DescType           desiredType = typeOSAGenericStorage,
	                                   OSAModeFlags       modeFlags   = OSAModeFlags( kOSAModeNull ) );
	
	inline Nucleus::Owned< AEDesc > OSAStore( const OSASpec&  script,
	                                          DescType        desiredType = typeOSAGenericStorage,
	                                          OSAModeFlags    modeFlags   = OSAModeFlags ())
	{
		return OSAStore( script.component, script.id, desiredType, modeFlags );
	}
	
	Nucleus::Owned< OSASpec > OSAExecute( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                      OSAID                                 compiledScriptID,
	                                      OSAID                                 contextID = OSAID(),
	                                      OSAModeFlags                          modeFlags = OSAModeFlags( kOSAModeNull ) );
	
	inline Nucleus::Owned< OSASpec > OSAExecute( const OSASpec&  script,
	                                             OSAID           contextID = OSAID(),
	                                             OSAModeFlags    modeFlags = OSAModeFlags ())
	{
		return OSAExecute( script.component, script.id, contextID, modeFlags );
	}
	
	Nucleus::Owned< AEDesc > OSADisplay( ComponentInstance  scriptingComponent,
	                                     OSAID              scriptValueID,
	                                     DescType           desiredType = typeChar,
	                                     OSAModeFlags       modeFlags   = OSAModeFlags ());
	
	inline Nucleus::Owned< AEDesc > OSADisplay( const OSASpec&  scriptValue,
	                                            DescType        desiredType = typeChar,
	                                            OSAModeFlags    modeFlags   = OSAModeFlags ())
	{
		return OSADisplay( scriptValue.component,
		                   scriptValue.id,
		                   desiredType,
		                   modeFlags );
	}
	
	inline void OSADispose( Nucleus::Owned< OSASpec > )  {}
	
	Nucleus::Owned< OSASpec > OSACompile( Nucleus::Shared< ComponentInstance >  scriptingComponent, 
	                                      const AEDesc&                         sourceData, 
	                                      OSAModeFlags                          modeFlags = OSAModeFlags ());
	
	Nucleus::Owned< OSASpec > OSACompile( Nucleus::Shared< ComponentInstance >  scriptingComponent, 
	                                      const AEDesc&                         sourceData, 
	                                      OSAModeFlags                          modeFlags, 
	                                      Nucleus::Owned< OSASpec >             previousScriptID );
	
	
	inline Nucleus::Owned< ComponentInstance >
	OpenGenericScriptingComponent()
	{
		return Nitrogen::OpenDefaultComponent( kOSAComponentType,
		                                       kOSAGenericScriptingComponentSubtype );
	}
	
}

#endif

