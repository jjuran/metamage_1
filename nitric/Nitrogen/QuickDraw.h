// QuickDraw.h

#ifndef NITROGEN_QUICKDRAW_H
#define NITROGEN_QUICKDRAW_H

#ifndef __QUICKDRAW__
#include <QuickDraw.h>
#endif
#ifndef NITROGEN_PSEUDOREFERENCE_H
#include "Nitrogen/Pseudoreference.h"
#endif

namespace Nitrogen
  {
   using ::InitCursor;

   Point GetPortPenSize( CGrafPtr );
   using ::SetPortPenSize;
   
   typedef Pseudoreference< CGrafPtr,
                            Point,
                            Point (*)( CGrafPtr ),               GetPortPenSize,
                            pascal void  (*)( CGrafPtr, Point ), SetPortPenSize > PortPenSize;
  }

#endif
