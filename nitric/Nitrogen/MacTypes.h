// MacTypes.h

#ifndef NITROGEN_MACTYPES_H
#define NITROGEN_MACTYPES_H

#ifndef __MacTypes__
#include <MacTypes.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif

#include <cstddef>
#include <string>

namespace Nitrogen
  {
   using ::UInt8;
   using ::SInt8;
   using ::UInt16;
   using ::SInt16;
   using ::UInt32;
   using ::SInt32;

   typedef long long                wide;
   typedef unsigned long long       UnsignedWide;


   // Nitrogen uses floating point types in preference to fixed-point types.
   template < class Floating, int fractionBits, class Integral >
   Floating FixedToFloatingPoint( Integral in )
     {
      return std::ldexp( static_cast<Floating>(in), -fractionBits );
     }

   template < class Integral, int fractionBits, class Floating >
   Integral FloatingToFixedPoint( Floating in )
     {
      return static_cast< Integral >( std::nearbyint( std::ldexp( in, fractionBits ) ) );
     }
   
   inline double FixedToDouble( ::Fixed in )                   { return FixedToFloatingPoint< double,  16 >( in ); }
   inline ::Fixed DoubleToFixed( double in )                   { return FloatingToFixedPoint< ::Fixed, 16 >( in ); }

   inline double UnsignedFixedToDouble( ::UnsignedFixed in )   { return FixedToFloatingPoint< double,          16 >( in ); }
   inline ::UnsignedFixed DoubleToUnsignedFixed( double in )   { return FloatingToFixedPoint< ::UnsignedFixed, 16 >( in ); }

   inline double FractToDouble( ::Fract in )                   { return FixedToFloatingPoint< double,  30 >( in ); }
   inline ::Fract DoubleToFract( double in )                   { return FloatingToFixedPoint< ::Fract, 30 >( in ); }

   inline double ShortFixedToDouble( ::ShortFixed in )         { return FixedToFloatingPoint< double,       8 >( in ); }
   inline ::ShortFixed DoubleToShortFixed( double in )         { return FloatingToFixedPoint< ::ShortFixed, 8 >( in ); }
   
   
   using ::Float32;
   using ::Float64;
   using ::Float80;
   using ::Float96;
   
   typedef ::std::size_t  Size;

   class OptionBitsTag {};
   typedef FlagType< OptionBitsTag, ::OptionBits, 0 > OptionBits;
   
   class ScriptCodeTag {};
   typedef SelectorType< ScriptCodeTag, ::ScriptCode, ::smSystemScript > ScriptCode;

   class LangCodeTag {};
   typedef SelectorType< LangCodeTag,   ::LangCode,   ::langUnspecified > LangCode;

   class RegionCodeTag {};
   typedef SelectorType< RegionCodeTag, ::RegionCode, ::verUS > RegionCode;
   
   class FourCharCodeTag {};
   typedef SelectorType< FourCharCodeTag, ::FourCharCode, '????' > FourCharCode;
   
   class OSTypeTag {};
   typedef SelectorType< OSTypeTag, ::OSType, '????' > OSType;
   
   class ResTypeTag {};
   typedef SelectorType< ResTypeTag, ::ResType, '????' > ResType;
   
   typedef bool Boolean;

   class StyleTag {};
   typedef FlagType< StyleTag, ::Style > Style;
      //Style may need a conversion to short...

   using ::UnicodeScalarValue;
   using ::UTF32Char;
   using ::UniChar;
   using ::UTF16Char;
   using ::UTF8Char;

   typedef std::basic_string< UTF32Char > UTF32String;
   typedef std::basic_string< UTF16Char > UTF16String;
   typedef std::basic_string< UTF8Char > UTF8String;
   typedef std::basic_string< UniChar > UniString;
  }

#endif
