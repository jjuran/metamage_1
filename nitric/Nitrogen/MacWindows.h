// MacWindows.h

#ifndef NITROGEN_MACWINDOWS_H
#define NITROGEN_MACWINDOWS_H

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

namespace Nitrogen
  {
   using ::WindowRef;
   
   template <> struct Disposer< WindowRef >: public std::unary_function< WindowRef, void >
     {
      void operator()( WindowRef w ) const
        {
         ::DisposeWindow( w );
        }
     };
     
   using ::ShowWindow;
  }

#endif
