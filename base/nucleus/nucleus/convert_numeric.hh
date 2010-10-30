// nucleus/convert.hh
// ------------------

// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_CONVERTNUMERIC_HH
#define NUCLEUS_CONVERTNUMERIC_HH

// nucleus
#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif
#ifndef NUCLEUS_OVERLOADEDMATH_HH
#include "nucleus/overloaded_math.hh"
#endif

// Standard C++
#include <limits>


namespace nucleus
{
	
	struct numeric_conversion_range_error                                      {};
	struct numeric_conversion_overflow_error : numeric_conversion_range_error  {};
	struct numeric_conversion_NaN_error      : numeric_conversion_range_error  {};
	
	template < class Output,
	           class Input,
	           bool radicesAreEqual = ( std::numeric_limits< Input >::radix == std::numeric_limits< Output >::radix ) >
	struct numeric_converter;
		/* I don't know how to safely do bounds checking when the radices aren't equal. --LL */
	
	template < class Output, class Input >
	struct numeric_converter< Output, Input, true >
	{
		typedef Input   argument_type;
		typedef Output  result_type;
		
		Output operator()( const Input& input )
		{
			const Input in = ( !std::numeric_limits< Input >::is_integer && std::numeric_limits< Output >::is_integer )
			                 ? static_cast< Input >( c_std::nearbyint( input ) )
			                 : input;
			
			static const bool inputHasNaN =     std::numeric_limits< Input >::has_quiet_NaN
			                                 || std::numeric_limits< Input >::has_signaling_NaN;
			
			static const bool outputHasNaN =    std::numeric_limits< Output >::has_quiet_NaN
			                                 || std::numeric_limits< Output >::has_signaling_NaN;
			
			if ( inputHasNaN && !outputHasNaN && in != in )
			{
				throw numeric_conversion_NaN_error();
			}
			
			// We'll assume that inexact types for which this is used are floating-point.
			
			static const int inputSize = std::numeric_limits< Input >::is_exact
			                              ? std::numeric_limits< Input >::digits
			                              : std::numeric_limits< Input >::max_exponent;
			
			static const int outputSize = std::numeric_limits< Output >::is_exact
			                               ? std::numeric_limits< Output >::digits
			                               : std::numeric_limits< Output >::max_exponent;
			
			static const bool shrinking =     std::numeric_limits< Output >::is_bounded
			                              && !std::numeric_limits< Output >::has_infinity
			                              && ( !std::numeric_limits< Input >::is_bounded || inputSize > outputSize );
			
			if ( shrinking )
			{
				if ( in > static_cast< Input >( std::numeric_limits< Output >::max() ) )
				{
					throw numeric_conversion_overflow_error();
				}
				
				if ( std::numeric_limits< Input >::is_signed )
				{
					if ( std::numeric_limits< Output >::is_signed )
					{
						const Output outputMin = std::numeric_limits< Output >::is_exact
						                           ? std::numeric_limits< Output >::min()
						                           : static_cast< Output >( -std::numeric_limits< Output >::max() );
						
						if ( in < static_cast< Input >( outputMin ) )
						{
							throw numeric_conversion_overflow_error();
						}
					}
					else if ( in < static_cast< Input >( 0 ) )
					{
						throw numeric_conversion_overflow_error();
					}
				}
				
				if ( std::numeric_limits< Input >::is_signed && in < static_cast< Input >( std::numeric_limits< Output >::min() ) )
				{
					throw numeric_conversion_overflow_error();
				}
			}
			else
			{
				static const bool signedToUnsigned =    std::numeric_limits< Input >::is_signed
				                                    && !std::numeric_limits< Output >::is_signed;
				
				if ( signedToUnsigned && in < static_cast< Input >( 0 ) )
				{
					throw numeric_conversion_overflow_error();
				}
			}
			
			return static_cast< Output >( in );
		}
	};
	
