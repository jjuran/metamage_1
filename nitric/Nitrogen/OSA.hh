// Nitrogen/OSA.hh
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_OSA_HH
#define NITROGEN_OSA_HH

#ifndef __OSA__
#include <OSA.h>
#endif

// Debug
#include "debug/assert.hh"

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_SHARED_HH
#include "nucleus/shared.hh"
#endif

#ifndef NITROGEN_APPLEEVENTS_HH
#include "Nitrogen/AppleEvents.hh"
#endif
#ifndef NITROGEN_COMPONENTS_HH
#include "Nitrogen/Components.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( OSA );
	
	
	static const ComponentType kOSAComponentType = ComponentType( ::kOSAComponentType );
	
	static const ComponentSubType kOSAGenericScriptingComponentSubtype = ComponentSubType( ::kOSAGenericScriptingComponentSubtype );
	
	static const AEEventClass kOSASuite = AEEventClass( ::kOSASuite );
	
	static const ResType kOSAScriptResourceType = ResType( ::kOSAScriptResourceType );
	
	static const DescType typeOSAGenericStorage = DescType( ::typeOSAGenericStorage );
	
	
	enum OSAID
	{
		kOSANullScript = ::kOSANullScript,
		
		kOSAID_Max = nucleus::enumeration_traits< ::OSAID >::max
	};
	
	enum OSAModeFlags
	{
		kOSAModeNull                    = ::kOSAModeNull,
		kOSAModePreventGetSource        = ::kOSAModePreventGetSource,
		kOSAModeNeverInteract           = ::kOSAModeNeverInteract,
		kOSAModeCanInteract             = ::kOSAModeCanInteract,
		kOSAModeAlwaysInteract          = ::kOSAModeAlwaysInteract,
		kOSAModeDontReconnect           = ::kOSAModeDontReconnect,
		kOSAModeCantSwitchLayer         = ::kOSAModeCantSwitchLayer,
		kOSAModeDoRecord                = ::kOSAModeDoRecord,
		kOSAModeCompileIntoContext      = ::kOSAModeCompileIntoContext,
		kOSAModeAugmentContext          = ::kOSAModeAugmentContext,
		kOSAModeDisplayForHumans        = ::kOSAModeDisplayForHumans,
		kOSAModeDontStoreParent         = ::kOSAModeDontStoreParent,
		kOSAModeDispatchToDirectObject  = ::kOSAModeDispatchToDirectObject,
		kOSAModeDontGetDataForArguments = ::kOSAModeDontGetDataForArguments,
		
		kOSAModeFlags_Max = nucleus::enumeration_traits< SInt32 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( OSAModeFlags )
	
}

namespace nucleus
{
	
	template <>
	struct disposer< Nitrogen::OSAID >
	{
		typedef Nitrogen::OSAID  argument_type;
		typedef void             result_type;
		
		private:
			shared< ComponentInstance > itsComponent;
		
		public:
			disposer()
			{
			}
			
			disposer( const shared< ComponentInstance >& component )
			:
				itsComponent( component )
			{
			}
			
			const shared< ComponentInstance >& Component() const
			{
				return itsComponent;
			}
			
			void operator()( Nitrogen::OSAID id ) const
			{
				NUCLEUS_REQUIRE_ERRORS( Nitrogen::OSA );
				
				ASSERT( itsComponent.get() != ComponentInstance() );
				
				::Nitrogen::HandleDestructionOSStatus( ::OSADispose( itsComponent.get(), id ) );
			}
	};
	
}

namespace Nitrogen
{
	
	// OSALoad
	
	nucleus::owned< OSAID >
	//
	OSALoad( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	         const AEDesc&                                scriptData,
	         OSAModeFlags                                 modeFlags = OSAModeFlags() );
	
	
	// OSAStore
	
	nucleus::owned< AEDesc_Data >
	//
	OSAStore( ComponentInstance  scriptingComponent,
	          OSAID              scriptID,
	          DescType           desiredType = typeOSAGenericStorage,
	          OSAModeFlags       modeFlags   = kOSAModeNull );
	
