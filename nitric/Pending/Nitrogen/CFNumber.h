// CFNumber.h

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
   template <> struct OwnedDefaults< CFNumberRef > : OwnedDefaults<CFTypeRef>  {};
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
   Owned< CFNumberRef > CFNumberCreate( CFAllocatorRef allocator,
                                        CFNumberType   theType,
                                        const void *   valuePtr );
   
   template < class NumericType >
   Owned< CFNumberRef > CFNumberCreate( CFAllocatorRef allocator,
                                        NumericType    value )
     {
      typedef typename CFNumber_Traits< NumericType >::ClosestType ClosestType;
      ClosestType convertable = Convert< ClosestType >( value );
      return Nitrogen::CFNumberCreate( allocator,
                                       CFNumber_Traits< ClosestType >::type,
                                       &convertable );
     }
  
   template < class NumericType >
   Owned< CFNumberRef > CFNumberCreate( NumericType value )
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
      
      NumericType result = Convert< NumericType >( convertable );
      
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
   
   template<> struct Converter<          char,      CFNumberRef > : ConverterFromCFNumber<          char      >  {};
   template<> struct Converter<   signed char,      CFNumberRef > : ConverterFromCFNumber<   signed char      >  {};
   template<> struct Converter<   signed short,     CFNumberRef > : ConverterFromCFNumber<   signed short     >  {};
   template<> struct Converter<   signed int,       CFNumberRef > : ConverterFromCFNumber<   signed int       >  {};
   template<> struct Converter<   signed long,      CFNumberRef > : ConverterFromCFNumber<   signed long      >  {};
   template<> struct Converter<   signed long long, CFNumberRef > : ConverterFromCFNumber<   signed long long >  {};
   template<> struct Converter< unsigned char,      CFNumberRef > : ConverterFromCFNumber< unsigned char      >  {};
   template<> struct Converter< unsigned short,     CFNumberRef > : ConverterFromCFNumber< unsigned short     >  {};
   template<> struct Converter< unsigned int,       CFNumberRef > : ConverterFromCFNumber< unsigned int       >  {};
   template<> struct Converter< unsigned long,      CFNumberRef > : ConverterFromCFNumber< unsigned long      >  {};
   template<> struct Converter< unsigned long long, CFNumberRef > : ConverterFromCFNumber< unsigned long long >  {};
   template<> struct Converter<          float,     CFNumberRef > : ConverterFromCFNumber<          float     >  {};
   template<> struct Converter<          double,    CFNumberRef > : ConverterFromCFNumber<          double    >  {};
   template<> struct Converter<     long double,    CFNumberRef > : ConverterFromCFNumber<     long double    >  {};


   template < class NumericType >
   struct ConverterToCFNumber: public std::unary_function< NumericType, Owned<CFNumberRef> >
     {
      Owned<CFNumberRef> operator()( const NumericType& in )
        {
         return CFNumberCreate( in );
        }
     };

   template<> struct Converter< Owned<CFNumberRef>,          char      > : ConverterToCFNumber<          char      >  {};
   template<> struct Converter< Owned<CFNumberRef>,   signed char      > : ConverterToCFNumber<   signed char      >  {};
   template<> struct Converter< Owned<CFNumberRef>,   signed short     > : ConverterToCFNumber<   signed short     >  {};
   template<> struct Converter< Owned<CFNumberRef>,   signed int       > : ConverterToCFNumber<   signed int       >  {};
   template<> struct Converter< Owned<CFNumberRef>,   signed long      > : ConverterToCFNumber<   signed long      >  {};
   template<> struct Converter< Owned<CFNumberRef>,   signed long long > : ConverterToCFNumber<   signed long long >  {};
   template<> struct Converter< Owned<CFNumberRef>, unsigned char      > : ConverterToCFNumber< unsigned char      >  {};
   template<> struct Converter< Owned<CFNumberRef>, unsigned short     > : ConverterToCFNumber< unsigned short     >  {};
   template<> struct Converter< Owned<CFNumberRef>, unsigned int       > : ConverterToCFNumber< unsigned int       >  {};
   template<> struct Converter< Owned<CFNumberRef>, unsigned long      > : ConverterToCFNumber< unsigned long      >  {};
   template<> struct Converter< Owned<CFNumberRef>, unsigned long long > : ConverterToCFNumber< unsigned long long >  {};
   template<> struct Converter< Owned<CFNumberRef>,          float     > : ConverterToCFNumber<          float     >  {};
   template<> struct Converter< Owned<CFNumberRef>,          double    > : ConverterToCFNumber<          double    >  {};
   template<> struct Converter< Owned<CFNumberRef>,     long double    > : ConverterToCFNumber<     long double    >  {};
  }

#endif
