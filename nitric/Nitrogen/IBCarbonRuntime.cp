// IBCarbonRuntime.cp

#ifndef NITROGEN_IBCARBONRUNTIME_H
#include "Nitrogen/IBCarbonRuntime.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   Owned< IBNibRef > CreateNibReference( CFStringRef inNibName )
     {
      OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      IBNibRef result;
      ThrowOSStatus( ::CreateNibReference( inNibName, &result ) );
      return Owned<IBNibRef>::Seize( result );
     }

   Owned< IBNibRef > CreateNibReferenceWithCFBundle( CFBundleRef inBundle,
                                                     CFStringRef inNibName )
     {
      OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      IBNibRef result;
      ThrowOSStatus( ::CreateNibReferenceWithCFBundle( inBundle, inNibName, &result ) );
      return Owned<IBNibRef>::Seize( result );
     }

   Owned< WindowRef > CreateWindowFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      WindowRef result;
      ThrowOSStatus( ::CreateWindowFromNib( inNibRef, inName, &result ) );
      return Owned<WindowRef>::Seize( result );
     }

   Owned< MenuRef > CreateMenuFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      MenuRef result;
      ThrowOSStatus( ::CreateMenuFromNib( inNibRef, inName, &result ) );
      return Owned<MenuRef>::Seize( result );
     }

#if 0
   Owned< Handle > CreateMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      Handle result;
      ThrowOSStatus( ::CreateMenuBarFromNib( inNibRef, inName, &result ) );
      return Owned<Handle>::Seize( result );
     }
#endif

   void SetMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      ThrowOSStatus( ::SetMenuBarFromNib( inNibRef, inName ) );
     }
   
   void RegisterInterfaceBuilderServicesErrors()
     {
      RegisterOSStatus< memFullErr                               >();
      RegisterOSStatus< kIBCarbonRuntimeCantFindNibFile          >();
      RegisterOSStatus< kIBCarbonRuntimeObjectNotOfRequestedType >();
      RegisterOSStatus< kIBCarbonRuntimeCantFindObject           >();
     }
  }
