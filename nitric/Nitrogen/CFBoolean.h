// CFBoolean.h

#ifndef NITROGEN_CFBOOLEAN_H
#define NITROGEN_CFBOOLEAN_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFNUMBER__
#include FRAMEWORK_HEADER(CoreFoundation,CFNumber.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
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
   template <> struct OwnedDefaults< Nitrogen::CFBooleanRef >: OwnedDefaults<Nitrogen::CFTypeRef>  {};
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
