// OSA.cp

#ifndef NITROGEN_OSA_H
#include "Nitrogen/OSA.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
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
	
	Owned< OSASpec >
	OSALoad(
		Shared< ComponentInstance > scriptingComponent, 
		const AEDesc& scriptData, 
		OSAModeFlags modeFlags)
	{
		OnlyOnce< RegisterOSAErrors >();
		
		::OSAID resultingScriptID;
		ThrowOSStatus(
			::OSALoad( scriptingComponent, &scriptData, modeFlags, &resultingScriptID )
		);
		
		return Owned< OSASpec >::Seize(
			OSASpec( scriptingComponent, OSAID::Make( resultingScriptID ) )
		);
	}
	
	Owned< AEDesc >
	OSAStore(
		ComponentInstance scriptingComponent, 
		OSAID scriptID, 
		DescType desiredType, 
		OSAModeFlags modeFlags)
	{
		OnlyOnce< RegisterOSAErrors >();
		
		AEDesc resultingScriptData;
		ThrowOSStatus(
			::OSAStore( scriptingComponent, scriptID, desiredType, modeFlags, &resultingScriptData )
		);
		
		return Owned< AEDesc >::Seize( resultingScriptData );
	}
	
	Owned< OSASpec >
	OSAExecute(
		Shared< ComponentInstance > scriptingComponent, 
		OSAID compiledScriptID, 
		OSAID contextID, 
		OSAModeFlags modeFlags)
	{
		OnlyOnce< RegisterOSAErrors >();
		
		::OSAID resultingScriptValueID;
		ThrowOSStatus(
			::OSAExecute( scriptingComponent, compiledScriptID, contextID, modeFlags, &resultingScriptValueID )
		);
		
		return Owned< OSASpec >::Seize(
			OSASpec( scriptingComponent, OSAID::Make( resultingScriptValueID ) )
		);
	}
	
	Owned< AEDesc >
	OSADisplay(
		ComponentInstance scriptingComponent, 
		OSAID scriptValueID, 
		DescType desiredType, 
		OSAModeFlags modeFlags)
	{
		OnlyOnce< RegisterOSAErrors >();
		
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
		
		return Owned< AEDesc >::Seize( resultingText );
	}
	
	Owned< OSASpec > OSACompile( Shared< ComponentInstance >  scriptingComponent, 
	                             const AEDesc&                sourceData, 
	                             OSAModeFlags                 modeFlags )
	{
		OnlyOnce< RegisterOSAErrors >();
		
		::OSAID previousAndResultingScriptID = kOSANullScript;
		
		ThrowOSStatus
		(
			::OSACompile
			(
				scriptingComponent, &sourceData, modeFlags, &previousAndResultingScriptID
			)
		);
		
		return Owned< OSASpec >::Seize
		(
			OSASpec( scriptingComponent, OSAID( previousAndResultingScriptID ) )
		);
	}
	
	Owned< OSASpec > OSACompile( Shared< ComponentInstance >  scriptingComponent, 
	                             const AEDesc&                sourceData, 
	                             OSAModeFlags                 modeFlags, 
	                             Owned< OSASpec >             previousScriptID )
	{
		OnlyOnce< RegisterOSAErrors >();
		
		::OSAID previousAndResultingScriptID = previousScriptID.Release().id;
		
		OSAError err = ::OSACompile(
			scriptingComponent, &sourceData, modeFlags, &previousAndResultingScriptID
		);
		
		Owned< OSASpec > resultingScriptID = Owned< OSASpec >::Seize(
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

