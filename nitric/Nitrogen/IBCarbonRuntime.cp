// Nitrogen/IBCarbonRuntime.cp
// ---------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2006 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/IBCarbonRuntime.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
  {
	
	InterfaceBuilderServicesErrorsRegistrationDependency::InterfaceBuilderServicesErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterInterfaceBuilderServicesErrors();
	
	
	class InterfaceBuilderServicesErrorsRegistration
	{
		public:
			InterfaceBuilderServicesErrorsRegistration()  { RegisterInterfaceBuilderServicesErrors(); }
	};
	
	static InterfaceBuilderServicesErrorsRegistration theRegistration;
	
	
   Nucleus::Owned< IBNibRef > CreateNibReference( CFStringRef inNibName )
     {
      IBNibRef result;
      ThrowOSStatus( ::CreateNibReference( inNibName, &result ) );
      return Nucleus::Owned<IBNibRef>::Seize( result );
     }

   Nucleus::Owned< IBNibRef > CreateNibReferenceWithCFBundle( CFBundleRef inBundle,
                                                     CFStringRef inNibName )
     {
      IBNibRef result;
      ThrowOSStatus( ::CreateNibReferenceWithCFBundle( inBundle, inNibName, &result ) );
      return Nucleus::Owned<IBNibRef>::Seize( result );
     }

   Nucleus::Owned< WindowRef > CreateWindowFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      WindowRef result;
      ThrowOSStatus( ::CreateWindowFromNib( inNibRef, inName, &result ) );
      return Nucleus::Owned<WindowRef>::Seize( result );
     }

   Nucleus::Owned< MenuRef > CreateMenuFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      MenuRef result;
      ThrowOSStatus( ::CreateMenuFromNib( inNibRef, inName, &result ) );
      return Nucleus::Owned<MenuRef>::Seize( result );
     }

#if 0
   Nucleus::Owned< Handle > CreateMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      Handle result;
      ThrowOSStatus( ::CreateMenuBarFromNib( inNibRef, inName, &result ) );
      return Nucleus::Owned<Handle>::Seize( result );
     }
#endif

   void SetMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
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
