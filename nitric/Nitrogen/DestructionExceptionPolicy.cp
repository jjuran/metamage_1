// DestructionExceptionPolicy.cp

#ifndef NITROGEN_DESTRUCTIONEXCEPTIONPOLICY_H
#include "Nitrogen/DestructionExceptionPolicy.h"
#endif

namespace Nitrogen
  {
   static void DefaultDestructionExceptionHandler( const TheExceptionBeingHandled& )
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
   
   void HandleDestructionException( const TheExceptionBeingHandled& exception )
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
   
   #ifdef NITROGEN_WARN_OF_DESTRUCTION_EXCEPTION_RISKS
      void WarnOfDestructionExceptionRisk( bool wouldThrow )
        {
         CurrentDestructionExceptionWarning()( wouldThrow  );
        }
   #endif

   

   #ifdef UNCAUGHT_EXCEPTION_EXISTS
      void DestructorThrowsWhenNoUncaughtExceptions::HandleDestructionException( const TheExceptionBeingHandled& exception ) const
        {
         if ( !std::uncaught_exception() )
            throw;
         
          Nitrogen::HandleDestructionException( exception );
        }

      void DestructorThrowsWhenNoUncaughtExceptions::WarnOfDestructionExceptionRisk() const
        {
         Nitrogen::WarnOfDestructionExceptionRisk( !std::uncaught_exception() );
        }
   #endif
   
   #ifdef UNCAUGHT_EXCEPTION_COUNT_EXISTS
      void DestructorThrowsWhenNoNewUncaughtExceptions::HandleDestructionException( const TheExceptionBeingHandled& exception ) const
        {
         if ( !exceptionCounter.ExceptionHasBeenThrown() )
            throw;

          Nitrogen::HandleDestructionException( exception );
        }

      void DestructorThrowsWhenNoNewUncaughtExceptions::WarnOfDestructionExceptionRisk() const
        {
         Nitrogen::WarnOfDestructionExceptionRisk( !exceptionCounter.ExceptionHasBeenThrown() );
        }
   #endif
  }
