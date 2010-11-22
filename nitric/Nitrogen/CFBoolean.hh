// Nitrogen/CFBoolean.hh
// ---------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFBOOLEAN_HH
#define NITROGEN_CFBOOLEAN_HH

#ifndef __CFNUMBER__
#include <CFNumber.h>
#endif
#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif
#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif

namespace Nitrogen
  {
   using ::CFBooleanRef;
   
   template <> struct CFType_Traits< CFBooleanRef >: Basic_CFType_Traits< CFBooleanRef, ::CFBooleanGetTypeID > {};
  }

namespace nucleus
  {
   template <> struct disposer_class< CFBooleanRef >: disposer_class<Nitrogen::CFTypeRef>  {};
  }

namespace Nitrogen
  {
   inline bool CFBooleanGetValue( const CFBooleanRef b ) { return ::CFBooleanGetValue( b ); }
   inline void CFShow( const CFBooleanRef b )            { ::CFShow( b ); }
  }
  
namespace nucleus
  {
   template <>
   struct converter< bool, CFBooleanRef >
     {
      typedef CFBooleanRef  argument_type;
      typedef bool          result_type;
      
      bool operator()( const CFBooleanRef& in ) const
        {
         return Nitrogen::CFBooleanGetValue( in );
        }
     };

   template <>
   struct converter< CFBooleanRef, bool >
     {
      typedef bool          argument_type;
      typedef CFBooleanRef  result_type;
      
      CFBooleanRef operator()( const bool& in ) const
        {
         return in ? kCFBooleanTrue : kCFBooleanFalse;
        }
     };
   
   template <>
   struct converter< nucleus::owned<CFBooleanRef>, bool >
     {
      typedef bool                            argument_type;
      typedef nucleus::owned< CFBooleanRef >  result_type;
      
      nucleus::owned<CFBooleanRef> operator()( const bool& in ) const
        {
         return Nitrogen::CFRetain( convert<CFBooleanRef>( in ) );
        }
     };
  }

#endif
