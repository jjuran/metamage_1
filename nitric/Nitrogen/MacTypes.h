// MacTypes.h

#ifndef NITROGEN_MACTYPES_H
#define NITROGEN_MACTYPES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MacTypes__
#include FRAMEWORK_HEADER(CarbonCore,MacTypes.h)
#endif
#ifndef __SCRIPT__
#include FRAMEWORK_HEADER(CarbonCore,Script.h)
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
#ifndef NITROGEN_OVERLOADED_MATH_H
#include "Nitrogen/Overloaded_math.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_MAKE_H
#include "Nitrogen/Make.h"
#endif

#include <cstddef>
#include <string>
#include <cmath>

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
      return static_cast< Integral >( CStd::nearbyint( std::ldexp( in, fractionBits ) ) );
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
   typedef SelectorType< FourCharCodeTag, ::FourCharCode, '\?\?\?\?' > FourCharCode;
   
   class OSTypeTag {};
   typedef SelectorType< OSTypeTag, ::OSType, '\?\?\?\?' > OSType;
   
   class ResTypeTag {};
   typedef SelectorType< ResTypeTag, ::ResType, '\?\?\?\?' > ResType;

   class ResourceIDTag {};
   typedef IDType< ResourceIDTag, SInt16, 0 > ResourceID;
   typedef ResourceID ResID;
   
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
   
   using ::Point;

   template <>
   struct Maker< Point >
     {
      Point operator()( short v, short h ) const
        {
         Point result;
         result.v = v;
         result.h = h;
         return result;
        }
      
      Point operator()() const
        {
         return operator()( 0, 0 );
        }
     };
   
   using ::Rect;
   
   template <>
   struct Maker< Rect >
     {
      Rect operator()( short top, short left, short bottom, short right ) const
        {
         Rect result;
         result.top = top;
         result.left = left;
         result.bottom = bottom;
         result.right = right;
         return result;
        }
      
      Rect operator()() const
        {
         return operator()( 0, 0, 0, 0 );
        }
     };   
  }

#endif
