// OnlyOnce.h

#ifndef NITROGEN_ONLYONCE_H
#define NITROGEN_ONLYONCE_H

namespace Nitrogen
  {
   template < void (*function)() >
   void OnlyOnce()
     {
      static bool dummy = ( function(), false );
     }
  }

#endif
