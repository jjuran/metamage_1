// Nitrogen/IBCarbonRuntime.hh
// ---------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2006 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_IBCARBONRUNTIME_HH
#define NITROGEN_IBCARBONRUNTIME_HH

#ifndef __IBCARBONRUNTIME__
#include <IBCarbonRuntime.h>
#endif
#ifndef NITROGEN_MACWINDOWS_HH
#include "Nitrogen/MacWindows.hh"
#endif
#ifndef NITROGEN_MENUS_HH
#include "Nitrogen/Menus.hh"
#endif

namespace Nitrogen
  {
	
	class InterfaceBuilderServicesErrorsRegistrationDependency
	{
		public:
			InterfaceBuilderServicesErrorsRegistrationDependency();
	};
	
   using ::IBNibRef;
   
  }

namespace nucleus
  {
   template <> struct disposer< IBNibRef >: public std::unary_function< IBNibRef, void >
     {
      void operator()( IBNibRef n ) const
        {
         ::DisposeNibReference( n );
        }
     };
  }

namespace Nitrogen
  {
   
   nucleus::owned< IBNibRef > CreateNibReference( CFStringRef inNibName );
   nucleus::owned< IBNibRef > CreateNibReferenceWithCFBundle( CFBundleRef inBundle, CFStringRef inNibName );

   inline void DisposeNibReference( nucleus::owned<IBNibRef> /*toDispose*/ )
     {
     }

   nucleus::owned< WindowRef > CreateWindowFromNib ( IBNibRef inNibRef, CFStringRef inName );
   nucleus::owned< MenuRef >   CreateMenuFromNib   ( IBNibRef inNibRef, CFStringRef inName );
#if 0
   nucleus::owned< Handle >    CreateMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName );
#endif
   void SetMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName );
   
  }

#endif
