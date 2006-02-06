// IBCarbonRuntime.cp

#ifndef NITROGEN_IBCARBONRUNTIME_H
#include "Nitrogen/IBCarbonRuntime.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   Nucleus::Owned< IBNibRef > CreateNibReference( CFStringRef inNibName )
     {
      Nucleus::OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      IBNibRef result;
      ThrowOSStatus( ::CreateNibReference( inNibName, &result ) );
      return Nucleus::Owned<IBNibRef>::Seize( result );
     }

   Nucleus::Owned< IBNibRef > CreateNibReferenceWithCFBundle( CFBundleRef inBundle,
                                                     CFStringRef inNibName )
     {
      Nucleus::OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      IBNibRef result;
      ThrowOSStatus( ::CreateNibReferenceWithCFBundle( inBundle, inNibName, &result ) );
      return Nucleus::Owned<IBNibRef>::Seize( result );
     }

   Nucleus::Owned< WindowRef > CreateWindowFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      Nucleus::OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      WindowRef result;
      ThrowOSStatus( ::CreateWindowFromNib( inNibRef, inName, &result ) );
      return Nucleus::Owned<WindowRef>::Seize( result );
     }

   Nucleus::Owned< MenuRef > CreateMenuFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      Nucleus::OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      MenuRef result;
      ThrowOSStatus( ::CreateMenuFromNib( inNibRef, inName, &result ) );
      return Nucleus::Owned<MenuRef>::Seize( result );
     }

#if 0
   Nucleus::Owned< Handle > CreateMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      Nucleus::OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
      Handle result;
      ThrowOSStatus( ::CreateMenuBarFromNib( inNibRef, inName, &result ) );
      return Nucleus::Owned<Handle>::Seize( result );
     }
#endif

   void SetMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      Nucleus::OnlyOnce< RegisterInterfaceBuilderServicesErrors >();
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
