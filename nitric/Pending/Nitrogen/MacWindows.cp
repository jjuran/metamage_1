// MacWindows.cp

#ifndef NITROGEN_MACWINDOWS_H
#include "Nitrogen/MacWindows.h"
#endif

namespace Nitrogen
  {
   WindowAttributes GetWindowAttributes( WindowRef window )
     {
      OnlyOnce< RegisterWindowManagerErrors >();
      ::WindowAttributes result;
      ThrowOSStatus( ::GetWindowAttributes( window, &result ) );
      return result;
     }

   void ChangeWindowAttributes ( WindowRef        window,
                                 WindowAttributes setTheseAttributes,
                                 WindowAttributes clearTheseAttributes )
     {
      OnlyOnce< RegisterWindowManagerErrors >();
      ThrowOSStatus( ::ChangeWindowAttributes( window, setTheseAttributes, clearTheseAttributes ) );
     }
   
   void RegisterWindowManagerErrors()
     {
      OnlyOnce< RegisterOSStatus< memFullErr > >();
     }
  }