	inline nucleus::owned< AEDesc_Data >
	//
	OSAStore( const nucleus::owned< OSAID >&  script,
	          DescType                        desiredType = typeOSAGenericStorage,
	          OSAModeFlags                    modeFlags   = OSAModeFlags() )
	{
		return OSAStore( script.disposer().Component(),
		                 script.get(),
		                 desiredType,
		                 modeFlags );
	}
	
	
	// OSAExecute
	
	nucleus::owned< OSAID >
	//
	OSAExecute( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	            OSAID                                        compiledScriptID,
	            OSAID                                        contextID = OSAID(),
	            OSAModeFlags                                 modeFlags = kOSAModeNull );
	
	inline nucleus::owned< OSAID >
	//
	OSAExecute( const nucleus::owned< OSAID >&  script,
	            OSAID                           contextID = OSAID(),
	            OSAModeFlags                    modeFlags = OSAModeFlags() )
	{
		return OSAExecute( script.disposer().Component(),
		                   script.get(),
		                   contextID,
		                   modeFlags );
	}
	
	
	// OSADisplay
	
	nucleus::owned< AEDesc_Data >
	//
	OSADisplay( ComponentInstance  scriptingComponent,
	            OSAID              scriptValueID,
	            DescType           desiredType = typeChar,
	            OSAModeFlags       modeFlags   = OSAModeFlags() );
	
	inline nucleus::owned< AEDesc_Data >
	//
	OSADisplay( const nucleus::owned< OSAID >&  scriptValue,
	            DescType                        desiredType = typeChar,
	            OSAModeFlags                    modeFlags   = OSAModeFlags() )
	{
		return OSADisplay( scriptValue.disposer().Component(),
		                   scriptValue.get(),
		                   desiredType,
		                   modeFlags );
	}
	
	
	nucleus::owned< AEDesc_Data > OSAScriptError( ComponentInstance  scriptingComponent,
	                                              AEKeyword          selector,
	                                              DescType           desiredType );
	
	inline void OSADispose( nucleus::owned< OSAID > )
	{
	}
	
	// OSASetScriptInfo
	// OSAGetScriptInfo
	// OSASetActiveProc
	// OSAGetActiveProc
	
	nucleus::owned< AEDesc_Data > OSAScriptingComponentName( ComponentInstance scriptingComponent );
	
	
	// OSACompile
	
	nucleus::owned< OSAID >
	//
	OSACompile( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	            const AEDesc&                                sourceData,
	            OSAModeFlags                                 modeFlags = OSAModeFlags() );
	
	nucleus::owned< OSAID >
	//
	OSACompile( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	            const AEDesc&                                sourceData,
	            OSAModeFlags                                 modeFlags,
	            nucleus::owned< OSAID >                      previousScriptID );
	
	
	// OSACopyID
	
	void OSACopyID( OSAID                     fromID,
	                nucleus::owned< OSAID >&  to );
	
	void OSACopyID( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	                OSAID                                        fromID,
	                nucleus::owned< OSAID >&                     to );
	
	inline void OSACopyID( const nucleus::owned< OSAID >&  from,
	                       nucleus::owned< OSAID >&        to )
	{
		OSACopyID( from.disposer().Component(),
		           from.get(),
		           to );
	}
	
	nucleus::owned< OSAID >
	//
	OSACopyID( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	           OSAID                                        fromID );
	
	inline nucleus::owned< OSAID >
	//
	OSACopyID( const nucleus::owned< OSAID >& fromID )
	{
		return OSACopyID( fromID.disposer().Component(),
		                  fromID.get() );
	}
	
	
	// OSAGetSource
	// OSACoerceFromDesc
	// OSACoerceToDesc
	
	// OSASetSendProc
	// OSAGetSendProc
	// OSASetCreateProc
	// OSAGetCreateProc
	// OSASetDefaultTarget
	
	// OSAStartRecording
	// OSAStopRecording
	
	// OSALoadExecute
	// OSACompileExecute
	// OSADoScript
	
