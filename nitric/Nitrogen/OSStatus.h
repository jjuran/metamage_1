// Nitrogen/OSStatus.h
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2007 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


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
         typedef ::OSStatus ErrorNumber;
         
         OSStatus()                                            : status( noErr )    {}
         OSStatus( ::OSStatus s )                              : status( s )        {}
      
         OSStatus( ::OSErr s )                                 : status( s )        {}
         OSStatus( signed int s )                              : status( s )        {}
         
         static OSStatus Make( ::OSStatus s )                  { return OSStatus( s ); }
         ::OSStatus Get() const                                { return status; }
         operator ::OSStatus() const                           { return status; }
     };
   
   template < ::OSStatus error >
   inline void RegisterOSStatus()
     {
      Nucleus::RegisterErrorCode<OSStatus, error>();
     }
   
   void ThrowOSStatus_Internal( OSStatus );
   
	inline void ThrowOSStatus( OSStatus err )
	{
		if ( err != noErr )
		{
			ThrowOSStatus_Internal( err );
		}
	}
   
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
										                public std::bad_alloc,
										                public DebuggingContext
     {
      public:
         ErrorCode() : OSStatus( memFullErr )  {}
         ErrorCode( const DebuggingContext& debugging ) : OSStatus( memFullErr ),
                                                          DebuggingContext( debugging )
         {}
         
         const char* what() const throw()  { return "OSStatus -108 (memFullErr)"; }
     };
	
	template <>
	class ErrorCode< Nitrogen::OSStatus, ::eofErr > : public Nitrogen::OSStatus,
	                                                  public io::end_of_input,
	                                                  public DebuggingContext
	{
		public:
			ErrorCode() : OSStatus( eofErr )  {}
			ErrorCode( const DebuggingContext& debugging ) : OSStatus( eofErr ),
                                                             DebuggingContext( debugging )
         {}
	};
	
	template <>
	class ErrorCode< Nitrogen::OSStatus, ::kOTNoDataErr > : public Nitrogen::OSStatus,
	                                                        public io::no_input_pending,
	                                                        public DebuggingContext
	{
		public:
			ErrorCode() : OSStatus( kOTNoDataErr )  {}
			ErrorCode( const DebuggingContext& debugging ) : OSStatus( kOTNoDataErr ),
                                                             DebuggingContext( debugging )
         {}
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
