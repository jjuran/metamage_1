// OSStatus.h

#ifndef NITROGEN_OSSTATUS_H
#define NITROGEN_OSSTATUS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MACTYPES__
#include FRAMEWORK_HEADER(CarbonCore,MacTypes.h)
#endif
#ifndef __MACERRORS__
#include FRAMEWORK_HEADER(CarbonCore,MacErrors.h)
#endif
#ifndef NITROGEN_ERRORCODE_H
#include "Nitrogen/ErrorCode.h"
#endif
#ifndef NITROGEN_DESTRUCTIONEXCEPTIONPOLICY_H
#include "Nitrogen/DestructionExceptionPolicy.h"
#endif
#ifndef NITROGEN_THEEXCEPTIONBEINGHANDLED_H
#include "Nitrogen/TheExceptionBeingHandled.h"
#endif

namespace Nitrogen
  {
   class OSStatus
     {
      private:
         ::OSStatus status;
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         
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
         
            operator bool() const;
            operator char() const;
            operator signed char() const;
            operator signed short() const;
            operator signed int() const;
            operator signed long long() const;
            operator unsigned char() const;
            operator unsigned short() const;
            operator unsigned int() const;
            operator unsigned long() const;
            operator unsigned long long() const;
            
            friend void operator==( OSStatus,          bool      );
            friend void operator==( OSStatus,          char      );
            friend void operator==( OSStatus,   signed char      );
            friend void operator==( OSStatus,   signed long long );
            friend void operator==( OSStatus, unsigned char      );
            friend void operator==( OSStatus, unsigned short     );
            friend void operator==( OSStatus, unsigned int       );
            friend void operator==( OSStatus, unsigned long      );
            friend void operator==( OSStatus, unsigned long long );

            friend void operator==(          bool,      OSStatus );
            friend void operator==(          char,      OSStatus );
            friend void operator==(   signed char,      OSStatus );
            friend void operator==(   signed long long, OSStatus );
            friend void operator==( unsigned char,      OSStatus );
            friend void operator==( unsigned short,     OSStatus );
            friend void operator==( unsigned int,       OSStatus );
            friend void operator==( unsigned long,      OSStatus );
            friend void operator==( unsigned long long, OSStatus );

            friend void operator!=( OSStatus,          bool      );
            friend void operator!=( OSStatus,          char      );
            friend void operator!=( OSStatus,   signed char      );
            friend void operator!=( OSStatus,   signed long long );
            friend void operator!=( OSStatus, unsigned char      );
            friend void operator!=( OSStatus, unsigned short     );
            friend void operator!=( OSStatus, unsigned int       );
            friend void operator!=( OSStatus, unsigned long      );
            friend void operator!=( OSStatus, unsigned long long );

            friend void operator!=(          bool,      OSStatus );
            friend void operator!=(          char,      OSStatus );
            friend void operator!=(   signed char,      OSStatus );
            friend void operator!=(   signed long long, OSStatus );
            friend void operator!=( unsigned char,      OSStatus );
            friend void operator!=( unsigned short,     OSStatus );
            friend void operator!=( unsigned int,       OSStatus );
            friend void operator!=( unsigned long,      OSStatus );
            friend void operator!=( unsigned long long, OSStatus );
         #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
            
      public:
         typedef ::OSStatus ErrorNumber;
         
         OSStatus()                                            : status( noErr )    {}
         OSStatus( ::OSStatus s )                              : status( s )        {}
      
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         OSStatus( ::OSErr s )                                 : status( s )        {}
         OSStatus( signed int s )                              : status( s )        {}
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
         
         static OSStatus Make( ::OSStatus s )                  { return OSStatus( s ); }
         ::OSStatus Get() const                                { return status; }
         operator ::OSStatus() const                           { return status; }

      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         friend bool operator==( OSStatus a, OSStatus b )      { return a.Get() == b.Get(); }
         friend bool operator!=( OSStatus a, OSStatus b )      { return a.Get() != b.Get(); }
         
         friend bool operator==( OSStatus a, signed short b )  { return a.Get() == b; }
         friend bool operator==( OSStatus a, signed int   b )  { return a.Get() == b; }
         friend bool operator==( OSStatus a, signed long  b )  { return a.Get() == b; }

         friend bool operator==( signed short a, OSStatus b )  { return a == b.Get(); }
         friend bool operator==( signed int   a, OSStatus b )  { return a == b.Get(); }
         friend bool operator==( signed long  a, OSStatus b )  { return a == b.Get(); }

         friend bool operator!=( OSStatus a, signed short b )  { return a.Get() != b; }
         friend bool operator!=( OSStatus a, signed int   b )  { return a.Get() != b; }
         friend bool operator!=( OSStatus a, signed long  b )  { return a.Get() != b; }

         friend bool operator!=( signed short a, OSStatus b )  { return a != b.Get(); }
         friend bool operator!=( signed int   a, OSStatus b )  { return a != b.Get(); }
         friend bool operator!=( signed long  a, OSStatus b )  { return a != b.Get(); }
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
     };
   
   template <>
   class ErrorCode< OSStatus, memFullErr >: public OSStatus,
                                            public std::bad_alloc
     {
      public:
         ErrorCode()
           : OSStatus( memFullErr )
           {}
     };
   
   template < ::OSStatus error >
   void RegisterOSStatus()
     {
      RegisterErrorCode<OSStatus, error>();
     }
   
   void ThrowOSStatus( OSStatus );
   
   template < class DestructionExceptionPolicy >
   struct DestructionOSStatusPolicy: public DestructionExceptionPolicy
     {
      void HandleDestructionOSStatus( OSStatus error ) const
        {
         try
           {
            ThrowOSStatus( error );
            DestructionExceptionPolicy::WarnOfDestructionExceptionRisk();
           }
         catch( ... )
           {
            DestructionExceptionPolicy::HandleDestructionException( TheExceptionBeingHandled() );
           }
        }
     };
   
   typedef DestructionOSStatusPolicy< DefaultDestructionExceptionPolicy > DefaultDestructionOSStatusPolicy;
  }

#endif
