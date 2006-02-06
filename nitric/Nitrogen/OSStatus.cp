// OSStatus.cp

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
  {
  
#ifdef	NITROGEN_DEBUG
  static OSStatusLoggingProc gOSStatusLoggingProc = NULL;
//	SetOSStatusLoggingProc
//	
//	Sets the logging proc
  OSStatusLoggingProc SetOSStatusLoggingProc ( OSStatusLoggingProc newProc ) {
  	OSStatusLoggingProc retVal = gOSStatusLoggingProc;
  	gOSStatusLoggingProc = newProc;
  	return retVal;
  	}
  	


//	Debug version of ThrowOSStatusInternal
//	Calls the user-installed logging proc if there's an error
  void ThrowOSStatusInternal( OSStatus error, const char *file, int line )
     {
      if ( error != noErr )
      	 {
      	 if ( NULL != gOSStatusLoggingProc )
      	    {
      	    try { gOSStatusLoggingProc ( error, file, line ); } catch (...) {}
      	    }
         Nucleus::ThrowErrorCode< OSStatus >( error );
         }
     }

#else
//	Non-Debug version of ThrowOSStatusInternal
//	Just throws the error
   void ThrowOSStatusInternal( OSStatus error )
     {
      if ( error != noErr )
         Nucleus::ThrowErrorCode< OSStatus >( error );
     }
#endif
  }
