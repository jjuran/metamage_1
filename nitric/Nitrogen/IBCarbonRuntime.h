// IBCarbonRuntime.h

#ifndef NITROGEN_IBCARBONRUNTIME_H
#define NITROGEN_IBCARBONRUNTIME_H

#ifndef __IBCARBONRUNTIME__
#include <IBCarbonRuntime.h>
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_MACWINDOWS_H
#include "Nitrogen/MacWindows.h"
#endif
#ifndef NITROGEN_MENUS_H
#include "Nitrogen/Menus.h"
#endif

namespace Nitrogen
  {
   using ::IBNibRef;
   
   template <> struct Disposer< IBNibRef >: public std::unary_function< IBNibRef, void >
     {
      void operator()( IBNibRef n ) const
        {
         if ( n != 0 )
            ::DisposeNibReference( n );
        }
     };
   
   Owned< IBNibRef > CreateNibReference( CFStringRef inNibName );
   Owned< IBNibRef > CreateNibReferenceWithCFBundle( CFBundleRef inBundle, CFStringRef inNibName );

   inline void DisposeNibReference( Owned<IBNibRef> /*toDispose*/ )
     {
     }

   Owned< WindowRef > CreateWindowFromNib ( IBNibRef inNibRef, CFStringRef inName );
   Owned< MenuRef >   CreateMenuFromNib   ( IBNibRef inNibRef, CFStringRef inName );
#if 0
   Owned< Handle >    CreateMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName );
#endif
   void SetMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName );
   
   void RegisterInterfaceBuilderServicesErrors();
  }

#endif
