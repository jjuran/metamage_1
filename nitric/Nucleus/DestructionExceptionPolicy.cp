// DestructionExceptionPolicy.cp

#ifndef NUCLEUS_DESTRUCTIONEXCEPTIONPOLICY_H
#include "Nucleus/DestructionExceptionPolicy.h"
#endif

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
