// Nitrogen/OSA.cp
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_OSA_H
#include "Nitrogen/OSA.h"
#endif


namespace Nitrogen
{
	
	OSAErrorsRegistrationDependency::OSAErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterOSAErrors();
	
	
	class OSAErrorsRegistration
	{
		public:
			OSAErrorsRegistration()  { RegisterOSAErrors(); }
	};
	
	static OSAErrorsRegistration theRegistration;
	
	
	Nucleus::Owned< OSASpec > OSALoad( Nucleus::Shared< ComponentInstance >  scriptingComponent, 
	                                   const AEDesc&                         scriptData, 
	                                   OSAModeFlags                          modeFlags )
	{
		::OSAID resultingScriptID;
		
		ThrowOSStatus( ::OSALoad( scriptingComponent,
		                          &scriptData,
		                          modeFlags,
		                          &resultingScriptID ) );
		
		return Nucleus::Owned< OSASpec >::Seize( OSASpec( scriptingComponent,
		                                                  OSAID( resultingScriptID ) ) );
	}
	
	Nucleus::Owned< AEDesc_Data > OSAStore( ComponentInstance  scriptingComponent,
	                                        OSAID              scriptID,
	                                        DescType           desiredType,
	                                        OSAModeFlags       modeFlags )
	{
		AEDesc_Data resultingScriptData;
		
		ThrowOSStatus( ::OSAStore( scriptingComponent,
		                           scriptID,
		                           desiredType,
		                           modeFlags,
		                           &resultingScriptData ) );
		
		return Nucleus::Owned< AEDesc_Data >::Seize( resultingScriptData );
	}
	
	Nucleus::Owned< OSASpec > OSAExecute( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                      OSAID                                 compiledScriptID,
	                                      OSAID                                 contextID,
	                                      OSAModeFlags                          modeFlags )
	{
		::OSAID resultingScriptValueID;
		
		ThrowOSStatus( ::OSAExecute( scriptingComponent,
		                             compiledScriptID,
		                             contextID,
		                             modeFlags,
		                             &resultingScriptValueID ) );
		
		return Nucleus::Owned< OSASpec >::Seize( OSASpec( scriptingComponent,
		                                                  OSAID( resultingScriptValueID ) ) );
	}
	
	Nucleus::Owned< AEDesc_Data > OSADisplay( ComponentInstance  scriptingComponent,
	                                          OSAID              scriptValueID,
	                                          DescType           desiredType,
	                                          OSAModeFlags       modeFlags )
	{
		AEDesc_Data resultingText;
		
		ThrowOSStatus( ::OSADisplay( scriptingComponent,
		                             scriptValueID,
		                             desiredType,
		                             modeFlags,
		                             &resultingText ) );
		
		return Nucleus::Owned< AEDesc_Data >::Seize( resultingText );
	}
	
	Nucleus::Owned< AEDesc_Data > OSAScriptError( ComponentInstance  scriptingComponent,
	                                              AEKeyword          selector,
	                                              DescType           desiredType )
	{
		AEDesc_Data resultingErrorDescription;
		
		ThrowOSStatus( ::OSAScriptError( scriptingComponent,
		                                 selector,
		                                 desiredType,
		                                 &resultingErrorDescription ) );
		
		return Nucleus::Owned< AEDesc_Data >::Seize( resultingErrorDescription );
	}
	
	Nucleus::Owned< AEDesc_Data > OSAScriptingComponentName( ComponentInstance scriptingComponent )
	{
		AEDesc_Data resultingScriptingComponentName;
		
		ThrowOSStatus( ::OSAScriptingComponentName( scriptingComponent,
		                                            &resultingScriptingComponentName ) );
		
		return Nucleus::Owned< AEDesc_Data >::Seize( resultingScriptingComponentName );
	}
	
	Nucleus::Owned< OSASpec > OSACompile( Nucleus::Shared< ComponentInstance >  scriptingComponent, 
	                             const AEDesc&                sourceData, 
	                             OSAModeFlags                 modeFlags )
	{
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
	                                      const AEDesc&                         sourceData, 
	                                      OSAModeFlags                          modeFlags, 
	                                      Nucleus::Owned< OSASpec >             previousScriptID )
	{
		::OSAID previousAndResultingScriptID = previousScriptID.Release().id;
		
		OSAError err = ::OSACompile( scriptingComponent,
		                             &sourceData,
		                             modeFlags,
		                             &previousAndResultingScriptID );
		
		Nucleus::Owned< OSASpec > resultingScriptID
			= Nucleus::Owned< OSASpec >::Seize( OSASpec( scriptingComponent,
			                                             OSAID( previousAndResultingScriptID ) ) );
		
		if ( err != noErr )
		{
			// resultingScriptID gets lost (but not leaked)
		}
		
		ThrowOSStatus( err );
		
		return resultingScriptID;
	}
	
	void OSACopyID( OSAID                       fromID,
	                Nucleus::Owned< OSASpec >&  to )
	{
		::OSAID toID = to.Get().id;
		
		ThrowOSStatus( ::OSACopyID( to.Get().component,
		                            fromID,
		                            &toID ) );
		
		if ( toID != to.Get().id )
		{
			to.Release();
			
			to = Nucleus::Owned< OSASpec >::Seize( OSASpec( to.Get().component,
			                                                OSAID( toID ) ) );
		}
	}
	
	void OSACopyID( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                OSAID                                 fromID,
	                Nucleus::Owned< OSASpec >&            to )
	{
		if ( scriptingComponent.Get() != to.Get().component )
		{
			throw ErrOSAComponentMismatch();
		}
		
		OSACopyID( fromID, to );
	}
	
	Nucleus::Owned< OSASpec > OSACopyID( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                     OSAID                                 fromID )
	{
		::OSAID toID = kOSANullScript;
		
		ThrowOSStatus( ::OSACopyID( scriptingComponent,
		                            fromID,
		                            &toID ) );
		
		return Nucleus::Owned< OSASpec >::Seize( OSASpec( scriptingComponent,
		                                                  OSAID( toID ) ) );
	}
	
	Nucleus::Owned< OSASpec > OSAExecuteEvent( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                           const AppleEvent&                     appleEvent,
	                                           OSAID                                 contextID,
	                                           OSAModeFlags                          modeFlags )
	{
		::OSAID resultingScriptValueID;
		
		ThrowOSStatus( ::OSAExecuteEvent( scriptingComponent,
		                                  &appleEvent,
		                                  contextID,
		                                  modeFlags,
		                                  &resultingScriptValueID ) );
		
		return Nucleus::Owned< OSASpec >::Seize( OSASpec( scriptingComponent,
		                                                  OSAID( resultingScriptValueID ) ) );
	}
	
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
	
}

