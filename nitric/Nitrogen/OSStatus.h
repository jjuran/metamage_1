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
         
      public:
         typedef ::OSStatus ErrorNumber;
         
         OSStatus()                                            : status( noErr )    {}
         OSStatus( ::OSStatus s )                              : status( s )        {}
         
         static OSStatus Make( ::OSStatus s )                  { return OSStatus( s ); }
         ::OSStatus Get() const                                { return status; }
         operator ::OSStatus() const                           { return status; }
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
