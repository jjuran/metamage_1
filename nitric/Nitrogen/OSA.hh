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
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

#ifndef NITROGEN_APPLEEVENTS_HH
#include "Nitrogen/AppleEvents.hh"
#endif
#ifndef NITROGEN_COMPONENTS_HH
#include "Nitrogen/Components.hh"
#endif

#ifndef NUCLEUS_SHARED_H
#include "Nucleus/Shared.h"
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

namespace Nucleus
{
	
	template <>
	struct Disposer< Nitrogen::OSAID > : public std::unary_function< Nitrogen::OSAID, void >,
	                                     private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		private:
			Shared< Nitrogen::ComponentInstance > itsComponent;
		
		public:
			Disposer()
			{
			}
			
			Disposer( const Shared< Nitrogen::ComponentInstance >& component )
			:
				itsComponent( component )
			{
			}
			
			const Shared< Nitrogen::ComponentInstance >& Component() const
			{
				return itsComponent;
			}
			
			void operator()( Nitrogen::OSAID id ) const
			{
				NUCLEUS_REQUIRE_ERRORS( Nitrogen::OSA );
				
				ASSERT( itsComponent.get() != ComponentInstance() );
				
				HandleDestructionOSStatus( ::OSADispose( itsComponent.get(), id ) );
			}
	};
	
}

namespace Nitrogen
{
	
	// OSALoad
	
	Nucleus::Owned< OSAID >
	//
	OSALoad( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	         const AEDesc&                                scriptData,
	         OSAModeFlags                                 modeFlags = OSAModeFlags() );
	
	
	// OSAStore
	
	Nucleus::Owned< AEDesc_Data >
	//
	OSAStore( ComponentInstance  scriptingComponent,
	          OSAID              scriptID,
	          DescType           desiredType = typeOSAGenericStorage,
	          OSAModeFlags       modeFlags   = kOSAModeNull );
	
	inline Nucleus::Owned< AEDesc_Data >
	//
	OSAStore( const Nucleus::Owned< OSAID >&  script,
	          DescType                        desiredType = typeOSAGenericStorage,
	          OSAModeFlags                    modeFlags   = OSAModeFlags() )
	{
		return OSAStore( script.disposer().Component(),
		                 script.get(),
		                 desiredType,
		                 modeFlags );
	}
	
	
	// OSAExecute
	
	Nucleus::Owned< OSAID >
	//
	OSAExecute( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	            OSAID                                        compiledScriptID,
	            OSAID                                        contextID = OSAID(),
	            OSAModeFlags                                 modeFlags = kOSAModeNull );
	
	inline Nucleus::Owned< OSAID >
	//
	OSAExecute( const Nucleus::Owned< OSAID >&  script,
	            OSAID                           contextID = OSAID(),
	            OSAModeFlags                    modeFlags = OSAModeFlags() )
	{
		return OSAExecute( script.disposer().Component(),
		                   script.get(),
		                   contextID,
		                   modeFlags );
	}
	
	
	// OSADisplay
	
	Nucleus::Owned< AEDesc_Data >
	//
	OSADisplay( ComponentInstance  scriptingComponent,
	            OSAID              scriptValueID,
	            DescType           desiredType = typeChar,
	            OSAModeFlags       modeFlags   = OSAModeFlags() );
	
	inline Nucleus::Owned< AEDesc_Data >
	//
	OSADisplay( const Nucleus::Owned< OSAID >&  scriptValue,
	            DescType                        desiredType = typeChar,
	            OSAModeFlags                    modeFlags   = OSAModeFlags() )
	{
		return OSADisplay( scriptValue.disposer().Component(),
		                   scriptValue.get(),
		                   desiredType,
		                   modeFlags );
	}
	
	
	Nucleus::Owned< AEDesc_Data > OSAScriptError( ComponentInstance  scriptingComponent,
	                                              AEKeyword          selector,
	                                              DescType           desiredType );
	
