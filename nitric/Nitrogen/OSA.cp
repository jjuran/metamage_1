// OSA.cp

#ifndef NITROGEN_OSA_H
#include "Nitrogen/OSA.h"
#endif

#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

namespace Nitrogen {
	
	void RegisterOSAErrors()
	{
		RegisterOSStatus< badComponentInstance       >();
		RegisterOSStatus< errOSASystemError          >();
		RegisterOSStatus< errOSAInvalidID            >();
		RegisterOSStatus< errOSABadStorageType       >();
		RegisterOSStatus< errOSAScriptError          >();
		RegisterOSStatus< errOSABadSelector          >();
		RegisterOSStatus< errOSASourceNotAvailable   >();
		RegisterOSStatus< errOSANoSuchDialect        >();
		RegisterOSStatus< errOSADataFormatObsolete   >();
		RegisterOSStatus< errOSADataFormatTooNew     >();
		RegisterOSStatus< errOSACorruptData          >();
		RegisterOSStatus< errOSARecordingIsAlreadyOn >();
		RegisterOSStatus< errOSAComponentMismatch    >();
		RegisterOSStatus< errOSACantOpenComponent    >();
	}
	
	Nucleus::Owned< OSASpec >
	OSALoad(
		Nucleus::Shared< ComponentInstance > scriptingComponent, 
		const AEDesc& scriptData, 
		OSAModeFlags modeFlags)
	{
		Nucleus::OnlyOnce< RegisterOSAErrors >();
		
		::OSAID resultingScriptID;
		ThrowOSStatus(
			::OSALoad( scriptingComponent, &scriptData, modeFlags, &resultingScriptID )
		);
		
		return Nucleus::Owned< OSASpec >::Seize(
			OSASpec( scriptingComponent, OSAID::Make( resultingScriptID ) )
		);
	}
	
	Nucleus::Owned< AEDesc >
	OSAStore(
		ComponentInstance scriptingComponent, 
		OSAID scriptID, 
		DescType desiredType, 
		OSAModeFlags modeFlags)
	{
		Nucleus::OnlyOnce< RegisterOSAErrors >();
		
		AEDesc resultingScriptData;
		ThrowOSStatus(
			::OSAStore( scriptingComponent, scriptID, desiredType, modeFlags, &resultingScriptData )
		);
		
		return Nucleus::Owned< AEDesc >::Seize( resultingScriptData );
	}
	
	Nucleus::Owned< OSASpec >
	OSAExecute(
		Nucleus::Shared< ComponentInstance > scriptingComponent, 
		OSAID compiledScriptID, 
		OSAID contextID, 
		OSAModeFlags modeFlags)
	{
		Nucleus::OnlyOnce< RegisterOSAErrors >();
		
		::OSAID resultingScriptValueID;
		ThrowOSStatus(
			::OSAExecute( scriptingComponent, compiledScriptID, contextID, modeFlags, &resultingScriptValueID )
		);
		
		return Nucleus::Owned< OSASpec >::Seize(
			OSASpec( scriptingComponent, OSAID::Make( resultingScriptValueID ) )
		);
	}
	
	Nucleus::Owned< AEDesc >
	OSADisplay(
		ComponentInstance scriptingComponent, 
		OSAID scriptValueID, 
		DescType desiredType, 
		OSAModeFlags modeFlags)
	{
		Nucleus::OnlyOnce< RegisterOSAErrors >();
		
		AEDesc resultingText;
		ThrowOSStatus(
			::OSADisplay(
				scriptingComponent, 
				scriptValueID, 
				desiredType, 
				modeFlags, 
				&resultingText
			)
		);
		
		return Nucleus::Owned< AEDesc >::Seize( resultingText );
	}
	
	Nucleus::Owned< OSASpec > OSACompile( Nucleus::Shared< ComponentInstance >  scriptingComponent, 
	                             const AEDesc&                sourceData, 
	                             OSAModeFlags                 modeFlags )
	{
		Nucleus::OnlyOnce< RegisterOSAErrors >();
		
		::OSAID previousAndResultingScriptID = kOSANullScript;
		
		ThrowOSStatus
		(
			::OSACompile
			(
				scriptingComponent, &sourceData, modeFlags, &previousAndResultingScriptID
			)
		);
		
		return Nucleus::Owned< OSASpec >::Seize
		(
			OSASpec( scriptingComponent, OSAID( previousAndResultingScriptID ) )
		);
	}
	
	Nucleus::Owned< OSASpec > OSACompile( Nucleus::Shared< ComponentInstance >  scriptingComponent, 
	                             const AEDesc&                sourceData, 
	                             OSAModeFlags                 modeFlags, 
	                             Nucleus::Owned< OSASpec >             previousScriptID )
	{
		Nucleus::OnlyOnce< RegisterOSAErrors >();
		
		::OSAID previousAndResultingScriptID = previousScriptID.Release().id;
		
		OSAError err = ::OSACompile(
			scriptingComponent, &sourceData, modeFlags, &previousAndResultingScriptID
		);
		
		Nucleus::Owned< OSASpec > resultingScriptID = Nucleus::Owned< OSASpec >::Seize(
			OSASpec( scriptingComponent, OSAID( previousAndResultingScriptID ) )
		);
		if ( err != noErr )
		{
			// resultingScriptID gets lost (but not leaked)
		}
		ThrowOSStatus( err );
		
		return resultingScriptID;
	}
	
}

