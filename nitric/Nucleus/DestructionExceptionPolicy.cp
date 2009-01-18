// Nucleus/DestructionExceptionPolicy.cp
// -------------------------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nucleus/DestructionExceptionPolicy.h"


namespace Nucleus
  {
   static void DefaultDestructionExceptionHandler( TheExceptionBeingHandled )
     {
     }
   
   static DestructionExceptionHandler& CurrentDestructionExceptionHandler()
     {
      static DestructionExceptionHandler currentDestructionExceptionHandler = DefaultDestructionExceptionHandler;
      return currentDestructionExceptionHandler;
     }
   
   DestructionExceptionHandler SetDestructionExceptionHandler( DestructionExceptionHandler newHandler )
     {
      DestructionExceptionHandler oldHandler = CurrentDestructionExceptionHandler();
      CurrentDestructionExceptionHandler() = newHandler;
      return oldHandler;
     }
   
   void HandleDestructionException( TheExceptionBeingHandled exception )
     {
      CurrentDestructionExceptionHandler()( exception );
     }
   


   static void DefaultDestructionExceptionWarning( bool )
     {
     }
   
   static DestructionExceptionWarning& CurrentDestructionExceptionWarning()
     {
      static DestructionExceptionWarning currentDestructionExceptionWarning = DefaultDestructionExceptionWarning;
      return currentDestructionExceptionWarning;
     }
   
   DestructionExceptionWarning SetDestructionExceptionWarning( DestructionExceptionWarning newWarning )
     {
      DestructionExceptionWarning oldWarning = CurrentDestructionExceptionWarning();
      CurrentDestructionExceptionWarning() = newWarning;
      return oldWarning;
     }
   
   #ifdef NUCLEUS_WARN_OF_DESTRUCTION_EXCEPTION_RISKS
      void WarnOfDestructionExceptionRisk( bool wouldThrow )
        {
         CurrentDestructionExceptionWarning()( wouldThrow  );
        }
   #endif

   

   #ifdef UNCAUGHT_EXCEPTION_EXISTS
      void DestructorThrowsWhenNoUncaughtExceptions::HandleDestructionException( TheExceptionBeingHandled exception ) const
        {
         if ( !std::uncaught_exception() )
            throw;
         
          Nucleus::HandleDestructionException( exception );
        }

      void DestructorThrowsWhenNoUncaughtExceptions::WarnOfDestructionExceptionRisk() const
        {
         Nucleus::WarnOfDestructionExceptionRisk( !std::uncaught_exception() );
        }
   #endif
   
   #ifdef UNCAUGHT_EXCEPTION_COUNT_EXISTS
      void DestructorThrowsWhenNoNewUncaughtExceptions::HandleDestructionException( TheExceptionBeingHandled exception ) const
        {
         if ( !exceptionCounter.ExceptionHasBeenThrown() )
            throw;

          Nucleus::HandleDestructionException( exception );
        }

      void DestructorThrowsWhenNoNewUncaughtExceptions::WarnOfDestructionExceptionRisk() const
        {
         Nucleus::WarnOfDestructionExceptionRisk( !exceptionCounter.ExceptionHasBeenThrown() );
        }
   #endif
  }
