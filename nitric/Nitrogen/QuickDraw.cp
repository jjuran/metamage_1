// QuickDraw.cp

#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

namespace Nitrogen
  {
   Point GetPortPenSize( CGrafPtr port )
     {
      Point result;
      ::GetPortPenSize( port, &result );
      return result;
     }
  }