	template<> struct converter<          char,        signed char      > : numeric_converter<          char,        signed char      >  {};
	template<> struct converter<          char,        signed short     > : numeric_converter<          char,        signed short     >  {};
	template<> struct converter<          char,        signed int       > : numeric_converter<          char,        signed int       >  {};
	template<> struct converter<          char,        signed long      > : numeric_converter<          char,        signed long      >  {};
	template<> struct converter<          char,        signed long long > : numeric_converter<          char,        signed long long >  {};
	template<> struct converter<          char,      unsigned char      > : numeric_converter<          char,      unsigned char      >  {};
	template<> struct converter<          char,      unsigned short     > : numeric_converter<          char,      unsigned short     >  {};
	template<> struct converter<          char,      unsigned int       > : numeric_converter<          char,      unsigned int       >  {};
	template<> struct converter<          char,      unsigned long      > : numeric_converter<          char,      unsigned long      >  {};
	template<> struct converter<          char,      unsigned long long > : numeric_converter<          char,      unsigned long long >  {};
	template<> struct converter<          char,               float     > : numeric_converter<          char,               float     >  {};
	template<> struct converter<          char,               double    > : numeric_converter<          char,               double    >  {};
	template<> struct converter<          char,          long double    > : numeric_converter<          char,          long double    >  {};
	
	template<> struct converter<   signed char,               char      > : numeric_converter<   signed char,               char      >  {};
	template<> struct converter<   signed char,        signed short     > : numeric_converter<   signed char,        signed short     >  {};
	template<> struct converter<   signed char,        signed int       > : numeric_converter<   signed char,        signed int       >  {};
	template<> struct converter<   signed char,        signed long      > : numeric_converter<   signed char,        signed long      >  {};
	template<> struct converter<   signed char,        signed long long > : numeric_converter<   signed char,        signed long long >  {};
	template<> struct converter<   signed char,      unsigned char      > : numeric_converter<   signed char,      unsigned char      >  {};
	template<> struct converter<   signed char,      unsigned short     > : numeric_converter<   signed char,      unsigned short     >  {};
	template<> struct converter<   signed char,      unsigned int       > : numeric_converter<   signed char,      unsigned int       >  {};
	template<> struct converter<   signed char,      unsigned long      > : numeric_converter<   signed char,      unsigned long      >  {};
	template<> struct converter<   signed char,      unsigned long long > : numeric_converter<   signed char,      unsigned long long >  {};
	template<> struct converter<   signed char,               float     > : numeric_converter<   signed char,               float     >  {};
	template<> struct converter<   signed char,               double    > : numeric_converter<   signed char,               double    >  {};
	template<> struct converter<   signed char,          long double    > : numeric_converter<   signed char,          long double    >  {};
	
	template<> struct converter< unsigned char,               char      > : numeric_converter< unsigned char,               char      >  {};
	template<> struct converter< unsigned char,        signed char      > : numeric_converter< unsigned char,        signed char      >  {};
	template<> struct converter< unsigned char,        signed short     > : numeric_converter< unsigned char,        signed short     >  {};
	template<> struct converter< unsigned char,        signed int       > : numeric_converter< unsigned char,        signed int       >  {};
	template<> struct converter< unsigned char,        signed long      > : numeric_converter< unsigned char,        signed long      >  {};
	template<> struct converter< unsigned char,        signed long long > : numeric_converter< unsigned char,        signed long long >  {};
	template<> struct converter< unsigned char,      unsigned short     > : numeric_converter< unsigned char,      unsigned short     >  {};
	template<> struct converter< unsigned char,      unsigned int       > : numeric_converter< unsigned char,      unsigned int       >  {};
	template<> struct converter< unsigned char,      unsigned long      > : numeric_converter< unsigned char,      unsigned long      >  {};
	template<> struct converter< unsigned char,      unsigned long long > : numeric_converter< unsigned char,      unsigned long long >  {};
	template<> struct converter< unsigned char,               float     > : numeric_converter< unsigned char,               float     >  {};
	template<> struct converter< unsigned char,               double    > : numeric_converter< unsigned char,               double    >  {};
	template<> struct converter< unsigned char,          long double    > : numeric_converter< unsigned char,          long double    >  {};
	
	
	
