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
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif

namespace Nitrogen
  {
   using ::CFBooleanRef;
   
   template <> struct CFType_Traits< CFBooleanRef >: Basic_CFType_Traits< CFBooleanRef, ::CFBooleanGetTypeID > {};
  }

namespace Nucleus
  {
   template <> struct Disposer_Traits< Nitrogen::CFBooleanRef >: Disposer_Traits<Nitrogen::CFTypeRef>  {};
  }

namespace Nitrogen
  {
   inline bool CFBooleanGetValue( const CFBooleanRef b ) { return ::CFBooleanGetValue( b ); }
   inline void CFShow( const CFBooleanRef b )            { ::CFShow( b ); }
  }
  
namespace Nucleus
  {
   template <>
   struct Converter< bool, Nitrogen::CFBooleanRef >: public std::unary_function< Nitrogen::CFBooleanRef, bool >
     {
      bool operator()( const Nitrogen::CFBooleanRef& in ) const
        {
         return Nitrogen::CFBooleanGetValue( in );
        }
     };

   template <>
   struct Converter< Nitrogen::CFBooleanRef, bool >: public std::unary_function< bool, Nitrogen::CFBooleanRef >
     {
      Nitrogen::CFBooleanRef operator()( const bool& in ) const
        {
         return in ? kCFBooleanTrue : kCFBooleanFalse;
        }
     };
   
   template <>
   struct Converter< Nucleus::Owned<Nitrogen::CFBooleanRef>, bool >: public std::unary_function< bool, Nucleus::Owned<Nitrogen::CFBooleanRef> >
     {
      Nucleus::Owned<Nitrogen::CFBooleanRef> operator()( const bool& in ) const
        {
         return Nitrogen::CFRetain( Convert<Nitrogen::CFBooleanRef>( in ) );
        }
     };
  }

#endif
