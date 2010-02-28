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
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
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

namespace Nucleus
  {
   template <> struct Disposer< Nitrogen::IBNibRef >: public std::unary_function< Nitrogen::IBNibRef, void >
     {
      void operator()( Nitrogen::IBNibRef n ) const
        {
         ::DisposeNibReference( n );
        }
     };
  }

namespace Nitrogen
  {
   
   Nucleus::Owned< IBNibRef > CreateNibReference( CFStringRef inNibName );
   Nucleus::Owned< IBNibRef > CreateNibReferenceWithCFBundle( CFBundleRef inBundle, CFStringRef inNibName );

   inline void DisposeNibReference( Nucleus::Owned<IBNibRef> /*toDispose*/ )
     {
     }

   Nucleus::Owned< WindowRef > CreateWindowFromNib ( IBNibRef inNibRef, CFStringRef inName );
   Nucleus::Owned< MenuRef >   CreateMenuFromNib   ( IBNibRef inNibRef, CFStringRef inName );
#if 0
   Nucleus::Owned< Handle >    CreateMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName );
#endif
   void SetMenuBarFromNib( IBNibRef inNibRef, CFStringRef inName );
   
  }

#endif