	template<> struct converter<   signed short,              char      > : numeric_converter<   signed short,              char      >  {};
	template<> struct converter<   signed short,       signed int       > : numeric_converter<   signed short,       signed int       >  {};
	template<> struct converter<   signed short,       signed long      > : numeric_converter<   signed short,       signed long      >  {};
	template<> struct converter<   signed short,       signed long long > : numeric_converter<   signed short,       signed long long >  {};
	template<> struct converter<   signed short,     unsigned char      > : numeric_converter<   signed short,     unsigned char      >  {};
	template<> struct converter<   signed short,     unsigned short     > : numeric_converter<   signed short,     unsigned short     >  {};
	template<> struct converter<   signed short,     unsigned int       > : numeric_converter<   signed short,     unsigned int       >  {};
	template<> struct converter<   signed short,     unsigned long      > : numeric_converter<   signed short,     unsigned long      >  {};
	template<> struct converter<   signed short,     unsigned long long > : numeric_converter<   signed short,     unsigned long long >  {};
	template<> struct converter<   signed short,              float     > : numeric_converter<   signed short,              float     >  {};
	template<> struct converter<   signed short,              double    > : numeric_converter<   signed short,              double    >  {};
	template<> struct converter<   signed short,         long double    > : numeric_converter<   signed short,         long double    >  {};
	
	template<> struct converter< unsigned short,              char      > : numeric_converter< unsigned short,              char      >  {};
	template<> struct converter< unsigned short,       signed char      > : numeric_converter< unsigned short,       signed char      >  {};
	template<> struct converter< unsigned short,       signed short     > : numeric_converter< unsigned short,       signed short     >  {};
	template<> struct converter< unsigned short,       signed int       > : numeric_converter< unsigned short,       signed int       >  {};
	template<> struct converter< unsigned short,       signed long      > : numeric_converter< unsigned short,       signed long      >  {};
	template<> struct converter< unsigned short,       signed long long > : numeric_converter< unsigned short,       signed long long >  {};
	template<> struct converter< unsigned short,     unsigned int       > : numeric_converter< unsigned short,     unsigned int       >  {};
	template<> struct converter< unsigned short,     unsigned long      > : numeric_converter< unsigned short,     unsigned long      >  {};
	template<> struct converter< unsigned short,     unsigned long long > : numeric_converter< unsigned short,     unsigned long long >  {};
	template<> struct converter< unsigned short,              float     > : numeric_converter< unsigned short,              float     >  {};
	template<> struct converter< unsigned short,              double    > : numeric_converter< unsigned short,              double    >  {};
	template<> struct converter< unsigned short,         long double    > : numeric_converter< unsigned short,         long double    >  {};
	
	
	
	template<> struct converter<   signed int,                char      > : numeric_converter<   signed int,                char      >  {};
	template<> struct converter<   signed int,         signed long      > : numeric_converter<   signed int,         signed long      >  {};
	template<> struct converter<   signed int,         signed long long > : numeric_converter<   signed int,         signed long long >  {};
	template<> struct converter<   signed int,       unsigned char      > : numeric_converter<   signed int,       unsigned char      >  {};
	template<> struct converter<   signed int,       unsigned short     > : numeric_converter<   signed int,       unsigned short     >  {};
	template<> struct converter<   signed int,       unsigned int       > : numeric_converter<   signed int,       unsigned int       >  {};
	template<> struct converter<   signed int,       unsigned long      > : numeric_converter<   signed int,       unsigned long      >  {};
	template<> struct converter<   signed int,       unsigned long long > : numeric_converter<   signed int,       unsigned long long >  {};
	template<> struct converter<   signed int,                float     > : numeric_converter<   signed int,                float     >  {};
	template<> struct converter<   signed int,                double    > : numeric_converter<   signed int,                double    >  {};
	template<> struct converter<   signed int,           long double    > : numeric_converter<   signed int,           long double    >  {};
	
	template<> struct converter< unsigned int,                char      > : numeric_converter< unsigned int,                char      >  {};
	template<> struct converter< unsigned int,         signed char      > : numeric_converter< unsigned int,         signed char      >  {};
	template<> struct converter< unsigned int,         signed short     > : numeric_converter< unsigned int,         signed short     >  {};
	template<> struct converter< unsigned int,         signed int       > : numeric_converter< unsigned int,         signed int       >  {};
	template<> struct converter< unsigned int,         signed long      > : numeric_converter< unsigned int,         signed long      >  {};
	template<> struct converter< unsigned int,         signed long long > : numeric_converter< unsigned int,         signed long long >  {};
	template<> struct converter< unsigned int,       unsigned long      > : numeric_converter< unsigned int,       unsigned long      >  {};
	template<> struct converter< unsigned int,       unsigned long long > : numeric_converter< unsigned int,       unsigned long long >  {};
	template<> struct converter< unsigned int,                float     > : numeric_converter< unsigned int,                float     >  {};
	template<> struct converter< unsigned int,                double    > : numeric_converter< unsigned int,                double    >  {};
	template<> struct converter< unsigned int,           long double    > : numeric_converter< unsigned int,           long double    >  {};
	
	
	
