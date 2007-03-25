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
#ifndef NUCLEUS_ERRORCODE_H
#include "Nucleus/ErrorCode.h"
#endif
#ifndef NUCLEUS_EXCEPTION_H
#include "Nucleus/Exception.h"
#endif
#ifndef NUCLEUS_DESTRUCTIONEXCEPTIONPOLICY_H
#include "Nucleus/DestructionExceptionPolicy.h"
#endif
#ifndef NUCLEUS_THEEXCEPTIONBEINGHANDLED_H
#include "Nucleus/TheExceptionBeingHandled.h"
#endif
#ifndef IO_IO_HH
#include "io/io.hh"
#endif

#ifdef NITROGEN_DEBUG
#define	ThrowOSStatus(err)	ThrowOSStatusInternal(err, __FILE__, __LINE__)
#else
#define	ThrowOSStatus(err)	ThrowOSStatusInternal(err)
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
            //operator signed short() const;
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
         operator ::OSErr() const                              { return Nucleus::Convert< ::OSErr >( status ); }

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
   
   template < ::OSStatus error >
   inline void RegisterOSStatus()
     {
      Nucleus::RegisterErrorCode<OSStatus, error>();
     }
   
#ifdef NITROGEN_DEBUG
// Log non-noErr calls to OSStatus
// SetOSStatusLoggingProc returns the old proc to you.
   typedef void (*OSStatusLoggingProc) ( OSStatus, const char *, int );
   OSStatusLoggingProc SetOSStatusLoggingProc ( OSStatusLoggingProc newProc );
   
   void ThrowOSStatusInternal( OSStatus, const char *, int );
#else
   void ThrowOSStatusInternal( OSStatus );
#endif
   
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
            DestructionExceptionPolicy::HandleDestructionException( Nucleus::TheExceptionBeingHandled() );
           }
        }
     };
   
   typedef DestructionOSStatusPolicy< Nucleus::DefaultDestructionExceptionPolicy > DefaultDestructionOSStatusPolicy;
  }

namespace Nucleus
  {
   template <>
   class ErrorCode< Nitrogen::OSStatus, ::memFullErr >: public Nitrogen::OSStatus,
										                public std::bad_alloc
     {
      public:
         ErrorCode()
           : OSStatus( memFullErr )
           {}
         
         const char* what() const throw()  { return "OSStatus -108 (memFullErr)"; }
     };
	
	template <>
	class ErrorCode< Nitrogen::OSStatus, ::eofErr > : public Nitrogen::OSStatus,
	                                                  public io::end_of_input
	{
		public:
			ErrorCode() : OSStatus( eofErr )  {}
	};
	
	template <>
	class ErrorCode< Nitrogen::OSStatus, ::kOTNoDataErr > : public Nitrogen::OSStatus,
	                                                        public io::no_input_pending
	{
		public:
			ErrorCode() : OSStatus( kOTNoDataErr )  {}
	};
	
	
	template <>
	struct Converter< Exception, Nitrogen::OSStatus > : public std::unary_function< Nitrogen::OSStatus, Exception >
	{
		Exception operator()( Nitrogen::OSStatus error ) const
		{
			return Exception( "OSStatus " + Convert< std::string >( error ) );
		}
	};
	
  }

#endif
