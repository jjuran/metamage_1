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
   inline void SetPortPenSize( CGrafPtr port, Point penSize )     { ::SetPortPenSize( port, penSize ); }
      // Just a pass-through, but making sure it's not a pascal function, for Pseudoreference.
   
   typedef Pseudoreference< CGrafPtr,
                            Point,
                            Point (*)( CGrafPtr ),        GetPortPenSize,
                            void  (*)( CGrafPtr, Point ), SetPortPenSize > PortPenSize;
  }

#endif
