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

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
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

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_COMPONENTS_HH
#include "Nitrogen/Components.hh"
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( OSA );
	
	
	static const ComponentType kOSAComponentType = ComponentType( ::kOSAComponentType );
	
	static const ComponentSubType kOSAGenericScriptingComponentSubtype = ComponentSubType( ::kOSAGenericScriptingComponentSubtype );
	
	
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
				
				(void) ::OSADispose( itsComponent.get(), id );
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
	
	nucleus::owned< Mac::AEDesc_Data >
	//
	OSAStore( ComponentInstance  scriptingComponent,
	          OSAID              scriptID,
	          Mac::DescType      desiredType = Mac::typeOSAGenericStorage,
	          OSAModeFlags       modeFlags   = kOSAModeNull );
	
	inline nucleus::owned< Mac::AEDesc_Data >
	//
	OSAStore( const nucleus::owned< OSAID >&  script,
	          Mac::DescType                   desiredType = Mac::typeOSAGenericStorage,
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
	
	nucleus::owned< Mac::AEDesc_Data >
	//
	OSADisplay( ComponentInstance  scriptingComponent,
	            OSAID              scriptValueID,
	            Mac::DescType      desiredType = Mac::typeChar,
	            OSAModeFlags       modeFlags   = OSAModeFlags() );
	
	inline nucleus::owned< Mac::AEDesc_Data >
	//
	OSADisplay( const nucleus::owned< OSAID >&  scriptValue,
	            Mac::DescType                   desiredType = Mac::typeChar,
	            OSAModeFlags                    modeFlags   = OSAModeFlags() )
	{
		return OSADisplay( scriptValue.disposer().Component(),
		                   scriptValue.get(),
		                   desiredType,
		                   modeFlags );
	}
	
	
	nucleus::owned< Mac::AEDesc_Data > OSAScriptError( ComponentInstance  scriptingComponent,
	                                                   Mac::AEKeyword     selector,
	                                                   Mac::DescType      desiredType );
	
	inline void OSADispose( nucleus::owned< OSAID > )
	{
	}
	
	// OSASetScriptInfo
	// OSAGetScriptInfo
	// OSASetActiveProc
	// OSAGetActiveProc
	
	nucleus::owned< Mac::AEDesc_Data > OSAScriptingComponentName( ComponentInstance scriptingComponent );
	
	
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
	                 const Mac::AppleEvent&                       appleEvent,
	                 OSAID                                        contextID,
	                 OSAModeFlags                                 modeFlags = OSAModeFlags() );
	
	inline nucleus::owned< OSAID >
	//
	OSAExecuteEvent( const Mac::AppleEvent&          appleEvent,
	                 const nucleus::owned< OSAID >&  context,
	                 OSAModeFlags                    modeFlags = OSAModeFlags() )
	{
		return OSAExecuteEvent( context.disposer().Component(),
		                        appleEvent,
		                        context.get(),
		                        modeFlags );
	}
	
	
	nucleus::owned< Mac::AppleEvent >
	//
	OSADoEvent( nucleus::shared< ComponentInstance >  scriptingComponent,
	            const Mac::AppleEvent&                appleEvent,
	            OSAID                                 contextID,
	            OSAModeFlags                          modeFlags = OSAModeFlags() );
	
	inline nucleus::owned< Mac::AppleEvent >
	//
	OSADoEvent( const Mac::AppleEvent&          appleEvent,
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
	
	template < Mac::DescType desiredType >
	inline
	typename DescType_result< desiredType >::type
	OSAScriptError( ComponentInstance scriptingComponent, Mac::AEKeyword keyword )
	{
		return AEGetDescData< desiredType >( OSAScriptError( scriptingComponent,
		                                                     keyword,
		                                                     desiredType ) );
	}
	
	template < Mac::AEKeyword key >
	inline
	typename AEKeyword_result< key >::type
	OSAScriptError( ComponentInstance scriptingComponent )
	{
		return OSAScriptError< AEKeyword_DescType< key >::value >( scriptingComponent, key );
	}
	
}

#endif
