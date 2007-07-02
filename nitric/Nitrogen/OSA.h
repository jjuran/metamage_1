// Nitrogen/OSA.h
// --------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


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

#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_SHARED_H
#include "Nucleus/Shared.h"
#endif


namespace Nitrogen
{
	
	class OSAErrorsRegistrationDependency
	{
		public:
			OSAErrorsRegistrationDependency();
	};
	
	
	static const ComponentType kOSAComponentType = ComponentType( ::kOSAComponentType );
	
	static const ComponentSubType kOSAGenericScriptingComponentSubtype = ComponentSubType( ::kOSAGenericScriptingComponentSubtype );
	
	static const AEEventClass kOSASuite = AEEventClass( ::kOSASuite );
	
	static const ResType kOSAScriptResourceType = ResType( ::kOSAScriptResourceType );
	
	static const DescType typeOSAGenericStorage = DescType( ::typeOSAGenericStorage );
	
	typedef Nucleus::ID< class OSAID_Tag, ::OSAID >::Type OSAID;
	
	static const OSAID kOSANullScript = OSAID( ::kOSANullScript );
	
	typedef Nucleus::Flag< class OSAModeFlagsTag, long >::Type OSAModeFlags;
	
	static const OSAModeFlags kOSAModeNull = OSAModeFlags( ::kOSAModeNull );
	
	static const OSAModeFlags kOSAModeDisplayForHumans = OSAModeFlags( ::kOSAModeDisplayForHumans );
	
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
			(void) Nitrogen::OSAErrorsRegistrationDependency();
			
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
	
	Nucleus::Owned< AEDesc > OSAScriptError( ComponentInstance  scriptingComponent,
	                                         AEKeyword          selector,
	                                         DescType           desiredType );
	
	inline void OSADispose( Nucleus::Owned< OSASpec > )  {}
	
	// OSASetScriptInfo
	// OSAGetScriptInfo
	// OSASetActiveProc
	// OSAGetActiveProc
	
	Nucleus::Owned< AEDesc > OSAScriptingComponentName( ComponentInstance scriptingComponent );
	
	Nucleus::Owned< OSASpec > OSACompile( Nucleus::Shared< ComponentInstance >  scriptingComponent, 
	                                      const AEDesc&                         sourceData, 
	                                      OSAModeFlags                          modeFlags = OSAModeFlags ());
	
	Nucleus::Owned< OSASpec > OSACompile( Nucleus::Shared< ComponentInstance >  scriptingComponent, 
	                                      const AEDesc&                         sourceData, 
	                                      OSAModeFlags                          modeFlags, 
	                                      Nucleus::Owned< OSASpec >             previousScriptID );
	
	void OSACopyID( OSAID                       fromID,
	                Nucleus::Owned< OSASpec >&  to );
	
	void OSACopyID( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                OSAID                                 fromID,
	                Nucleus::Owned< OSASpec >&            to );
	
	inline void OSACopyID( const OSASpec&              from,
	                       Nucleus::Owned< OSASpec >&  to )
	{
		OSACopyID( from.component, from.id, to );
	}
	
	Nucleus::Owned< OSASpec > OSACopyID( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                     OSAID                                 fromID );
	
	inline Nucleus::Owned< OSASpec > OSACopyID( const OSASpec& fromID )
	{
		return OSACopyID( fromID.component, fromID.id );
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
	
	Nucleus::Owned< OSASpec > OSAExecuteEvent( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                           const AppleEvent&                     appleEvent,
	                                           OSAID                                 contextID,
	                                           OSAModeFlags                          modeFlags = OSAModeFlags() );
	
	inline Nucleus::Owned< OSASpec > OSAExecuteEvent( const AppleEvent&  appleEvent,
	                                                  OSASpec            context,
	                                                  OSAModeFlags       modeFlags = OSAModeFlags() )
	{
		return OSAExecuteEvent( context.component, appleEvent, context.id, modeFlags );
	}
	
	Nucleus::Owned< AppleEvent > OSADoEvent( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                         const AppleEvent&                     appleEvent,
	                                         OSAID                                 contextID,
	                                         OSAModeFlags                          modeFlags = OSAModeFlags() );
	
	inline Nucleus::Owned< AppleEvent > OSADoEvent( const AppleEvent&  appleEvent,
	                                                OSASpec            context,
	                                                OSAModeFlags       modeFlags = OSAModeFlags() )
	{
		return OSADoEvent( context.component, appleEvent, context.id, modeFlags );
	}
	
	Nucleus::Owned< OSASpec > OSAMakeContext( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                          const AEDesc&                         contextName,
	                                          OSAID                                 parentContext = OSAID() );
	
	inline Nucleus::Owned< OSASpec > OSAMakeContext( Nucleus::Shared< ComponentInstance >  scriptingComponent,
	                                                 OSAID                                 parentContext = OSAID() )
	{
		return OSAMakeContext( scriptingComponent, Nucleus::Make< AEDesc >(), parentContext );
	}
	
	inline Nucleus::Owned< OSASpec > OSAMakeContext( const AEDesc&  contextName,
	                                                 OSASpec        parentContext )
	{
		return OSAMakeContext( parentContext.component, contextName, parentContext.id );
	}
	
	inline Nucleus::Owned< OSASpec > OSAMakeContext( OSASpec parentContext )
	{
		return OSAMakeContext( parentContext.component, parentContext.id );
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
	
}

#endif

