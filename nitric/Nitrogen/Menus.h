// Menus.h

#ifndef NITROGEN_MENUS_H
#define NITROGEN_MENUS_H

#ifndef __MENUS__
#include <Menus.h>
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

namespace Nitrogen
  {
   using ::MenuRef;
   
   template <> struct Disposer< MenuRef >: public std::unary_function< MenuRef, void >
     {
      void operator()( MenuRef m ) const
        {
         if ( m != 0 )
            ::DisposeMenu( m );
        }
     };

   using ::EnableMenuItem;
   using ::DisableMenuItem;
   using ::SetItemMark;
  }

#endif
