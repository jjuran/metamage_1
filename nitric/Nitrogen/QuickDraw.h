// QuickDraw.h

#ifndef NITROGEN_QUICKDRAW_H
#define NITROGEN_QUICKDRAW_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __QUICKDRAW__
#include FRAMEWORK_HEADER(QD,QuickDraw.h)
#endif
#ifndef NITROGEN_PSEUDOREFERENCE_H
#include "Nitrogen/Pseudoreference.h"
#endif

namespace Nitrogen
  {
   using ::InitCursor;

   Point GetPortPenSize( CGrafPtr );
   using ::SetPortPenSize;
   
   class PortPenSize_Details
     {
      private:
         CGrafPtr port;
      
      public:
         typedef Point Value;
         typedef Value GetResult;
         typedef Value SetParameter;
         
         PortPenSize_Details( CGrafPtr thePort )     : port( thePort ) {}
         GetResult Get() const                       { return GetPortPenSize( port ); }
         void Set( SetParameter size ) const         { SetPortPenSize( port, size ); }
     };
   
   typedef Pseudoreference< PortPenSize_Details > PortPenSize;
  }

#endif
