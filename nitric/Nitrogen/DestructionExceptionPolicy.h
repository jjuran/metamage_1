// DestructionExceptionPolicy.h

#ifndef NITROGEN_DESTRUCTIONEXCEPTIONPOLICY_H
#define NITROGEN_DESTRUCTIONEXCEPTIONPOLICY_H

#ifndef NITROGEN_UNCAUGHTEXCEPTIONCOUNTER_H
#include "Nitrogen/UncaughtExceptionCounter.h"
#endif
#ifndef NITROGEN_THEEXCEPTIONBEINGHANDLED_H
#include "Nitrogen/TheExceptionBeingHandled.h"
#endif


#ifndef NITROGEN_DESTRUCTION_EXCEPTION_POLICY
   #ifdef UNCAUGHT_EXCEPTION_COUNT_EXISTS
      #define NITROGEN_DESTRUCTION_EXCEPTION_POLICY DestructorThrowsWhenNoNewUncaughtExceptions
   #else
      #define NITROGEN_DESTRUCTION_EXCEPTION_POLICY DestructorNeverThrows
   #endif
#endif


namespace Nitrogen
  {
   typedef void (*DestructionExceptionHandler)( TheExceptionBeingHandled );
   
   DestructionExceptionHandler SetDestructionExceptionHandler( DestructionExceptionHandler );
   
   void HandleDestructionException( TheExceptionBeingHandled );
   
   
   
   typedef void (*DestructionExceptionWarning)( bool wouldThrow );
   
   DestructionExceptionWarning SetDestructionExceptionWarning( DestructionExceptionWarning );
   
   void WarnOfDestructionExceptionRisk( bool wouldThrow );
   
   #ifndef NITROGEN_WARN_OF_DESTRUCTION_EXCEPTION_RISKS
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
            Nitrogen::WarnOfDestructionExceptionRisk( true );
           }
     };

   class DestructorNeverThrows
     {
      public:
         void HandleDestructionException( TheExceptionBeingHandled exception ) const
           {
            Nitrogen::HandleDestructionException( exception );
           }
         
         void WarnOfDestructionExceptionRisk() const
           {
            Nitrogen::WarnOfDestructionExceptionRisk( false );
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
   
   typedef NITROGEN_DESTRUCTION_EXCEPTION_POLICY DefaultDestructionExceptionPolicy;
  }

#endif
