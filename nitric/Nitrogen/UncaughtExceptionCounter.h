// UncaughtExceptionCounter.h

#ifndef NITROGEN_UNCAUGHTEXCEPTIONCOUNTER_H
#define NITROGEN_UNCAUGHTEXCEPTIONCOUNTER_H

#include <exception>

namespace Nitrogen
  {
   #ifdef UNCAUGHT_EXCEPTION_COUNT_EXISTS

      class UncaughtExceptionCounter
        {
         private:
            std::uncaught_exception_count_t oldExceptions;
         
         public:
            UncaughtExceptionCounter()
              : oldExceptions( std::uncaught_exception_count() )
              {}
            
            UncaughtExceptionCounter( const UncaughtExceptionCounter& )
              {}
            
            UncaughtExceptionCounter& operator=( const UncaughtExceptionCounter& )
              {
               return *this;
              }
            
            bool ExceptionHasBeenThrown() const
              {
               return std::uncaught_exception_count() > oldExceptions;
              }
        };
   
   #endif
  }

#endif
