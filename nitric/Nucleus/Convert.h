// Nucleus/Convert.h
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


/*
   Convert: The swiss army knife of conversion functions
   
   Some libraries come with a plethora of conversion functions, and adapting
   a library to C++ will often introduce even more.  Keeping track of the
   names given to these functions can be hard for a programmer, and it's
   essentially impossible for a template.
   
   Therefore, Nitrogen provides a single, extensible, template, Convert, to
   encompass all simple conversions:
   
      template < class Output, class Input > Output Convert( const Input& );
   
   Since the input parameter can be inferred from a function parameter, one
   can often write Convert<DesiredType>( input ).
   
   In general, the conversions provided by Convert are value-preserving,
   but not precision-preserving.  If the parameter is out of range for
   the output type, or the conversion cannot be performed, Convert
   throws an exception.  When the Output type has insufficient
   precision to exactly represent the input value, the result
   is chosen from among the closest output values.
   
   Convert also provides conversions between C++ types, as a way of
   priming the pump.  It can be used for range-checked, rounding conversions
   between the built-in numeric types, and it converts things to and
   from the standard string types by using the streaming operators.
   
   Where no specialization has been provided, Convert attempts an implicit
   conversion.
   
   In order to allow for partial specialization, Convert<Input,Output>
   uses a functor
   
      template < class Output, class Input > struct Converter;
   
   to choose the conversion.  One may extend Convert by specializing this
   class, providing a member function operator() to perform the conversion:
   
      template < partial-specialization-parameters >
      struct Converter< Output, Input >: public std::unary_function< Input, Output >
        {
         Output operator()( const Input& ) const;
        };
   
   When Convert is called with more than one parameter, the additional parameters
   are passed on to the constructor of the Converter functor.
*/

#ifndef NUCLEUS_CONVERT_H
#define NUCLEUS_CONVERT_H

#ifndef NUCLEUS_OVERLOADED_MATH_H
#include "Nucleus/Overloaded_math.h"
#endif

#include <limits>
#include <sstream>
#include <string>
#include <ios>

