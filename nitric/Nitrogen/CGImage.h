// CGImage.h

#ifndef NITROGEN_CGIMAGE_H
#define NITROGEN_CGIMAGE_H

#ifndef __CGIMAGE__
#include <CGImage.h>
#endif
#ifndef NITROGEN_CGBASE_H
#include "Nitrogen/CGBase.h"
#endif
#ifndef NITROGEN_CGCOLORSPACE_H
#include "Nitrogen/CGColorSpace.h"
#endif
#ifndef NITROGEN_CGDATAPROVIDER_H
#include "Nitrogen/CGDataProvider.h"
#endif

namespace Nitrogen
  {
   using ::CGImageRef;
   
   template <> struct Disposer< CGImageRef >: public std::unary_function< CGImageRef, void >
     {
      void operator()( CGImageRef image ) const
        {
         ::CGImageRelease( image );
        }
     };
   
   inline Owned<CGImageRef> CGImageRetain( CGImageRef image )
     {
      return Owned<CGImageRef>::Seize( ::CGImageRetain( image ) );
     }
   
   inline void CGImageRelease( Owned<CGImageRef> )
     {}
  }

#endif
