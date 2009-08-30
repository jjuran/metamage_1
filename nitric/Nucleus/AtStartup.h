// Nucleus/AtStartup.h
// -------------------

// Part of the Nitrogen project.
//
// Written 2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_ATSTARTUP_H
#define NUCLEUS_ATSTARTUP_H

namespace Nucleus
  {
   namespace AtStartupPrivate
     {
      template < void (*function)() >
      bool RunAndReturnTrue()
        {
         return function(), true;
        }

      template < class Result, Result (*function)() >
      extern const Result valueAtStartup = function();
     }

   template < class Result, void (*function)() >
   void AtStartup()
     {
      return AtStartupPrivate::valueAtStartup< Result, function >;
     }
   
   template < void (*function)() >
   void AtStartup()
     {
      AtStartup< bool, AtStartupPrivate::RunAndReturnTrue<function> >();
     }
  }

#endif
