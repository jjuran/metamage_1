// AtStartup.h

#ifndef NITROGEN_ATSTARTUP_H
#define NITROGEN_ATSTARTUP_H

namespace Nitrogen
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
