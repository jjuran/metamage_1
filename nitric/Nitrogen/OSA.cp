// Nitrogen/OSA.cp
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/OSA.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( OSA )
	
	
	static void RegisterOSAErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class OSAErrorsRegistration
	{
		public:
			OSAErrorsRegistration()  { RegisterOSAErrors(); }
	};
	
	static OSAErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	Nucleus::Owned< OSAID >
	//
	OSALoad( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	         const AEDesc&                                scriptData,
	         OSAModeFlags                                 modeFlags )
	{
		::OSAID resultingScriptID;
		
		ThrowOSStatus( ::OSALoad( scriptingComponent,
		                          &scriptData,
		                          modeFlags,
		                          &resultingScriptID ) );
		
		return Nucleus::Owned< OSAID >::Seize( OSAID( resultingScriptID ),
		                                       scriptingComponent );
	}
	
	
	Nucleus::Owned< AEDesc_Data >
	//
	OSAStore( ComponentInstance  scriptingComponent,
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
	
	
	Nucleus::Owned< OSAID >
	//
	OSAExecute( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	            OSAID                                        compiledScriptID,
	            OSAID                                        contextID,
	            OSAModeFlags                                 modeFlags )
	{
		::OSAID resultingScriptValueID;
		
		ThrowOSStatus( ::OSAExecute( scriptingComponent,
		                             compiledScriptID,
		                             contextID,
		                             modeFlags,
		                             &resultingScriptValueID ) );
		
		return Nucleus::Owned< OSAID >::Seize( OSAID( resultingScriptValueID ),
		                                       scriptingComponent );
	}
	
	Nucleus::Owned< AEDesc_Data >
	//
	OSADisplay( ComponentInstance  scriptingComponent,
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
	
	
	Nucleus::Owned< OSAID >
	//
	OSACompile( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	            const AEDesc&                                sourceData,
	            OSAModeFlags                                 modeFlags )
	{
		::OSAID previousAndResultingScriptID = kOSANullScript;
		
		ThrowOSStatus( ::OSACompile( scriptingComponent,
		                             &sourceData,
		                             modeFlags,
		                             &previousAndResultingScriptID ) );
		
		return Nucleus::Owned< OSAID >::Seize( OSAID( previousAndResultingScriptID ),
		                                              scriptingComponent );
	}
	
	Nucleus::Owned< OSAID >
	//
	OSACompile( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	            const AEDesc&                                sourceData,
	            OSAModeFlags                                 modeFlags,
	            Nucleus::Owned< OSAID >                      previousScriptID )
	{
		::OSAID previousAndResultingScriptID = previousScriptID.Release();
		
		OSAError err = ::OSACompile( scriptingComponent,
		                             &sourceData,
		                             modeFlags,
		                             &previousAndResultingScriptID );
		
		Nucleus::Owned< OSAID > resultingScriptID
			= Nucleus::Owned< OSAID >::Seize( OSAID( previousAndResultingScriptID ),
			                                  scriptingComponent );
		
		if ( err != noErr )
		{
			// resultingScriptID gets lost (but not leaked)
		}
		
		ThrowOSStatus( err );
		
		return resultingScriptID;
	}
	
	
	void OSACopyID( OSAID                     fromID,
	                Nucleus::Owned< OSAID >&  to )
	{
		::OSAID toID = to.Get();
		
		ThrowOSStatus( ::OSACopyID( to.GetDisposer().Component(),
		                            fromID,
		                            &toID ) );
		
		if ( toID != to.Get() )
		{
			to.Release();
			
			to = Nucleus::Owned< OSAID >::Seize( OSAID( toID ),
			                                     to.GetDisposer().Component() );
		}
	}
	
	void OSACopyID( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	                OSAID                                        fromID,
	                Nucleus::Owned< OSAID >&                     to )
	{
		if ( scriptingComponent.Get() != to.GetDisposer().Component() )
		{
			throw ErrOSAComponentMismatch();
		}
		
		OSACopyID( fromID, to );
	}
	
	Nucleus::Owned< OSAID >
	//
	OSACopyID( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	           OSAID                                        fromID )
	{
		::OSAID toID = kOSANullScript;
		
		ThrowOSStatus( ::OSACopyID( scriptingComponent,
		                            fromID,
		                            &toID ) );
		
		return Nucleus::Owned< OSAID >::Seize( OSAID( toID ),
		                                       scriptingComponent );
	}
	
	
	Nucleus::Owned< OSAID >
	//
	OSAExecuteEvent( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	                 const AppleEvent&                            appleEvent,
	                 OSAID                                        contextID,
	                 OSAModeFlags                                 modeFlags )
	{
		::OSAID resultingScriptValueID;
		
		ThrowOSStatus( ::OSAExecuteEvent( scriptingComponent,
		                                  &appleEvent,
		                                  contextID,
		                                  modeFlags,
		                                  &resultingScriptValueID ) );
		
		return Nucleus::Owned< OSAID >::Seize( OSAID( resultingScriptValueID ),
		                                       scriptingComponent );
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

