/*
	Mac/Toolbox/Types/Fixed.hh
	--------------------------
*/

#ifndef MAC_TOOLBOX_TYPES_FIXED_HH
#define MAC_TOOLBOX_TYPES_FIXED_HH

// Standard C/C++
#include <cmath>

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#ifndef NUCLEUS_OVERLOADEDMATH_HH
#include "nucleus/overloaded_math.hh"
#endif


namespace Mac
{
	
	// Nitrogen uses floating point types in preference to fixed-point types.
	
	
	template < class Floating, int fractionBits, class Integral >
	inline Floating FixedToFloatingPoint( Integral in )
	{
		return std::ldexp( static_cast< Floating >( in ), -fractionBits );
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
	
	
	struct Fixed_scribe
	{
		typedef double argument_type;
		typedef double result_type;
		
		template < class Putter >
		static void Put( argument_type param, Putter putter )
		{
			const Fixed fixed = DoubleToFixed( param );
			
			putter( &fixed, &fixed + 1 );
		}
		
		template < class Getter >
		static result_type Get( Getter getter )
		{
			Fixed fixed;
			
			getter( &fixed, &fixed + 1 );
			
			return FixedToDouble( fixed );
		}
		
		static const bool hasStaticSize = true;
		
		typedef ::Fixed Buffer;
		
		static const std::size_t static_size = sizeof (::Fixed);
	};
	
}

#endif

