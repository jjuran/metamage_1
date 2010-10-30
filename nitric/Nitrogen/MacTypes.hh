// Nitrogen/MacTypes.hh
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_MACTYPES_HH
#define NITROGEN_MACTYPES_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif
#ifndef NUCLEUS_OVERLOADEDMATH_HH
#include "nucleus/overloaded_math.hh"
#endif
#ifndef NUCLEUS_SCRIBE_HH
#include "nucleus/scribe.hh"
#endif

// Nitrogen
#ifndef MAC_RESOURCES_TYPES_RESTYPE_HH
#include "Mac/Resources/Types/ResType.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_LANGCODE_HH
#include "Mac/Script/Types/LangCode.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_REGIONCODE_HH
#include "Mac/Script/Types/RegionCode.hh"
#endif
#ifndef MAC_SCRIPT_TYPES_SCRIPTCODE_HH
#include "Mac/Script/Types/ScriptCode.hh"
#endif
#ifndef MAC_TOOLBOX_TYPES_OSTYPE_HH
#include "Mac/Toolbox/Types/OSType.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_SIZEOFVERSREC_HH
#include "Mac/Toolbox/Utilities/SizeOf_VersRec.hh"
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

#include <cstddef>
#include <cmath>


namespace Nitrogen
  {
	
	using ::VersRec;
	
	using Mac::SizeOf_VersRec;
	
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
   inline Floating FixedToFloatingPoint( Integral in )
     {
      return std::ldexp( static_cast<Floating>(in), -fractionBits );
     }

   template < class Integral, int fractionBits, class Floating >
   inline Integral FloatingToFixedPoint( Floating in )
     {
      return static_cast< Integral >( nucleus::c_std::nearbyint( std::ldexp( in, fractionBits ) ) );
     }
   
   inline double FixedToDouble( ::Fixed in )                   { return FixedToFloatingPoint< double,  16 >( in ); }
   inline ::Fixed DoubleToFixed( double in )                   { return FloatingToFixedPoint< ::Fixed, 16 >( in ); }

   inline double UnsignedFixedToDouble( ::UnsignedFixed in )   { return FixedToFloatingPoint< double,          16 >( in ); }
   inline ::UnsignedFixed DoubleToUnsignedFixed( double in )   { return FloatingToFixedPoint< ::UnsignedFixed, 16 >( in ); }

   inline double FractToDouble( ::Fract in )                   { return FixedToFloatingPoint< double,  30 >( in ); }
   inline ::Fract DoubleToFract( double in )                   { return FloatingToFixedPoint< ::Fract, 30 >( in ); }

   inline double ShortFixedToDouble( ::ShortFixed in )         { return FixedToFloatingPoint< double,       8 >( in ); }
   inline ::ShortFixed DoubleToShortFixed( double in )         { return FloatingToFixedPoint< ::ShortFixed, 8 >( in ); }
   
   struct FixedFlattener
     {
      typedef double Put_Parameter;
      
      template < class Putter >
      static void Put( Put_Parameter toPut, Putter put )
        {
         const Fixed fixed = DoubleToFixed( toPut );
         put( &fixed, &fixed + 1 );
        }
      
      typedef double Get_Result;
      
      template < class Getter >
      static Get_Result Get( Getter get )
        {
         Fixed fixed;
         get( &fixed, &fixed + 1 );
         return FixedToDouble( fixed );
        }
      
      typedef Put_Parameter Parameter;
      typedef Get_Result    Result;
      
      static const bool hasStaticSize = true;
      
      typedef ::Fixed Buffer;
     };
   
   
   using ::Float32;
   using ::Float64;
   using ::Float80;
   using ::Float96;
   
   typedef ::std::size_t  Size;
	
	enum OptionBits
	{
		kNilOptions = ::kNilOptions,
		
		kOptionBits_Max = nucleus::enumeration_traits< ::OptionBits >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( OptionBits )
	
	using Mac::ScriptCode;
	using Mac::smSystemScript;
	
	using Mac::LangCode;
	using Mac::langUnspecified;
	
	using Mac::RegionCode;
	using Mac::verUS;
	
	using Mac::OSType;
	using Mac::kUnknownType;
	
	using Mac::ResType;
	using Mac::kVersionResType;
	
	typedef bool Boolean;
	
	typedef nucleus::converting_POD_scribe< bool, ::Boolean > BooleanFlattener;
	
	enum Style
	{
		kStyle_Max = nucleus::enumeration_traits< ::Style >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( Style )

   using ::Point;
   using ::Rect;
  }

namespace nucleus
  {
   template <>
   struct maker< Point >
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

   template <>
   struct maker< Rect >
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