	inline void OSADispose( Nucleus::Owned< OSAID > )
	{
	}
	
	// OSASetScriptInfo
	// OSAGetScriptInfo
	// OSASetActiveProc
	// OSAGetActiveProc
	
	Nucleus::Owned< AEDesc_Data > OSAScriptingComponentName( ComponentInstance scriptingComponent );
	
	
	// OSACompile
	
	Nucleus::Owned< OSAID >
	//
	OSACompile( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	            const AEDesc&                                sourceData,
	            OSAModeFlags                                 modeFlags = OSAModeFlags() );
	
	Nucleus::Owned< OSAID >
	//
	OSACompile( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	            const AEDesc&                                sourceData,
	            OSAModeFlags                                 modeFlags,
	            Nucleus::Owned< OSAID >                      previousScriptID );
	
	
	// OSACopyID
	
	void OSACopyID( OSAID                     fromID,
	                Nucleus::Owned< OSAID >&  to );
	
	void OSACopyID( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	                OSAID                                        fromID,
	                Nucleus::Owned< OSAID >&                     to );
	
	inline void OSACopyID( const Nucleus::Owned< OSAID >&  from,
	                       Nucleus::Owned< OSAID >&        to )
	{
		OSACopyID( from.disposer().Component(),
		           from.get(),
		           to );
	}
	
	Nucleus::Owned< OSAID >
	//
	OSACopyID( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	           OSAID                                        fromID );
	
	inline Nucleus::Owned< OSAID >
	//
	OSACopyID( const Nucleus::Owned< OSAID >& fromID )
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
	
	Nucleus::Owned< OSAID >
	//
	OSAExecuteEvent( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	                 const AppleEvent&                            appleEvent,
	                 OSAID                                        contextID,
	                 OSAModeFlags                                 modeFlags = OSAModeFlags() );
	
	inline Nucleus::Owned< OSAID >
	//
	OSAExecuteEvent( const AppleEvent&               appleEvent,
	                 const Nucleus::Owned< OSAID >&  context,
	                 OSAModeFlags                    modeFlags = OSAModeFlags() )
	{
		return OSAExecuteEvent( context.disposer().Component(),
		                        appleEvent,
		                        context.get(),
		                        modeFlags );
	}
	
	
	Nucleus::Owned< AppleEvent >
	//
	OSADoEvent( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	            const AppleEvent&                     appleEvent,
	            OSAID                                 contextID,
	            OSAModeFlags                          modeFlags = OSAModeFlags() );
	
	inline Nucleus::Owned< AppleEvent >
	//
	OSADoEvent( const AppleEvent&               appleEvent,
	            const Nucleus::Owned< OSAID >&  context,
	            OSAModeFlags                    modeFlags = OSAModeFlags() )
	{
		return OSADoEvent( context.disposer().Component(),
		                   appleEvent,
		                   context.get(),
		                   modeFlags );
	}
	
	
	Nucleus::Owned< OSAID >
	//
	OSAMakeContext( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	                const AEDesc&                                contextName,
	                OSAID                                        parentContext = OSAID() );
	
	inline Nucleus::Owned< OSAID >
	//
	OSAMakeContext( const Nucleus::Shared< ComponentInstance >&  scriptingComponent,
	                OSAID                                        parentContext = OSAID() )
	{
		return OSAMakeContext( scriptingComponent, Nucleus::Make< AEDesc >(), parentContext );
	}
	
	inline Nucleus::Owned< OSAID >
	//
	OSAMakeContext( const AEDesc&                   contextName,
	                const Nucleus::Owned< OSAID >&  parentContext )
	{
		return OSAMakeContext( parentContext.disposer().Component(),
		                       contextName,
		                       parentContext.get() );
	}
	
	inline Nucleus::Owned< OSAID >
	//
	OSAMakeContext( const Nucleus::Owned< OSAID >&  parentContext )
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

