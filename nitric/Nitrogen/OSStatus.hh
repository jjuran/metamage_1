// Nitrogen/OSStatus.hh
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_OSSTATUS_HH
#define NITROGEN_OSSTATUS_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef NUCLEUS_ERRORCODE_HH
#include "nucleus/error_code.hh"
#endif


namespace Nitrogen
  {
   class OSStatus
     {
      private:
         ::OSStatus status;
         
         // Not implemented:
            OSStatus(          bool      );
            OSStatus(          char      );
            OSStatus(   signed char      );
            OSStatus(   signed long long );
            OSStatus( unsigned char      );
            OSStatus( unsigned short     );
            OSStatus( unsigned int       );
            OSStatus( unsigned long      );
            OSStatus( unsigned long long );
         
      public:
         typedef ::OSStatus error_number;
         
         OSStatus()                                            : status( noErr )    {}
         OSStatus( ::OSStatus s )                              : status( s )        {}
      
         OSStatus( ::OSErr s )                                 : status( s )        {}
         OSStatus( signed int s )                              : status( s )        {}
         
         static OSStatus Make( ::OSStatus s )                  { return OSStatus( s ); }
         ::OSStatus Get() const                                { return status; }
         operator ::OSStatus() const                           { return status; }
     };
	
	#define DEFINE_OSSTATUS( c_name, new_name )  DEFINE_ERRORCODE( OSStatus, c_name, new_name )
	
   template < ::OSStatus error >
   inline void RegisterOSStatus()
     {
      ::nucleus::register_error_code< OSStatus, error >();
     }
   
   void ThrowOSStatus_Internal( OSStatus );
   
	inline void ThrowOSStatus( OSStatus err )
	{
		if ( err != noErr )
		{
			ThrowOSStatus_Internal( err );
		}
	}
	
	inline void HandleDestructionOSStatus( ::OSStatus err )
	{
		// ignore errors in destructors for now
	}
	
  }

namespace Nitrogen
{
	
	template < ::OSStatus status >
	struct OSStatus_EndOfEnumeration : public ::nucleus::error_code_end_of_enumeration< OSStatus, status >
	{
	};
	
}

#endif

