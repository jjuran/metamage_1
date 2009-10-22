// Nitrogen/CFNumber.h
// -------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFNUMBER_H
#define NITROGEN_CFNUMBER_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFDATE__
#include FRAMEWORK_HEADER(CoreFoundation,CFNumber.h)
#endif
#ifndef NITROGEN_CFBOOLEAN_H
#include "Nitrogen/CFBoolean.h"
#endif

namespace Nitrogen
  {
   using ::CFNumberRef;
   using ::CFNumberType;
   
   template <> struct CFType_Traits< CFNumberRef >: Basic_CFType_Traits< CFNumberRef, ::CFNumberGetTypeID > {};
  }

namespace Nucleus
  {
   template <> struct Disposer_Traits< Nitrogen::CFNumberRef > : Disposer_Traits<Nitrogen::CFTypeRef>  {};
  }

namespace Nitrogen
  {
   inline void CFShow( const CFNumberRef n )        { ::CFShow( n ); }
 
   template < class NumericType > struct CFNumber_Traits;
   
   template < class Type, CFNumberType typeCode >
   struct Basic_CFNumber_Traits
     {
      typedef Type ClosestType;
      static const CFNumberType type = typeCode;
     };
   
   template <> struct CFNumber_Traits< char      >: Basic_CFNumber_Traits< char,      ::kCFNumberCharType     > {};
   template <> struct CFNumber_Traits< short     >: Basic_CFNumber_Traits< short,     ::kCFNumberShortType    > {};
   template <> struct CFNumber_Traits< int       >: Basic_CFNumber_Traits< int,       ::kCFNumberIntType      > {};
   template <> struct CFNumber_Traits< long      >: Basic_CFNumber_Traits< long,      ::kCFNumberLongType     > {};
   template <> struct CFNumber_Traits< long long >: Basic_CFNumber_Traits< long long, ::kCFNumberLongLongType > {};
   template <> struct CFNumber_Traits< float     >: Basic_CFNumber_Traits< float,     ::kCFNumberFloatType    > {};
   template <> struct CFNumber_Traits< double    >: Basic_CFNumber_Traits< double,    ::kCFNumberDoubleType   > {};

   template < class Closest >
   struct Converting_CFNumber_Traits
     {
      typedef Closest ClosestType;
     };

   template <> struct CFNumber_Traits< unsigned char      >: Converting_CFNumber_Traits< int       > {};
   template <> struct CFNumber_Traits< unsigned short     >: Converting_CFNumber_Traits< int       > {};
   template <> struct CFNumber_Traits< unsigned int       >: Converting_CFNumber_Traits< int       > {};
   template <> struct CFNumber_Traits< unsigned long      >: Converting_CFNumber_Traits< long long > {};
   template <> struct CFNumber_Traits< unsigned long long >: Converting_CFNumber_Traits< long long > {};

   template <> struct CFNumber_Traits< signed char        >: Converting_CFNumber_Traits< char      > {};
   template <> struct CFNumber_Traits< long double        >: Converting_CFNumber_Traits< double    > {};


   class CFNumberCreate_Failed {};
   Nucleus::Owned< CFNumberRef > CFNumberCreate( CFAllocatorRef allocator,
                                        CFNumberType   theType,
                                        const void *   valuePtr );
   
   template < class NumericType >
   Nucleus::Owned< CFNumberRef > CFNumberCreate( CFAllocatorRef allocator,
                                        NumericType    value )
     {
      typedef typename CFNumber_Traits< NumericType >::ClosestType ClosestType;
      ClosestType convertable = Nucleus::Convert< ClosestType >( value );
      return Nitrogen::CFNumberCreate( allocator,
                                       CFNumber_Traits< ClosestType >::type,
                                       &convertable );
     }
  
   template < class NumericType >
   Nucleus::Owned< CFNumberRef > CFNumberCreate( NumericType value )
     {
      return CFNumberCreate< NumericType >( kCFAllocatorDefault, value );
     }
   
   
   using ::CFNumberGetType;
   using ::CFNumberGetByteSize;
   inline bool CFNumberIsFloatType( CFNumberRef number )    { return ::CFNumberIsFloatType( number ); }
   
      
   class CFNumberGetValue_Failed {};
   
   template < class DesiredType >
   class CFNumberGetValue_FailedWithResult: public CFNumberGetValue_Failed
     {
      private:
         DesiredType result;
      
      public:
         CFNumberGetValue_FailedWithResult( DesiredType n )
           : result( n )
           {}
         
         DesiredType Get() const       { return result; }
         operator DesiredType() const  { return result; }
     };

   template < class NumericType >
   NumericType CFNumberGetValue( CFNumberRef number )
     {
      typedef typename CFNumber_Traits< NumericType >::ClosestType ClosestType;
      ClosestType convertable;
      bool success = CFNumberGetValue( number,
                                       CFNumber_Traits< ClosestType >::type,
                                       &convertable );
      
      NumericType result = Nucleus::Convert< NumericType >( convertable );
      
      if ( !success )
         throw CFNumberGetValue_FailedWithResult<NumericType>( result );
      
      return result;
     }

   
   inline CFComparisonResult CFNumberCompare( CFNumberRef   number,
                                              CFNumberRef   otherNumber )
     {
      return ::CFNumberCompare( number, otherNumber, 0 );
     }


   template < class NumericType >
   struct ConverterFromCFNumber: public std::unary_function< CFNumberRef, NumericType >
     {
      bool operator()( const CFNumberRef& in ) const
        {
         return Nitrogen::CFNumberGetValue< NumericType >( in );
        }
     };
  }

