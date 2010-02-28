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
   AEEventHandler::AEEventHandler()
     : theAEEventClass(),
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

   Nucleus::Owned<AEEventHandler>
   AEInstallEventHandler( const AEEventHandler& toInstall )
     {
      ThrowOSStatus( ::AEInstallEventHandler( toInstall.theAEEventClass,
                                              toInstall.theAEEventID,
                                              toInstall.handler,
                                              toInstall.handlerRefCon,
                                              toInstall.isSysHandler ) );
      
      return Nucleus::Owned<AEEventHandler>::Seize( toInstall );
     }

   void AERemoveEventHandler( Nucleus::Owned<AEEventHandler> toRemove )
     {
      AEEventHandler released = toRemove.release();
      
      if ( released.handler != AEEventHandlerUPP() )
         ThrowOSStatus( ::AERemoveEventHandler( released.theAEEventClass,
                                                released.theAEEventID,
                                                released.handler,
                                                released.isSysHandler ) );
     }

   AEEventHandler AEGetEventHandler( AEEventClass theAEEventClass,
                                     AEEventID    theAEEventID,
                                     bool         isSysHandler )
     {
      ::AEEventHandlerUPP handler;
      long handlerRefCon;

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
  }
