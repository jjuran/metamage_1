// FlagType.h

#ifndef NITROGEN_FLAGTYPE_H
#define NITROGEN_FLAGTYPE_H

#ifndef NITROGEN_ENUMERATIONISPROMOTABLE_H
#include "Nitrogen/EnumerationIsPromotable.h"
#endif

namespace Nitrogen
  {

#ifndef JOSHUA_JURAN_EXPERIMENTAL

   template < class UnderlyingType,
              class T,
              bool promotable = EnumerationIsPromotable< UnderlyingType, T >::promotable >
   struct FlagTypeBlacklist;

   template < class UnderlyingType, class T >
   struct FlagTypeBlacklist< UnderlyingType, T, false >
     {
      typedef T ConstructionForbidden;
      class ConstructionAllowed {};

      typedef T ConversionForbidden;
      class ConversionAllowed {};

      typedef T ComparisonForbidden;
      class ComparisonAllowed {};

      typedef T BitwiseForbidden;
      class BitwiseAllowed {};
      
      // not implemented:
         friend bool operator==( UnderlyingType, ComparisonAllowed );
         friend bool operator==( ComparisonAllowed, UnderlyingType );
         friend bool operator!=( UnderlyingType, ComparisonAllowed );
         friend bool operator!=( ComparisonAllowed, UnderlyingType );

         friend UnderlyingType operator&( UnderlyingType, BitwiseAllowed );
         friend UnderlyingType operator&( BitwiseAllowed, UnderlyingType );
         friend UnderlyingType operator|( UnderlyingType, BitwiseAllowed );
         friend UnderlyingType operator|( BitwiseAllowed, UnderlyingType );
         friend UnderlyingType operator^( UnderlyingType, BitwiseAllowed );
         friend UnderlyingType operator^( BitwiseAllowed, UnderlyingType );
     };
   
   template < class UnderlyingType, class T >
   struct FlagTypeBlacklist< UnderlyingType, T, true >
     {
      class ConstructionForbidden;
      typedef T ConstructionAllowed;

      typedef T ConversionForbidden;
      class ConversionAllowed {};

      class ComparisonForbidden;
      typedef T ComparisonAllowed;

      class BitwiseForbidden;
      typedef T BitwiseAllowed;
     };
   
   template < class UnderlyingType >
   struct FlagTypeBlacklist< UnderlyingType, UnderlyingType, false >
     {
      class ConstructionForbidden;
      typedef UnderlyingType ConstructionAllowed;

      class ConversionForbidden;
      typedef UnderlyingType ConversionAllowed;

      class ComparisonForbidden;
      typedef UnderlyingType ComparisonAllowed;

      class BitwiseForbidden;
      typedef UnderlyingType BitwiseAllowed;
     };
   
   template < class UnderlyingType >
   struct FlagTypeBlacklist< UnderlyingType, UnderlyingType, true >
     {
      class ConstructionForbidden;
      typedef UnderlyingType ConstructionAllowed;

      class ConversionForbidden;
      typedef UnderlyingType ConversionAllowed;

      class ComparisonForbidden;
      typedef UnderlyingType ComparisonAllowed;

      class BitwiseForbidden;
      typedef UnderlyingType BitwiseAllowed;
     };

   template < class UnderlyingType >
   struct FlagTypeBlacklist< UnderlyingType, bool, false >
     {
      typedef bool ConstructionForbidden;
      class ConstructionAllowed {};

      class ConversionForbidden;
      typedef bool ConversionAllowed;

      typedef bool ComparisonForbidden;
      class ComparisonAllowed {};

      typedef bool BitwiseForbidden;
      class BitwiseAllowed {};

      // not implemented:
         friend bool operator==( UnderlyingType, ComparisonAllowed );
         friend bool operator==( ComparisonAllowed, UnderlyingType );
         friend bool operator!=( UnderlyingType, ComparisonAllowed );
         friend bool operator!=( ComparisonAllowed, UnderlyingType );

         friend UnderlyingType operator&( UnderlyingType, BitwiseAllowed );
         friend UnderlyingType operator&( BitwiseAllowed, UnderlyingType );
         friend UnderlyingType operator|( UnderlyingType, BitwiseAllowed );
         friend UnderlyingType operator|( BitwiseAllowed, UnderlyingType );
         friend UnderlyingType operator^( UnderlyingType, BitwiseAllowed );
         friend UnderlyingType operator^( BitwiseAllowed, UnderlyingType );
     };
   
   template <>
   struct FlagTypeBlacklist< bool, bool, false >
     {
      class ConstructionForbidden;
      typedef bool ConstructionAllowed;

      class ConversionForbidden;
      typedef bool ConversionAllowed;

      class ComparisonForbidden;
      typedef bool ComparisonAllowed;

      class BitwiseForbidden;
      typedef bool BitwiseAllowed;
     };

#endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL

   template < class Tag, class UnderlyingType, UnderlyingType defaultValue = 0 >
   class FlagType
     {
      private:
         UnderlyingType value;
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         
         template < class T > struct Blacklist :FlagTypeBlacklist< UnderlyingType, T > {};
         
         // Forbidden constructors, unimplemented:
            FlagType( typename Blacklist<          bool      >::ConstructionForbidden );
            FlagType( typename Blacklist<          char      >::ConstructionForbidden );
            FlagType( typename Blacklist<   signed char      >::ConstructionForbidden );
            FlagType( typename Blacklist<   signed short     >::ConstructionForbidden );
            FlagType( typename Blacklist<   signed int       >::ConstructionForbidden );
            FlagType( typename Blacklist<   signed long      >::ConstructionForbidden );
            FlagType( typename Blacklist<   signed long long >::ConstructionForbidden );
            FlagType( typename Blacklist< unsigned char      >::ConstructionForbidden );
            FlagType( typename Blacklist< unsigned short     >::ConstructionForbidden );
            FlagType( typename Blacklist< unsigned int       >::ConstructionForbidden );
            FlagType( typename Blacklist< unsigned long      >::ConstructionForbidden );
            FlagType( typename Blacklist< unsigned long long >::ConstructionForbidden );
            
         // Forbidden conversions, unimplemented:
            // CodeWarrior 7 has a bug which allows conversion operators to be called even
            // when they're private.  So it will give link errors, not compile errors, for these.
            operator typename Blacklist<          bool      >::ConversionForbidden() const;
            operator typename Blacklist<          char      >::ConversionForbidden() const;
            operator typename Blacklist<   signed char      >::ConversionForbidden() const;
            operator typename Blacklist<   signed short     >::ConversionForbidden() const;
            operator typename Blacklist<   signed int       >::ConversionForbidden() const;
            operator typename Blacklist<   signed long      >::ConversionForbidden() const;
            operator typename Blacklist<   signed long long >::ConversionForbidden() const;
            operator typename Blacklist< unsigned char      >::ConversionForbidden() const;
            operator typename Blacklist< unsigned short     >::ConversionForbidden() const;
            operator typename Blacklist< unsigned int       >::ConversionForbidden() const;
            operator typename Blacklist< unsigned long      >::ConversionForbidden() const;
            operator typename Blacklist< unsigned long long >::ConversionForbidden() const;

         // Forbidden comparisons, unimplemented:
            friend void operator==( FlagType, typename Blacklist<          bool      >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist<          char      >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist<   signed char      >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist<   signed short     >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist<   signed int       >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist<   signed long      >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist<   signed long long >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist< unsigned char      >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist< unsigned short     >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist< unsigned int       >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist< unsigned long      >::ComparisonForbidden );
            friend void operator==( FlagType, typename Blacklist< unsigned long long >::ComparisonForbidden );

            friend void operator==( typename Blacklist<          bool      >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist<          char      >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist<   signed char      >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist<   signed short     >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist<   signed int       >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist<   signed long      >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist<   signed long long >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist< unsigned char      >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist< unsigned short     >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist< unsigned int       >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist< unsigned long      >::ComparisonForbidden, FlagType );
            friend void operator==( typename Blacklist< unsigned long long >::ComparisonForbidden, FlagType );

            friend void operator!=( FlagType, typename Blacklist<          bool      >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist<          char      >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist<   signed char      >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist<   signed short     >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist<   signed int       >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist<   signed long      >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist<   signed long long >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist< unsigned char      >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist< unsigned short     >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist< unsigned int       >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist< unsigned long      >::ComparisonForbidden );
            friend void operator!=( FlagType, typename Blacklist< unsigned long long >::ComparisonForbidden );

            friend void operator!=( typename Blacklist<          bool      >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist<          char      >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist<   signed char      >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist<   signed short     >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist<   signed int       >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist<   signed long      >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist<   signed long long >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist< unsigned char      >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist< unsigned short     >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist< unsigned int       >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist< unsigned long      >::ComparisonForbidden, FlagType );
            friend void operator!=( typename Blacklist< unsigned long long >::ComparisonForbidden, FlagType );

         // Forbidden bitwise operations, unimplemented:
            friend void operator|( FlagType, typename Blacklist<          bool      >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist<          char      >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist<   signed char      >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist<   signed short     >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist<   signed int       >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist<   signed long      >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist<   signed long long >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist< unsigned char      >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist< unsigned short     >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist< unsigned int       >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist< unsigned long      >::BitwiseForbidden );
            friend void operator|( FlagType, typename Blacklist< unsigned long long >::BitwiseForbidden );

            friend void operator|( typename Blacklist<          bool      >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist<          char      >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist<   signed char      >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist<   signed short     >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist<   signed int       >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist<   signed long      >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist<   signed long long >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist< unsigned char      >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist< unsigned short     >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist< unsigned int       >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist< unsigned long      >::BitwiseForbidden, FlagType );
            friend void operator|( typename Blacklist< unsigned long long >::BitwiseForbidden, FlagType );

            friend void operator&( FlagType, typename Blacklist<          bool      >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist<          char      >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist<   signed char      >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist<   signed short     >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist<   signed int       >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist<   signed long      >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist<   signed long long >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist< unsigned char      >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist< unsigned short     >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist< unsigned int       >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist< unsigned long      >::BitwiseForbidden );
            friend void operator&( FlagType, typename Blacklist< unsigned long long >::BitwiseForbidden );

            friend void operator&( typename Blacklist<          bool      >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist<          char      >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist<   signed char      >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist<   signed short     >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist<   signed int       >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist<   signed long      >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist<   signed long long >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist< unsigned char      >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist< unsigned short     >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist< unsigned int       >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist< unsigned long      >::BitwiseForbidden, FlagType );
            friend void operator&( typename Blacklist< unsigned long long >::BitwiseForbidden, FlagType );

            friend void operator^( FlagType, typename Blacklist<          bool      >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist<          char      >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist<   signed char      >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist<   signed short     >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist<   signed int       >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist<   signed long      >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist<   signed long long >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist< unsigned char      >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist< unsigned short     >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist< unsigned int       >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist< unsigned long      >::BitwiseForbidden );
            friend void operator^( FlagType, typename Blacklist< unsigned long long >::BitwiseForbidden );

            friend void operator^( typename Blacklist<          bool      >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist<          char      >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist<   signed char      >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist<   signed short     >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist<   signed int       >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist<   signed long      >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist<   signed long long >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist< unsigned char      >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist< unsigned short     >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist< unsigned int       >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist< unsigned long      >::BitwiseForbidden, FlagType );
            friend void operator^( typename Blacklist< unsigned long long >::BitwiseForbidden, FlagType );

         // Forbidden bitwise assignments, unimplemented:
            void operator|=( typename Blacklist<          bool      >::BitwiseForbidden );
            void operator|=( typename Blacklist<          char      >::BitwiseForbidden );
            void operator|=( typename Blacklist<   signed char      >::BitwiseForbidden );
            void operator|=( typename Blacklist<   signed short     >::BitwiseForbidden );
            void operator|=( typename Blacklist<   signed int       >::BitwiseForbidden );
            void operator|=( typename Blacklist<   signed long      >::BitwiseForbidden );
            void operator|=( typename Blacklist<   signed long long >::BitwiseForbidden );
            void operator|=( typename Blacklist< unsigned char      >::BitwiseForbidden );
            void operator|=( typename Blacklist< unsigned short     >::BitwiseForbidden );
            void operator|=( typename Blacklist< unsigned int       >::BitwiseForbidden );
            void operator|=( typename Blacklist< unsigned long      >::BitwiseForbidden );
            void operator|=( typename Blacklist< unsigned long long >::BitwiseForbidden );

            void operator&=( typename Blacklist<          bool      >::BitwiseForbidden );
            void operator&=( typename Blacklist<          char      >::BitwiseForbidden );
            void operator&=( typename Blacklist<   signed char      >::BitwiseForbidden );
            void operator&=( typename Blacklist<   signed short     >::BitwiseForbidden );
            void operator&=( typename Blacklist<   signed int       >::BitwiseForbidden );
            void operator&=( typename Blacklist<   signed long      >::BitwiseForbidden );
            void operator&=( typename Blacklist<   signed long long >::BitwiseForbidden );
            void operator&=( typename Blacklist< unsigned char      >::BitwiseForbidden );
            void operator&=( typename Blacklist< unsigned short     >::BitwiseForbidden );
            void operator&=( typename Blacklist< unsigned int       >::BitwiseForbidden );
            void operator&=( typename Blacklist< unsigned long      >::BitwiseForbidden );
            void operator&=( typename Blacklist< unsigned long long >::BitwiseForbidden );

            void operator^=( typename Blacklist<          bool      >::BitwiseForbidden );
            void operator^=( typename Blacklist<          char      >::BitwiseForbidden );
            void operator^=( typename Blacklist<   signed char      >::BitwiseForbidden );
            void operator^=( typename Blacklist<   signed short     >::BitwiseForbidden );
            void operator^=( typename Blacklist<   signed int       >::BitwiseForbidden );
            void operator^=( typename Blacklist<   signed long      >::BitwiseForbidden );
            void operator^=( typename Blacklist<   signed long long >::BitwiseForbidden );
            void operator^=( typename Blacklist< unsigned char      >::BitwiseForbidden );
            void operator^=( typename Blacklist< unsigned short     >::BitwiseForbidden );
            void operator^=( typename Blacklist< unsigned int       >::BitwiseForbidden );
            void operator^=( typename Blacklist< unsigned long      >::BitwiseForbidden );
            void operator^=( typename Blacklist< unsigned long long >::BitwiseForbidden );
      
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
      
      public:
         FlagType()                                            : value( defaultValue )    {}
      
      #ifdef JOSHUA_JURAN_EXPERIMENTAL
         
         FlagType( UnderlyingType theValue )                   : value( theValue )        {}
         operator UnderlyingType() const                       { return value; }
         
      #endif  // #ifdef JOSHUA_JURAN_EXPERIMENTAL
      
         static FlagType Make( UnderlyingType v )              { return FlagType( v ); }
         UnderlyingType Get() const                            { return value; }
         
         bool operator!() const                                { return !value; }
         FlagType operator~() const                            { return FlagType( ~value ); }
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
      
         friend bool operator==( FlagType a, FlagType b )      { return a.Get() == b.Get(); }
         friend bool operator!=( FlagType a, FlagType b )      { return a.Get() != b.Get(); }

         friend FlagType operator|( FlagType a, FlagType b )   { return static_cast<UnderlyingType>( a.Get() | b.Get() ); }
         friend FlagType operator&( FlagType a, FlagType b )   { return static_cast<UnderlyingType>( a.Get() & b.Get() ); }
         friend FlagType operator^( FlagType a, FlagType b )   { return static_cast<UnderlyingType>( a.Get() ^ b.Get() ); }

      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
      
         FlagType& operator|=( FlagType b )                    { value |= b.Get(); return *this; }
         FlagType& operator&=( FlagType b )                    { value &= b.Get(); return *this; }
         FlagType& operator^=( FlagType b )                    { value ^= b.Get(); return *this; }
      
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
      
         // Allowed converting constructors:
            FlagType( typename Blacklist<          bool      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist<          char      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist<   signed char      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist<   signed short     >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist<   signed int       >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist<   signed long      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist<   signed long long >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist< unsigned char      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist< unsigned short     >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist< unsigned int       >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist< unsigned long      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
            FlagType( typename Blacklist< unsigned long long >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         
         // Allowed conversions:
            operator typename Blacklist<          bool      >::ConversionAllowed() const    { return value; }
            operator typename Blacklist<          char      >::ConversionAllowed() const    { return value; }
            operator typename Blacklist<   signed char      >::ConversionAllowed() const    { return value; }
            operator typename Blacklist<   signed short     >::ConversionAllowed() const    { return value; }
            operator typename Blacklist<   signed int       >::ConversionAllowed() const    { return value; }
            operator typename Blacklist<   signed long      >::ConversionAllowed() const    { return value; }
            operator typename Blacklist<   signed long long >::ConversionAllowed() const    { return value; }
            operator typename Blacklist< unsigned char      >::ConversionAllowed() const    { return value; }
            operator typename Blacklist< unsigned short     >::ConversionAllowed() const    { return value; }
            operator typename Blacklist< unsigned int       >::ConversionAllowed() const    { return value; }
            operator typename Blacklist< unsigned long      >::ConversionAllowed() const    { return value; }
            operator typename Blacklist< unsigned long long >::ConversionAllowed() const    { return value; }

         // Allowed comparisons:
            friend bool operator==( FlagType a, typename Blacklist<          bool      >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist<          char      >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist<   signed char      >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist<   signed short     >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist<   signed int       >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist<   signed long      >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist<   signed long long >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist< unsigned char      >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist< unsigned short     >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist< unsigned int       >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist< unsigned long      >::ComparisonAllowed b )    { return a.Get() == b; }
            friend bool operator==( FlagType a, typename Blacklist< unsigned long long >::ComparisonAllowed b )    { return a.Get() == b; }

            friend bool operator==( typename Blacklist<          bool      >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist<          char      >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist<   signed char      >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist<   signed short     >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist<   signed int       >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist<   signed long      >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist<   signed long long >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist< unsigned char      >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist< unsigned short     >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist< unsigned int       >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist< unsigned long      >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }
            friend bool operator==( typename Blacklist< unsigned long long >::ComparisonAllowed a, FlagType b )    { return a == b.Get(); }

            friend bool operator!=( FlagType a, typename Blacklist<          bool      >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist<          char      >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist<   signed char      >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist<   signed short     >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist<   signed int       >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist<   signed long      >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist<   signed long long >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist< unsigned char      >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist< unsigned short     >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist< unsigned int       >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist< unsigned long      >::ComparisonAllowed b )    { return a.Get() != b; }
            friend bool operator!=( FlagType a, typename Blacklist< unsigned long long >::ComparisonAllowed b )    { return a.Get() != b; }

            friend bool operator!=( typename Blacklist<          bool      >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist<          char      >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist<   signed char      >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist<   signed short     >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist<   signed int       >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist<   signed long      >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist<   signed long long >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist< unsigned char      >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist< unsigned short     >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist< unsigned int       >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist< unsigned long      >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }
            friend bool operator!=( typename Blacklist< unsigned long long >::ComparisonAllowed a, FlagType b )    { return a != b.Get(); }

         // Allowed bitwise operators:
            friend FlagType operator|( FlagType a, typename Blacklist<          bool      >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist<          char      >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist<   signed char      >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist<   signed short     >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist<   signed int       >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist<   signed long      >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist<   signed long long >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist< unsigned char      >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist< unsigned short     >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist< unsigned int       >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist< unsigned long      >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }
            friend FlagType operator|( FlagType a, typename Blacklist< unsigned long long >::BitwiseAllowed b )    { return FlagType( a.Get() | b ); }

            friend FlagType operator|( typename Blacklist<          bool      >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist<          char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist<   signed char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist<   signed short     >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist<   signed int       >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist<   signed long      >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist<   signed long long >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist< unsigned char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist< unsigned short     >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist< unsigned int       >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist< unsigned long      >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }
            friend FlagType operator|( typename Blacklist< unsigned long long >::BitwiseAllowed a, FlagType b )    { return FlagType( a | b.Get() ); }

            friend FlagType operator&( FlagType a, typename Blacklist<          bool      >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist<          char      >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist<   signed char      >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist<   signed short     >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist<   signed int       >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist<   signed long      >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist<   signed long long >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist< unsigned char      >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist< unsigned short     >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist< unsigned int       >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist< unsigned long      >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }
            friend FlagType operator&( FlagType a, typename Blacklist< unsigned long long >::BitwiseAllowed b )    { return FlagType( a.Get() & b ); }

            friend FlagType operator&( typename Blacklist<          bool      >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist<          char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist<   signed char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist<   signed short     >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist<   signed int       >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist<   signed long      >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist<   signed long long >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist< unsigned char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist< unsigned short     >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist< unsigned int       >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist< unsigned long      >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }
            friend FlagType operator&( typename Blacklist< unsigned long long >::BitwiseAllowed a, FlagType b )    { return FlagType( a & b.Get() ); }

            friend FlagType operator^( FlagType a, typename Blacklist<          bool      >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist<          char      >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist<   signed char      >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist<   signed short     >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist<   signed int       >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist<   signed long      >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist<   signed long long >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist< unsigned char      >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist< unsigned short     >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist< unsigned int       >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist< unsigned long      >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }
            friend FlagType operator^( FlagType a, typename Blacklist< unsigned long long >::BitwiseAllowed b )    { return FlagType( a.Get() ^ b ); }

            friend FlagType operator^( typename Blacklist<          bool      >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist<          char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist<   signed char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist<   signed short     >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist<   signed int       >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist<   signed long      >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist<   signed long long >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist< unsigned char      >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist< unsigned short     >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist< unsigned int       >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist< unsigned long      >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }
            friend FlagType operator^( typename Blacklist< unsigned long long >::BitwiseAllowed a, FlagType b )    { return FlagType( a ^ b.Get() ); }

         // Allowed bitwise assignments:
            FlagType& operator|=( typename Blacklist<          bool      >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist<          char      >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist<   signed char      >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist<   signed short     >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist<   signed int       >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist<   signed long      >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist<   signed long long >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist< unsigned char      >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist< unsigned short     >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist< unsigned int       >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist< unsigned long      >::BitwiseAllowed b )     { value |= b; return *this; }
            FlagType& operator|=( typename Blacklist< unsigned long long >::BitwiseAllowed b )     { value |= b; return *this; }

            FlagType& operator&=( typename Blacklist<          bool      >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist<          char      >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist<   signed char      >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist<   signed short     >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist<   signed int       >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist<   signed long      >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist<   signed long long >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist< unsigned char      >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist< unsigned short     >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist< unsigned int       >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist< unsigned long      >::BitwiseAllowed b )     { value &= b; return *this; }
            FlagType& operator&=( typename Blacklist< unsigned long long >::BitwiseAllowed b )     { value &= b; return *this; }

            FlagType& operator^=( typename Blacklist<          bool      >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist<          char      >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist<   signed char      >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist<   signed short     >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist<   signed int       >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist<   signed long      >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist<   signed long long >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist< unsigned char      >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist< unsigned short     >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist< unsigned int       >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist< unsigned long      >::BitwiseAllowed b )     { value ^= b; return *this; }
            FlagType& operator^=( typename Blacklist< unsigned long long >::BitwiseAllowed b )     { value ^= b; return *this; }
         
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
         
     };
  }

#endif
