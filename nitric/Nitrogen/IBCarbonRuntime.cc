// Nitrogen/IBCarbonRuntime.cc
// ---------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2006 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/IBCarbonRuntime.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


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
	
	
   nucleus::owned< IBNibRef > CreateNibReference( CFStringRef inNibName )
     {
      IBNibRef result;
      ThrowOSStatus( ::CreateNibReference( inNibName, &result ) );
      return nucleus::owned<IBNibRef>::seize( result );
     }

   nucleus::owned< IBNibRef > CreateNibReferenceWithCFBundle( CFBundleRef inBundle,
                                                     CFStringRef inNibName )
     {
      IBNibRef result;
      ThrowOSStatus( ::CreateNibReferenceWithCFBundle( inBundle, inNibName, &result ) );
      return nucleus::owned<IBNibRef>::seize( result );
     }

   nucleus::owned< WindowRef > CreateWindowFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      WindowRef result;
      ThrowOSStatus( ::CreateWindowFromNib( inNibRef, inName, &result ) );
      return nucleus::owned<WindowRef>::seize( result );
     }

   nucleus::owned< MenuRef > CreateMenuFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      MenuRef result;
      ThrowOSStatus( ::CreateMenuFromNib( inNibRef, inName, &result ) );
      return nucleus::owned<MenuRef>::seize( result );
     }

#if 0
   nucleus::owned< Handle > CreateMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName )
     {
      Handle result;
      ThrowOSStatus( ::CreateMenuBarFromNib( inNibRef, inName, &result ) );
      return nucleus::owned<Handle>::seize( result );
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