	template<> struct converter<   signed long,               char      > : numeric_converter<   signed long,               char      >  {};
	template<> struct converter<   signed long,        signed long long > : numeric_converter<   signed long,        signed long long >  {};
	template<> struct converter<   signed long,      unsigned char      > : numeric_converter<   signed long,      unsigned char      >  {};
	template<> struct converter<   signed long,      unsigned short     > : numeric_converter<   signed long,      unsigned short     >  {};
	template<> struct converter<   signed long,      unsigned int       > : numeric_converter<   signed long,      unsigned int       >  {};
	template<> struct converter<   signed long,      unsigned long      > : numeric_converter<   signed long,      unsigned long      >  {};
	template<> struct converter<   signed long,      unsigned long long > : numeric_converter<   signed long,      unsigned long long >  {};
	template<> struct converter<   signed long,               float     > : numeric_converter<   signed long,               float     >  {};
	template<> struct converter<   signed long,               double    > : numeric_converter<   signed long,               double    >  {};
	template<> struct converter<   signed long,          long double    > : numeric_converter<   signed long,          long double    >  {};
	
	template<> struct converter< unsigned long,               char      > : numeric_converter< unsigned long,               char      >  {};
	template<> struct converter< unsigned long,        signed char      > : numeric_converter< unsigned long,        signed char      >  {};
	template<> struct converter< unsigned long,        signed short     > : numeric_converter< unsigned long,        signed short     >  {};
	template<> struct converter< unsigned long,        signed int       > : numeric_converter< unsigned long,        signed int       >  {};
	template<> struct converter< unsigned long,        signed long      > : numeric_converter< unsigned long,        signed long      >  {};
	template<> struct converter< unsigned long,        signed long long > : numeric_converter< unsigned long,        signed long long >  {};
	template<> struct converter< unsigned long,      unsigned long long > : numeric_converter< unsigned long,      unsigned long long >  {};
	template<> struct converter< unsigned long,               float     > : numeric_converter< unsigned long,               float     >  {};
	template<> struct converter< unsigned long,               double    > : numeric_converter< unsigned long,               double    >  {};
	template<> struct converter< unsigned long,          long double    > : numeric_converter< unsigned long,          long double    >  {};
	
	
	
	template<> struct converter<   signed long long,          char      > : numeric_converter<   signed long long,          char      >  {};
	template<> struct converter<   signed long long,   signed long long > : numeric_converter<   signed long long,   signed long long >  {};
	template<> struct converter<   signed long long, unsigned char      > : numeric_converter<   signed long long, unsigned char      >  {};
	template<> struct converter<   signed long long, unsigned short     > : numeric_converter<   signed long long, unsigned short     >  {};
	template<> struct converter<   signed long long, unsigned int       > : numeric_converter<   signed long long, unsigned int       >  {};
	template<> struct converter<   signed long long, unsigned long      > : numeric_converter<   signed long long, unsigned long      >  {};
	template<> struct converter<   signed long long, unsigned long long > : numeric_converter<   signed long long, unsigned long long >  {};
	template<> struct converter<   signed long long,          float     > : numeric_converter<   signed long long,          float     >  {};
	template<> struct converter<   signed long long,          double    > : numeric_converter<   signed long long,          double    >  {};
	template<> struct converter<   signed long long,     long double    > : numeric_converter<   signed long long,     long double    >  {};
	
	template<> struct converter< unsigned long long,          char      > : numeric_converter< unsigned long long,          char      >  {};
	template<> struct converter< unsigned long long,   signed char      > : numeric_converter< unsigned long long,   signed char      >  {};
	template<> struct converter< unsigned long long,   signed short     > : numeric_converter< unsigned long long,   signed short     >  {};
	template<> struct converter< unsigned long long,   signed int       > : numeric_converter< unsigned long long,   signed int       >  {};
	template<> struct converter< unsigned long long,   signed long      > : numeric_converter< unsigned long long,   signed long      >  {};
	template<> struct converter< unsigned long long,   signed long long > : numeric_converter< unsigned long long,   signed long long >  {};
	template<> struct converter< unsigned long long, unsigned long long > : numeric_converter< unsigned long long, unsigned long long >  {};
	template<> struct converter< unsigned long long,          float     > : numeric_converter< unsigned long long,          float     >  {};
	template<> struct converter< unsigned long long,          double    > : numeric_converter< unsigned long long,          double    >  {};
	template<> struct converter< unsigned long long,     long double    > : numeric_converter< unsigned long long,     long double    >  {};
	
	
	
