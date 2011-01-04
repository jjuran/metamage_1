// Nitrogen/AppleEvents.cc
// -----------------------

// Part of the Nitrogen project.
//
// Written 2002-2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/AppleEvents.hh"


namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	AEEventHandler::AEEventHandler()
	:
		theAEEventClass(),
		theAEEventID(),
		handler(),
		handlerRefCon(),
		isSysHandler()
	{
	}
	
	bool operator==( const AEEventHandler& a, const AEEventHandler& b )
	{
		return a.handler         == b.handler
		    && a.handlerRefCon   == b.handlerRefCon
		    && a.theAEEventID    == b.theAEEventID
		    && a.theAEEventClass == b.theAEEventClass
		    && a.isSysHandler    == b.isSysHandler;
	}
	
	nucleus::owned< AEEventHandler >
	//
	AEInstallEventHandler( const AEEventHandler& toInstall )
	{
		ThrowOSStatus( ::AEInstallEventHandler( toInstall.theAEEventClass,
		                                        toInstall.theAEEventID,
		                                        toInstall.handler,
		                                        toInstall.handlerRefCon,
		                                        toInstall.isSysHandler ) );
		
		return nucleus::owned< AEEventHandler >::seize( toInstall );
	}
	
	void AERemoveEventHandler( nucleus::owned< AEEventHandler > toRemove )
	{
		AEEventHandler released = toRemove.release();
		
		if ( released.handler != AEEventHandlerUPP() )
			ThrowOSStatus( ::AERemoveEventHandler( released.theAEEventClass,
			                                       released.theAEEventID,
			                                       released.handler,
			                                       released.isSysHandler ) );
	}
	
	AEEventHandler AEGetEventHandler( AEEventClass  theAEEventClass,
	                                  AEEventID     theAEEventID,
	                                  bool          isSysHandler )
	{
		::AEEventHandlerUPP  handler;
		long                 handlerRefCon;
		
		ThrowOSStatus( ::AEGetEventHandler( theAEEventClass,
		                                    theAEEventID,
		                                    &handler,
		                                    &handlerRefCon,
		                                    isSysHandler ) );
		
		return AEEventHandler( theAEEventClass,
		                       theAEEventID,
		                       handler,
		                       handlerRefCon,
		                       isSysHandler );
	}
	
	namespace compile_tests
	{
		
		pascal OSErr AE_ProcPtr( const AppleEvent*, AppleEvent*, ::SRefCon );
		
		void AE_no_refcon( const Mac::AppleEvent&, Mac::AppleEvent& );
		
		void AE_WindowRef_refcon( const Mac::AppleEvent&, Mac::AppleEvent&, WindowRef );
		
		static void foo()
		{
			Mac::AEEventClass aeClass = Mac::AEEventClass();
			Mac::AEEventID    aeID    = Mac::AEEventID   ();
			bool isSysHandler         = false;
			
			AEInstallEventHandler< AE_ProcPtr >( aeClass, aeID, 0, isSysHandler );
			
			AEInstallEventHandler< AE_no_refcon >( aeClass, aeID, isSysHandler );
			
			AEInstallEventHandler< AE_no_refcon >( aeClass, aeID );
			
			AEInstallEventHandler< WindowRef, AE_WindowRef_refcon >( aeClass, aeID, WindowRef(), isSysHandler );
			
			AEInstallEventHandler< WindowRef, AE_WindowRef_refcon >( aeClass, aeID, WindowRef() );
		}
		
	}
	
}