namespace Nucleus
  {   
   template<> struct Converter<          char,      Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<          char      >  {};
   template<> struct Converter<   signed char,      Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<   signed char      >  {};
   template<> struct Converter<   signed short,     Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<   signed short     >  {};
   template<> struct Converter<   signed int,       Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<   signed int       >  {};
   template<> struct Converter<   signed long,      Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<   signed long      >  {};
   template<> struct Converter<   signed long long, Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<   signed long long >  {};
   template<> struct Converter< unsigned char,      Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber< unsigned char      >  {};
   template<> struct Converter< unsigned short,     Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber< unsigned short     >  {};
   template<> struct Converter< unsigned int,       Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber< unsigned int       >  {};
   template<> struct Converter< unsigned long,      Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber< unsigned long      >  {};
   template<> struct Converter< unsigned long long, Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber< unsigned long long >  {};
   template<> struct Converter<          float,     Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<          float     >  {};
   template<> struct Converter<          double,    Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<          double    >  {};
   template<> struct Converter<     long double,    Nitrogen::CFNumberRef > : Nitrogen::ConverterFromCFNumber<     long double    >  {};
  }

namespace Nitrogen
  {

   template < class NumericType >
   struct ConverterToCFNumber: public std::unary_function< NumericType, Nucleus::Owned<Nitrogen::CFNumberRef> >
     {
      Nucleus::Owned<Nitrogen::CFNumberRef> operator()( const NumericType& in )
        {
         return CFNumberCreate( in );
        }
     };

  }

namespace Nucleus
  {
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,          char      > : Nitrogen::ConverterToCFNumber<          char      >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,   signed char      > : Nitrogen::ConverterToCFNumber<   signed char      >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,   signed short     > : Nitrogen::ConverterToCFNumber<   signed short     >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,   signed int       > : Nitrogen::ConverterToCFNumber<   signed int       >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,   signed long      > : Nitrogen::ConverterToCFNumber<   signed long      >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,   signed long long > : Nitrogen::ConverterToCFNumber<   signed long long >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>, unsigned char      > : Nitrogen::ConverterToCFNumber< unsigned char      >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>, unsigned short     > : Nitrogen::ConverterToCFNumber< unsigned short     >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>, unsigned int       > : Nitrogen::ConverterToCFNumber< unsigned int       >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>, unsigned long      > : Nitrogen::ConverterToCFNumber< unsigned long      >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>, unsigned long long > : Nitrogen::ConverterToCFNumber< unsigned long long >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,          float     > : Nitrogen::ConverterToCFNumber<          float     >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,          double    > : Nitrogen::ConverterToCFNumber<          double    >  {};
   template<> struct Converter< Nucleus::Owned<Nitrogen::CFNumberRef>,     long double    > : Nitrogen::ConverterToCFNumber<     long double    >  {};
  }

#endif