namespace Nucleus
  {
   template < class Output, class Input >
   struct Converter: public std::unary_function< Input, Output >
     {
      Output operator()( const Input& in ) const      { return Output( in ); }
     };

   template < class Input >
   struct ConvertInputTraits
     {
      typedef Input ConverterInputType;
     };
   
   template < class Output, class Input >
   inline Output Convert( const Input& input )
     {
      return Converter< Output, typename ConvertInputTraits<Input>::ConverterInputType >()( input );
     }

   template < class Output, class Input, class P1 >
   inline Output Convert( const Input& input, const P1& p1 )
     {
      return Converter< Output, typename ConvertInputTraits<Input>::ConverterInputType >( p1 )( input );
     }

   template < class Output, class Input, class P1, class P2 >
   inline Output Convert( const Input& input, const P1& p1, const P2& p2 )
     {
      return Converter< Output, typename ConvertInputTraits<Input>::ConverterInputType >( p1, p2 )( input );
     }

   template < class Output, class Input, class P1, class P2, class P3 >
   inline Output Convert( const Input& input, const P1& p1, const P2& p2, const P3& p3 )
     {
      return Converter< Output, typename ConvertInputTraits<Input>::ConverterInputType >( p1, p2, p3 )( input );
     }


   
   template < class CharT,
              class Traits,
              class Allocator,
              class Input >
   struct Converter< std::basic_string< CharT, Traits, Allocator >, Input >
              : public std::unary_function< Input, std::basic_string< CharT, Traits, Allocator > >
     {
      std::basic_string< CharT, Traits, Allocator > operator()( const Input& input ) const
        {
         std::basic_ostringstream< CharT, Traits, Allocator > stream;
         stream << std::boolalpha << input;
         return stream.str(); 
        }
     };
   
   class ConversionFromStringFailed {};
   
   template < class Output,
              class CharT,
              class Traits,
              class Allocator >
   struct Converter< Output, std::basic_string< CharT, Traits, Allocator > >
            : public std::unary_function< std::basic_string< CharT, Traits, Allocator >, Output >
     {
      Output operator()( const std::basic_string< CharT, Traits, Allocator >& input ) const
        {
         std::basic_istringstream< CharT, Traits, Allocator > stream( input );
         Output output;
         stream >> std::boolalpha >> output;
         if ( stream.snextc() != Traits::eof() )
            throw ConversionFromStringFailed();
         return output; 
        }
     };
   
   
   struct NumericConversionRangeError                                      {};
   struct NumericConversionOverflowError:   NumericConversionRangeError    {};
   struct NumericConversionNotANumberError: NumericConversionRangeError    {};
   
   template < class Output,
              class Input,
              bool radicesAreEqual = ( std::numeric_limits<Input>::radix == std::numeric_limits<Output>::radix ) >
   struct NumericConverter;
      /* I don't know how to safely do bounds checking when the radices aren't equal. --LL */
   
   template < class Output, class Input >
   struct NumericConverter< Output, Input, true >: public std::unary_function< Input, Output >
     {
      Output operator()( const Input& input )
        {
         const Input in = ( !std::numeric_limits<Input>::is_integer && std::numeric_limits<Output>::is_integer )
                          ? static_cast<Input>( CStd::nearbyint( input ) )
                          : input;
         
         static const bool inputHasNaN =     std::numeric_limits<Input>::has_quiet_NaN
                                          || std::numeric_limits<Input>::has_signaling_NaN;
         
         static const bool outputHasNaN =    std::numeric_limits<Output>::has_quiet_NaN
                                          || std::numeric_limits<Output>::has_signaling_NaN;

         if ( inputHasNaN && !outputHasNaN && in != in )
            throw NumericConversionNotANumberError();
         
         // We'll assume that inexact types for which this is used are floating-point.

         static const int inputSize = std::numeric_limits<Input>::is_exact
                                       ? std::numeric_limits<Input>::digits
                                       : std::numeric_limits<Input>::max_exponent;

         static const int outputSize = std::numeric_limits<Output>::is_exact
                                        ? std::numeric_limits<Output>::digits
                                        : std::numeric_limits<Output>::max_exponent;
         
         static const bool shrinking =     std::numeric_limits<Output>::is_bounded
                                       && !std::numeric_limits<Output>::has_infinity
                                       && ( !std::numeric_limits<Input>::is_bounded || inputSize > outputSize );
         
         if ( shrinking )
           {
            if ( in > static_cast<Input>( std::numeric_limits<Output>::max() ) )
               throw NumericConversionOverflowError();

            if ( std::numeric_limits<Input>::is_signed )
              {
               if ( std::numeric_limits<Output>::is_signed )
                 {
                  const Output outputMin = std::numeric_limits<Output>::is_exact
                                             ? std::numeric_limits<Output>::min()
                                             : static_cast<Output>( -std::numeric_limits<Output>::max() );

                  if ( in < static_cast<Input>( outputMin ) )
                     throw NumericConversionOverflowError();
                 }
                else
                 {
                  if ( in < static_cast<Input>( 0 ) )
                     throw NumericConversionOverflowError();
                 }
              }
            
            if ( std::numeric_limits<Input>::is_signed && in < static_cast<Input>( std::numeric_limits<Output>::min() ) )
               throw NumericConversionOverflowError();
           }
          else
           {
            static const bool signedToUnsigned =    std::numeric_limits<Input>::is_signed
                                                && !std::numeric_limits<Output>::is_signed;
            
            if ( signedToUnsigned && in < static_cast<Input>( 0 ) )
               throw NumericConversionOverflowError();
           }
         
         return static_cast<Output>( in );
        }
     };
  
   template<> struct Converter<          char,        signed char      > : NumericConverter<          char,        signed char      >  {};
   template<> struct Converter<          char,        signed short     > : NumericConverter<          char,        signed short     >  {};
   template<> struct Converter<          char,        signed int       > : NumericConverter<          char,        signed int       >  {};
   template<> struct Converter<          char,        signed long      > : NumericConverter<          char,        signed long      >  {};
   template<> struct Converter<          char,        signed long long > : NumericConverter<          char,        signed long long >  {};
   template<> struct Converter<          char,      unsigned char      > : NumericConverter<          char,      unsigned char      >  {};
   template<> struct Converter<          char,      unsigned short     > : NumericConverter<          char,      unsigned short     >  {};
   template<> struct Converter<          char,      unsigned int       > : NumericConverter<          char,      unsigned int       >  {};
   template<> struct Converter<          char,      unsigned long      > : NumericConverter<          char,      unsigned long      >  {};
   template<> struct Converter<          char,      unsigned long long > : NumericConverter<          char,      unsigned long long >  {};
   template<> struct Converter<          char,               float     > : NumericConverter<          char,               float     >  {};
   template<> struct Converter<          char,               double    > : NumericConverter<          char,               double    >  {};
   template<> struct Converter<          char,          long double    > : NumericConverter<          char,          long double    >  {};
   
   template<> struct Converter<   signed char,               char      > : NumericConverter<   signed char,               char      >  {};
   template<> struct Converter<   signed char,        signed short     > : NumericConverter<   signed char,        signed short     >  {};
   template<> struct Converter<   signed char,        signed int       > : NumericConverter<   signed char,        signed int       >  {};
   template<> struct Converter<   signed char,        signed long      > : NumericConverter<   signed char,        signed long      >  {};
   template<> struct Converter<   signed char,        signed long long > : NumericConverter<   signed char,        signed long long >  {};
   template<> struct Converter<   signed char,      unsigned char      > : NumericConverter<   signed char,      unsigned char      >  {};
   template<> struct Converter<   signed char,      unsigned short     > : NumericConverter<   signed char,      unsigned short     >  {};
   template<> struct Converter<   signed char,      unsigned int       > : NumericConverter<   signed char,      unsigned int       >  {};
   template<> struct Converter<   signed char,      unsigned long      > : NumericConverter<   signed char,      unsigned long      >  {};
   template<> struct Converter<   signed char,      unsigned long long > : NumericConverter<   signed char,      unsigned long long >  {};
   template<> struct Converter<   signed char,               float     > : NumericConverter<   signed char,               float     >  {};
   template<> struct Converter<   signed char,               double    > : NumericConverter<   signed char,               double    >  {};
   template<> struct Converter<   signed char,          long double    > : NumericConverter<   signed char,          long double    >  {};
   
   template<> struct Converter< unsigned char,               char      > : NumericConverter< unsigned char,               char      >  {};
   template<> struct Converter< unsigned char,        signed char      > : NumericConverter< unsigned char,        signed char      >  {};
   template<> struct Converter< unsigned char,        signed short     > : NumericConverter< unsigned char,        signed short     >  {};
   template<> struct Converter< unsigned char,        signed int       > : NumericConverter< unsigned char,        signed int       >  {};
   template<> struct Converter< unsigned char,        signed long      > : NumericConverter< unsigned char,        signed long      >  {};
   template<> struct Converter< unsigned char,        signed long long > : NumericConverter< unsigned char,        signed long long >  {};
   template<> struct Converter< unsigned char,      unsigned short     > : NumericConverter< unsigned char,      unsigned short     >  {};
   template<> struct Converter< unsigned char,      unsigned int       > : NumericConverter< unsigned char,      unsigned int       >  {};
   template<> struct Converter< unsigned char,      unsigned long      > : NumericConverter< unsigned char,      unsigned long      >  {};
   template<> struct Converter< unsigned char,      unsigned long long > : NumericConverter< unsigned char,      unsigned long long >  {};
   template<> struct Converter< unsigned char,               float     > : NumericConverter< unsigned char,               float     >  {};
   template<> struct Converter< unsigned char,               double    > : NumericConverter< unsigned char,               double    >  {};
   template<> struct Converter< unsigned char,          long double    > : NumericConverter< unsigned char,          long double    >  {};



   template<> struct Converter<   signed short,              char      > : NumericConverter<   signed short,              char      >  {};
   template<> struct Converter<   signed short,       signed int       > : NumericConverter<   signed short,       signed int       >  {};
   template<> struct Converter<   signed short,       signed long      > : NumericConverter<   signed short,       signed long      >  {};
   template<> struct Converter<   signed short,       signed long long > : NumericConverter<   signed short,       signed long long >  {};
   template<> struct Converter<   signed short,     unsigned char      > : NumericConverter<   signed short,     unsigned char      >  {};
   template<> struct Converter<   signed short,     unsigned short     > : NumericConverter<   signed short,     unsigned short     >  {};
   template<> struct Converter<   signed short,     unsigned int       > : NumericConverter<   signed short,     unsigned int       >  {};
   template<> struct Converter<   signed short,     unsigned long      > : NumericConverter<   signed short,     unsigned long      >  {};
   template<> struct Converter<   signed short,     unsigned long long > : NumericConverter<   signed short,     unsigned long long >  {};
   template<> struct Converter<   signed short,              float     > : NumericConverter<   signed short,              float     >  {};
   template<> struct Converter<   signed short,              double    > : NumericConverter<   signed short,              double    >  {};
   template<> struct Converter<   signed short,         long double    > : NumericConverter<   signed short,         long double    >  {};

   template<> struct Converter< unsigned short,              char      > : NumericConverter< unsigned short,              char      >  {};
   template<> struct Converter< unsigned short,       signed char      > : NumericConverter< unsigned short,       signed char      >  {};
   template<> struct Converter< unsigned short,       signed short     > : NumericConverter< unsigned short,       signed short     >  {};
   template<> struct Converter< unsigned short,       signed int       > : NumericConverter< unsigned short,       signed int       >  {};
   template<> struct Converter< unsigned short,       signed long      > : NumericConverter< unsigned short,       signed long      >  {};
   template<> struct Converter< unsigned short,       signed long long > : NumericConverter< unsigned short,       signed long long >  {};
   template<> struct Converter< unsigned short,     unsigned int       > : NumericConverter< unsigned short,     unsigned int       >  {};
   template<> struct Converter< unsigned short,     unsigned long      > : NumericConverter< unsigned short,     unsigned long      >  {};
   template<> struct Converter< unsigned short,     unsigned long long > : NumericConverter< unsigned short,     unsigned long long >  {};
   template<> struct Converter< unsigned short,              float     > : NumericConverter< unsigned short,              float     >  {};
   template<> struct Converter< unsigned short,              double    > : NumericConverter< unsigned short,              double    >  {};
   template<> struct Converter< unsigned short,         long double    > : NumericConverter< unsigned short,         long double    >  {};



   template<> struct Converter<   signed int,                char      > : NumericConverter<   signed int,                char      >  {};
   template<> struct Converter<   signed int,         signed long      > : NumericConverter<   signed int,         signed long      >  {};
   template<> struct Converter<   signed int,         signed long long > : NumericConverter<   signed int,         signed long long >  {};
   template<> struct Converter<   signed int,       unsigned char      > : NumericConverter<   signed int,       unsigned char      >  {};
   template<> struct Converter<   signed int,       unsigned short     > : NumericConverter<   signed int,       unsigned short     >  {};
   template<> struct Converter<   signed int,       unsigned int       > : NumericConverter<   signed int,       unsigned int       >  {};
   template<> struct Converter<   signed int,       unsigned long      > : NumericConverter<   signed int,       unsigned long      >  {};
   template<> struct Converter<   signed int,       unsigned long long > : NumericConverter<   signed int,       unsigned long long >  {};
   template<> struct Converter<   signed int,                float     > : NumericConverter<   signed int,                float     >  {};
   template<> struct Converter<   signed int,                double    > : NumericConverter<   signed int,                double    >  {};
   template<> struct Converter<   signed int,           long double    > : NumericConverter<   signed int,           long double    >  {};

   template<> struct Converter< unsigned int,                char      > : NumericConverter< unsigned int,                char      >  {};
   template<> struct Converter< unsigned int,         signed char      > : NumericConverter< unsigned int,         signed char      >  {};
   template<> struct Converter< unsigned int,         signed short     > : NumericConverter< unsigned int,         signed short     >  {};
   template<> struct Converter< unsigned int,         signed int       > : NumericConverter< unsigned int,         signed int       >  {};
   template<> struct Converter< unsigned int,         signed long      > : NumericConverter< unsigned int,         signed long      >  {};
   template<> struct Converter< unsigned int,         signed long long > : NumericConverter< unsigned int,         signed long long >  {};
   template<> struct Converter< unsigned int,       unsigned long      > : NumericConverter< unsigned int,       unsigned long      >  {};
   template<> struct Converter< unsigned int,       unsigned long long > : NumericConverter< unsigned int,       unsigned long long >  {};
   template<> struct Converter< unsigned int,                float     > : NumericConverter< unsigned int,                float     >  {};
   template<> struct Converter< unsigned int,                double    > : NumericConverter< unsigned int,                double    >  {};
   template<> struct Converter< unsigned int,           long double    > : NumericConverter< unsigned int,           long double    >  {};



   template<> struct Converter<   signed long,               char      > : NumericConverter<   signed long,               char      >  {};
   template<> struct Converter<   signed long,        signed long long > : NumericConverter<   signed long,        signed long long >  {};
   template<> struct Converter<   signed long,      unsigned char      > : NumericConverter<   signed long,      unsigned char      >  {};
   template<> struct Converter<   signed long,      unsigned short     > : NumericConverter<   signed long,      unsigned short     >  {};
   template<> struct Converter<   signed long,      unsigned int       > : NumericConverter<   signed long,      unsigned int       >  {};
   template<> struct Converter<   signed long,      unsigned long      > : NumericConverter<   signed long,      unsigned long      >  {};
   template<> struct Converter<   signed long,      unsigned long long > : NumericConverter<   signed long,      unsigned long long >  {};
   template<> struct Converter<   signed long,               float     > : NumericConverter<   signed long,               float     >  {};
   template<> struct Converter<   signed long,               double    > : NumericConverter<   signed long,               double    >  {};
   template<> struct Converter<   signed long,          long double    > : NumericConverter<   signed long,          long double    >  {};

   template<> struct Converter< unsigned long,               char      > : NumericConverter< unsigned long,               char      >  {};
   template<> struct Converter< unsigned long,        signed char      > : NumericConverter< unsigned long,        signed char      >  {};
   template<> struct Converter< unsigned long,        signed short     > : NumericConverter< unsigned long,        signed short     >  {};
   template<> struct Converter< unsigned long,        signed int       > : NumericConverter< unsigned long,        signed int       >  {};
   template<> struct Converter< unsigned long,        signed long      > : NumericConverter< unsigned long,        signed long      >  {};
   template<> struct Converter< unsigned long,        signed long long > : NumericConverter< unsigned long,        signed long long >  {};
   template<> struct Converter< unsigned long,      unsigned long long > : NumericConverter< unsigned long,      unsigned long long >  {};
   template<> struct Converter< unsigned long,               float     > : NumericConverter< unsigned long,               float     >  {};
   template<> struct Converter< unsigned long,               double    > : NumericConverter< unsigned long,               double    >  {};
   template<> struct Converter< unsigned long,          long double    > : NumericConverter< unsigned long,          long double    >  {};



   template<> struct Converter<   signed long long,          char      > : NumericConverter<   signed long long,          char      >  {};
   template<> struct Converter<   signed long long,   signed long long > : NumericConverter<   signed long long,   signed long long >  {};
   template<> struct Converter<   signed long long, unsigned char      > : NumericConverter<   signed long long, unsigned char      >  {};
   template<> struct Converter<   signed long long, unsigned short     > : NumericConverter<   signed long long, unsigned short     >  {};
   template<> struct Converter<   signed long long, unsigned int       > : NumericConverter<   signed long long, unsigned int       >  {};
   template<> struct Converter<   signed long long, unsigned long      > : NumericConverter<   signed long long, unsigned long      >  {};
   template<> struct Converter<   signed long long, unsigned long long > : NumericConverter<   signed long long, unsigned long long >  {};
   template<> struct Converter<   signed long long,          float     > : NumericConverter<   signed long long,          float     >  {};
   template<> struct Converter<   signed long long,          double    > : NumericConverter<   signed long long,          double    >  {};
   template<> struct Converter<   signed long long,     long double    > : NumericConverter<   signed long long,     long double    >  {};

   template<> struct Converter< unsigned long long,          char      > : NumericConverter< unsigned long long,          char      >  {};
   template<> struct Converter< unsigned long long,   signed char      > : NumericConverter< unsigned long long,   signed char      >  {};
   template<> struct Converter< unsigned long long,   signed short     > : NumericConverter< unsigned long long,   signed short     >  {};
   template<> struct Converter< unsigned long long,   signed int       > : NumericConverter< unsigned long long,   signed int       >  {};
   template<> struct Converter< unsigned long long,   signed long      > : NumericConverter< unsigned long long,   signed long      >  {};
   template<> struct Converter< unsigned long long,   signed long long > : NumericConverter< unsigned long long,   signed long long >  {};
   template<> struct Converter< unsigned long long, unsigned long long > : NumericConverter< unsigned long long, unsigned long long >  {};
   template<> struct Converter< unsigned long long,          float     > : NumericConverter< unsigned long long,          float     >  {};
   template<> struct Converter< unsigned long long,          double    > : NumericConverter< unsigned long long,          double    >  {};
   template<> struct Converter< unsigned long long,     long double    > : NumericConverter< unsigned long long,     long double    >  {};



   template<> struct Converter<          float,              char      > : NumericConverter<          float,              char      >  {};
   template<> struct Converter<          float,       signed char      > : NumericConverter<          float,       signed char      >  {};
   template<> struct Converter<          float,       signed short     > : NumericConverter<          float,       signed short     >  {};
   template<> struct Converter<          float,       signed int       > : NumericConverter<          float,       signed int       >  {};
   template<> struct Converter<          float,       signed long      > : NumericConverter<          float,       signed long      >  {};
   template<> struct Converter<          float,       signed long long > : NumericConverter<          float,       signed long long >  {};
   template<> struct Converter<          float,     unsigned char      > : NumericConverter<          float,     unsigned char      >  {};
   template<> struct Converter<          float,     unsigned short     > : NumericConverter<          float,     unsigned short     >  {};
   template<> struct Converter<          float,     unsigned int       > : NumericConverter<          float,     unsigned int       >  {};
   template<> struct Converter<          float,     unsigned long      > : NumericConverter<          float,     unsigned long      >  {};
   template<> struct Converter<          float,     unsigned long long > : NumericConverter<          float,     unsigned long long >  {};
   template<> struct Converter<          float,              double    > : NumericConverter<          float,              double    >  {};
   template<> struct Converter<          float,         long double    > : NumericConverter<          float,         long double    >  {};



   template<> struct Converter<         double,              char      > : NumericConverter<         double,              char      >  {};
   template<> struct Converter<         double,       signed char      > : NumericConverter<         double,       signed char      >  {};
   template<> struct Converter<         double,       signed short     > : NumericConverter<         double,       signed short     >  {};
   template<> struct Converter<         double,       signed int       > : NumericConverter<         double,       signed int       >  {};
   template<> struct Converter<         double,       signed long      > : NumericConverter<         double,       signed long      >  {};
   template<> struct Converter<         double,       signed long long > : NumericConverter<         double,       signed long long >  {};
   template<> struct Converter<         double,     unsigned char      > : NumericConverter<         double,     unsigned char      >  {};
   template<> struct Converter<         double,     unsigned short     > : NumericConverter<         double,     unsigned short     >  {};
   template<> struct Converter<         double,     unsigned int       > : NumericConverter<         double,     unsigned int       >  {};
   template<> struct Converter<         double,     unsigned long      > : NumericConverter<         double,     unsigned long      >  {};
   template<> struct Converter<         double,     unsigned long long > : NumericConverter<         double,     unsigned long long >  {};
   template<> struct Converter<         double,         long double    > : NumericConverter<         double,         long double    >  {};



   template<> struct Converter<    long double,              char      > : NumericConverter<    long double,              char      >  {};
   template<> struct Converter<    long double,       signed char      > : NumericConverter<    long double,       signed char      >  {};
   template<> struct Converter<    long double,       signed short     > : NumericConverter<    long double,       signed short     >  {};
   template<> struct Converter<    long double,       signed int       > : NumericConverter<    long double,       signed int       >  {};
   template<> struct Converter<    long double,       signed long      > : NumericConverter<    long double,       signed long      >  {};
   template<> struct Converter<    long double,       signed long long > : NumericConverter<    long double,       signed long long >  {};
   template<> struct Converter<    long double,     unsigned char      > : NumericConverter<    long double,     unsigned char      >  {};
   template<> struct Converter<    long double,     unsigned short     > : NumericConverter<    long double,     unsigned short     >  {};
   template<> struct Converter<    long double,     unsigned int       > : NumericConverter<    long double,     unsigned int       >  {};
   template<> struct Converter<    long double,     unsigned long      > : NumericConverter<    long double,     unsigned long      >  {};
   template<> struct Converter<    long double,     unsigned long long > : NumericConverter<    long double,     unsigned long long >  {};
  }

#endif
