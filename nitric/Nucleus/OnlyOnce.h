// OnlyOnce.h

#ifndef NUCLEUS_ONLYONCE_H
#define NUCLEUS_ONLYONCE_H

namespace Nucleus
  {
   template < void (*function)() >
   void OnlyOnce()
     {
      static bool dummy = ( function(), false );
      (void)dummy;
     }
  }

#endif
