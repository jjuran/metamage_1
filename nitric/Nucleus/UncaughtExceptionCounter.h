// Nucleus/UncaughtExceptionCounter.h
// ----------------------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_UNCAUGHTEXCEPTIONCOUNTER_H
#define NUCLEUS_UNCAUGHTEXCEPTIONCOUNTER_H

#include <exception>

namespace Nucleus
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
