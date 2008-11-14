// Nitrogen/Gestalt.cp
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_GESTALT_H
#include "Nitrogen/Gestalt.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( GestaltManager )
	
	
	static void RegisterGestaltManagerErrors();
	
	
#pragma force_active on
	
	class GestaltManagerErrorsRegistration
	{
		public:
			GestaltManagerErrorsRegistration()  { RegisterGestaltManagerErrors(); }
	};
	
	static GestaltManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
   long Gestalt( Gestalt_Selector selector )
     {
      long result;
      ThrowOSStatus( ::Gestalt( selector, &result ) );
      return result;
     }

   long Gestalt( Gestalt_Selector selector, long defaultValue )
     {
      try
        {
         return Gestalt( selector );
        }
      catch ( const Nucleus::ErrorCode< OSStatus, gestaltUndefSelectorErr >& )
        {}
	  return defaultValue;
     }

   void RegisterGestaltManagerErrors()
     {
      RegisterOSStatus< memFullErr              >();
      RegisterOSStatus< envNotPresent           >();
      RegisterOSStatus< envBadVers              >();
      RegisterOSStatus< envVersTooBig           >();
      RegisterOSStatus< gestaltUnknownErr       >();
      RegisterOSStatus< gestaltUndefSelectorErr >();
      RegisterOSStatus< gestaltDupSelectorErr   >();
      RegisterOSStatus< gestaltLocationErr      >();
     }
  }