	template<> struct converter<          float,              char      > : numeric_converter<          float,              char      >  {};
	template<> struct converter<          float,       signed char      > : numeric_converter<          float,       signed char      >  {};
	template<> struct converter<          float,       signed short     > : numeric_converter<          float,       signed short     >  {};
	template<> struct converter<          float,       signed int       > : numeric_converter<          float,       signed int       >  {};
	template<> struct converter<          float,       signed long      > : numeric_converter<          float,       signed long      >  {};
	template<> struct converter<          float,       signed long long > : numeric_converter<          float,       signed long long >  {};
	template<> struct converter<          float,     unsigned char      > : numeric_converter<          float,     unsigned char      >  {};
	template<> struct converter<          float,     unsigned short     > : numeric_converter<          float,     unsigned short     >  {};
	template<> struct converter<          float,     unsigned int       > : numeric_converter<          float,     unsigned int       >  {};
	template<> struct converter<          float,     unsigned long      > : numeric_converter<          float,     unsigned long      >  {};
	template<> struct converter<          float,     unsigned long long > : numeric_converter<          float,     unsigned long long >  {};
	template<> struct converter<          float,              double    > : numeric_converter<          float,              double    >  {};
	template<> struct converter<          float,         long double    > : numeric_converter<          float,         long double    >  {};
	
	
	
	template<> struct converter<         double,              char      > : numeric_converter<         double,              char      >  {};
	template<> struct converter<         double,       signed char      > : numeric_converter<         double,       signed char      >  {};
	template<> struct converter<         double,       signed short     > : numeric_converter<         double,       signed short     >  {};
	template<> struct converter<         double,       signed int       > : numeric_converter<         double,       signed int       >  {};
	template<> struct converter<         double,       signed long      > : numeric_converter<         double,       signed long      >  {};
	template<> struct converter<         double,       signed long long > : numeric_converter<         double,       signed long long >  {};
	template<> struct converter<         double,     unsigned char      > : numeric_converter<         double,     unsigned char      >  {};
	template<> struct converter<         double,     unsigned short     > : numeric_converter<         double,     unsigned short     >  {};
	template<> struct converter<         double,     unsigned int       > : numeric_converter<         double,     unsigned int       >  {};
	template<> struct converter<         double,     unsigned long      > : numeric_converter<         double,     unsigned long      >  {};
	template<> struct converter<         double,     unsigned long long > : numeric_converter<         double,     unsigned long long >  {};
	template<> struct converter<         double,         long double    > : numeric_converter<         double,         long double    >  {};
	
	
	
	template<> struct converter<    long double,              char      > : numeric_converter<    long double,              char      >  {};
	template<> struct converter<    long double,       signed char      > : numeric_converter<    long double,       signed char      >  {};
	template<> struct converter<    long double,       signed short     > : numeric_converter<    long double,       signed short     >  {};
	template<> struct converter<    long double,       signed int       > : numeric_converter<    long double,       signed int       >  {};
	template<> struct converter<    long double,       signed long      > : numeric_converter<    long double,       signed long      >  {};
	template<> struct converter<    long double,       signed long long > : numeric_converter<    long double,       signed long long >  {};
	template<> struct converter<    long double,     unsigned char      > : numeric_converter<    long double,     unsigned char      >  {};
	template<> struct converter<    long double,     unsigned short     > : numeric_converter<    long double,     unsigned short     >  {};
	template<> struct converter<    long double,     unsigned int       > : numeric_converter<    long double,     unsigned int       >  {};
	template<> struct converter<    long double,     unsigned long      > : numeric_converter<    long double,     unsigned long      >  {};
	template<> struct converter<    long double,     unsigned long long > : numeric_converter<    long double,     unsigned long long >  {};
	
}

#endif
