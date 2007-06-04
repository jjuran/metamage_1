// Nucleus/DestructionExceptionPolicy.h
// ------------------------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_DESTRUCTIONEXCEPTIONPOLICY_H
#define NUCLEUS_DESTRUCTIONEXCEPTIONPOLICY_H

#ifndef NUCLEUS_UNCAUGHTEXCEPTIONCOUNTER_H
#include "Nucleus/UncaughtExceptionCounter.h"
#endif
#ifndef NUCLEUS_THEEXCEPTIONBEINGHANDLED_H
#include "Nucleus/TheExceptionBeingHandled.h"
#endif


#ifndef NUCLEUS_DESTRUCTION_EXCEPTION_POLICY
   #ifdef UNCAUGHT_EXCEPTION_COUNT_EXISTS
      #define NUCLEUS_DESTRUCTION_EXCEPTION_POLICY DestructorThrowsWhenNoNewUncaughtExceptions
   #else
      #define NUCLEUS_DESTRUCTION_EXCEPTION_POLICY DestructorNeverThrows
   #endif
#endif


namespace Nucleus
  {
   typedef void (*DestructionExceptionHandler)( TheExceptionBeingHandled );
   
   DestructionExceptionHandler SetDestructionExceptionHandler( DestructionExceptionHandler );
   
   void HandleDestructionException( TheExceptionBeingHandled );
   
   
   
   typedef void (*DestructionExceptionWarning)( bool wouldThrow );
   
   DestructionExceptionWarning SetDestructionExceptionWarning( DestructionExceptionWarning );
   
   void WarnOfDestructionExceptionRisk( bool wouldThrow );
   
   #ifndef NUCLEUS_WARN_OF_DESTRUCTION_EXCEPTION_RISKS
      inline void WarnOfDestructionExceptionRisk( bool )
        {
        }
   #endif
   
   
   
   class DestructorAlwaysThrows
     {
      public:
         void HandleDestructionException( TheExceptionBeingHandled ) const
           {
            throw;
           }
         
         void WarnOfDestructionExceptionRisk() const
           {
            Nucleus::WarnOfDestructionExceptionRisk( true );
           }
     };

   class DestructorNeverThrows
     {
      public:
         void HandleDestructionException( TheExceptionBeingHandled exception ) const
           {
            Nucleus::HandleDestructionException( exception );
           }
         
         void WarnOfDestructionExceptionRisk() const
           {
            Nucleus::WarnOfDestructionExceptionRisk( false );
           }
     };

   #ifdef UNCAUGHT_EXCEPTION_EXISTS
      class DestructorThrowsWhenNoUncaughtExceptions
        {
         public:
            void HandleDestructionException( TheExceptionBeingHandled ) const;
         
            void WarnOfDestructionExceptionRisk() const;
        };
   #endif
   
   #ifdef UNCAUGHT_EXCEPTION_COUNT_EXISTS
      class DestructorThrowsWhenNoNewUncaughtExceptions
        {
         private:
            UncaughtExceptionCounter exceptionCounter;
         
         public:
            void HandleDestructionException( TheExceptionBeingHandled ) const;

            void WarnOfDestructionExceptionRisk() const;
        };
   #endif
   
   typedef NUCLEUS_DESTRUCTION_EXCEPTION_POLICY DefaultDestructionExceptionPolicy;
  }

#endif