	// OSASetCurrentDialect
	// OSAGetCurrentDialect
	// OSAAvailableDialects
	// OSAGetDialectInfo
	// OSAAvailableDialectCodeList
	
	// OSASetResumeDispatchProc
	// OSAGetResumeDispatchProc
	
	nucleus::owned< OSAID >
	//
	OSAExecuteEvent( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	                 const AppleEvent&                            appleEvent,
	                 OSAID                                        contextID,
	                 OSAModeFlags                                 modeFlags = OSAModeFlags() );
	
	inline nucleus::owned< OSAID >
	//
	OSAExecuteEvent( const AppleEvent&               appleEvent,
	                 const nucleus::owned< OSAID >&  context,
	                 OSAModeFlags                    modeFlags = OSAModeFlags() )
	{
		return OSAExecuteEvent( context.disposer().Component(),
		                        appleEvent,
		                        context.get(),
		                        modeFlags );
	}
	
	
	nucleus::owned< AppleEvent >
	//
	OSADoEvent( nucleus::shared< ComponentInstance >  scriptingComponent,
	            const AppleEvent&                     appleEvent,
	            OSAID                                 contextID,
	            OSAModeFlags                          modeFlags = OSAModeFlags() );
	
	inline nucleus::owned< AppleEvent >
	//
	OSADoEvent( const AppleEvent&               appleEvent,
	            const nucleus::owned< OSAID >&  context,
	            OSAModeFlags                    modeFlags = OSAModeFlags() )
	{
		return OSADoEvent( context.disposer().Component(),
		                   appleEvent,
		                   context.get(),
		                   modeFlags );
	}
	
	
	nucleus::owned< OSAID >
	//
	OSAMakeContext( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	                const AEDesc&                                contextName,
	                OSAID                                        parentContext = OSAID() );
	
	inline nucleus::owned< OSAID >
	//
	OSAMakeContext( const nucleus::shared< ComponentInstance >&  scriptingComponent,
	                OSAID                                        parentContext = OSAID() )
	{
		return OSAMakeContext( scriptingComponent, nucleus::make< AEDesc >(), parentContext );
	}
	
	inline nucleus::owned< OSAID >
	//
	OSAMakeContext( const AEDesc&                   contextName,
	                const nucleus::owned< OSAID >&  parentContext )
	{
		return OSAMakeContext( parentContext.disposer().Component(),
		                       contextName,
		                       parentContext.get() );
	}
	
	inline nucleus::owned< OSAID >
	//
	OSAMakeContext( const nucleus::owned< OSAID >&  parentContext )
	{
		return OSAMakeContext( parentContext.disposer().Component(),
		                       parentContext.get() );
	}
	
	// OSADebuggerCreateSession
	// OSADebuggerGetSessionState
	// OSADebuggerSessionStep
	// OSADebuggerDisposeSession
	// OSADebuggerGetStatementRanges
	// OSADebuggerGetBreakpoint
	// OSADebuggerSetBreakpoint
	// OSADebuggerGetDefaultBreakpoint
	// OSADebuggerGetCurrentCallFrame
	// OSADebuggerGetCallFrameState
	// OSADebuggerGetVariable
	// OSADebuggerSetVariable
	// OSADebuggerGetPreviousCallFrame
	// OSADebuggerDisposeCallFrame
	// OSADebuggerCountVariables
	
	template < DescType desiredType >
	inline
	typename DescType_Traits< desiredType >::Result
	OSAScriptError( ComponentInstance scriptingComponent, AEKeyword keyword )
	{
		return AEGetDescData< desiredType >( OSAScriptError( scriptingComponent,
		                                                     keyword,
		                                                     desiredType ) );
	}
	
	template < AEKeyword key >
	inline
	typename AEKeyword_Traits< key >::Result
	OSAScriptError( ComponentInstance scriptingComponent )
	{
		return OSAScriptError< AEKeyword_Traits< key >::descType >( scriptingComponent, key );
	}
	
}

#endif

