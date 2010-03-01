// Nitrogen/OSA.cc
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/OSA.hh"


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
	
	
	nucleus::owned< OSAID >
	//
	OSALoad( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	         const AEDesc&                                scriptData,
	         OSAModeFlags                                 modeFlags )
	{
		::OSAID resultingScriptID;
		
		ThrowOSStatus( ::OSALoad( scriptingComponent,
		                          &scriptData,
		                          modeFlags,
		                          &resultingScriptID ) );
		
		return nucleus::owned< OSAID >::seize( OSAID( resultingScriptID ),
		                                       scriptingComponent );
	}
	
	
	nucleus::owned< AEDesc_Data >
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
		
		return nucleus::owned< AEDesc_Data >::seize( resultingScriptData );
	}
	
	
	nucleus::owned< OSAID >
	//
	OSAExecute( const nucleus::shared< ComponentInstance >&  scriptingComponent,
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
		
		return nucleus::owned< OSAID >::seize( OSAID( resultingScriptValueID ),
		                                       scriptingComponent );
	}
	
	nucleus::owned< AEDesc_Data >
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
		
		return nucleus::owned< AEDesc_Data >::seize( resultingText );
	}
	
	
	nucleus::owned< AEDesc_Data > OSAScriptError( ComponentInstance  scriptingComponent,
	                                              AEKeyword          selector,
	                                              DescType           desiredType )
	{
		AEDesc_Data resultingErrorDescription;
		
		ThrowOSStatus( ::OSAScriptError( scriptingComponent,
		                                 selector,
		                                 desiredType,
		                                 &resultingErrorDescription ) );
		
		return nucleus::owned< AEDesc_Data >::seize( resultingErrorDescription );
	}
	
	nucleus::owned< AEDesc_Data > OSAScriptingComponentName( ComponentInstance scriptingComponent )
	{
		AEDesc_Data resultingScriptingComponentName;
		
		ThrowOSStatus( ::OSAScriptingComponentName( scriptingComponent,
		                                            &resultingScriptingComponentName ) );
		
		return nucleus::owned< AEDesc_Data >::seize( resultingScriptingComponentName );
	}
	
	
	nucleus::owned< OSAID >
	//
	OSACompile( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	            const AEDesc&                                sourceData,
	            OSAModeFlags                                 modeFlags )
	{
		::OSAID previousAndResultingScriptID = kOSANullScript;
		
		ThrowOSStatus( ::OSACompile( scriptingComponent,
		                             &sourceData,
		                             modeFlags,
		                             &previousAndResultingScriptID ) );
		
		return nucleus::owned< OSAID >::seize( OSAID( previousAndResultingScriptID ),
		                                              scriptingComponent );
	}
	
	nucleus::owned< OSAID >
	//
	OSACompile( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	            const AEDesc&                                sourceData,
	            OSAModeFlags                                 modeFlags,
	            nucleus::owned< OSAID >                      previousScriptID )
	{
		::OSAID previousAndResultingScriptID = previousScriptID.release();
		
		OSAError err = ::OSACompile( scriptingComponent,
		                             &sourceData,
		                             modeFlags,
		                             &previousAndResultingScriptID );
		
		nucleus::owned< OSAID > resultingScriptID
			= nucleus::owned< OSAID >::seize( OSAID( previousAndResultingScriptID ),
			                                  scriptingComponent );
		
		if ( err != noErr )
		{
			// resultingScriptID gets lost (but not leaked)
		}
		
		ThrowOSStatus( err );
		
		return resultingScriptID;
	}
	
	
	void OSACopyID( OSAID                     fromID,
	                nucleus::owned< OSAID >&  to )
	{
		::OSAID toID = to.get();
		
		ThrowOSStatus( ::OSACopyID( to.disposer().Component(),
		                            fromID,
		                            &toID ) );
		
		if ( toID != to.get() )
		{
			to.release();
			
			to = nucleus::owned< OSAID >::seize( OSAID( toID ),
			                                     to.disposer().Component() );
		}
	}
	
	void OSACopyID( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	                OSAID                                        fromID,
	                nucleus::owned< OSAID >&                     to )
	{
		if ( scriptingComponent.get() != to.disposer().Component() )
		{
			ThrowOSStatus( errOSAComponentMismatch );
		}
		
		OSACopyID( fromID, to );
	}
	
	nucleus::owned< OSAID >
	//
	OSACopyID( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	           OSAID                                        fromID )
	{
		::OSAID toID = kOSANullScript;
		
		ThrowOSStatus( ::OSACopyID( scriptingComponent,
		                            fromID,
		                            &toID ) );
		
		return nucleus::owned< OSAID >::seize( OSAID( toID ),
		                                       scriptingComponent );
	}
	
	
	nucleus::owned< OSAID >
	//
	OSAExecuteEvent( const nucleus::shared< ComponentInstance >&  scriptingComponent,
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
		
		return nucleus::owned< OSAID >::seize( OSAID( resultingScriptValueID ),
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

