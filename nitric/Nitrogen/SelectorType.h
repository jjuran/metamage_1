// SelectorType.h

#ifndef NITROGEN_SELECTORTYPE_H
#define NITROGEN_SELECTORTYPE_H

#ifndef NITROGEN_ENUMERATIONISPROMOTABLE_H
#include "Nitrogen/EnumerationIsPromotable.h"
#endif

namespace Nitrogen
  {
   template < class UnderlyingType, class T >
   class SelectorTypeBlacklistDummyType
     {
      // Nothing implemented; this just placates the compiler when
      // it tries to instantiate functions it shouldn't instantiate.
      private:
         SelectorTypeBlacklistDummyType();
      
      public:
         operator UnderlyingType() const;
     };
   
   template < class UnderlyingType,
              class T,
              bool promotable = EnumerationIsPromotable< UnderlyingType, T >::promotable >
   struct SelectorTypeBlacklist;
   
   template < class UnderlyingType, class T >
   struct SelectorTypeBlacklist< UnderlyingType, T, false >
     {
      typedef T ConstructionForbidden;
      typedef SelectorTypeBlacklistDummyType< UnderlyingType, T > ConstructionAllowed;

      typedef T ConversionForbidden;
      typedef SelectorTypeBlacklistDummyType< UnderlyingType, T > ConversionAllowed;

      typedef T ComparisonForbidden;
      typedef SelectorTypeBlacklistDummyType< UnderlyingType, T > ComparisonAllowed;
     };

   template < class UnderlyingType, class T >
   struct SelectorTypeBlacklist< UnderlyingType, T, true >
     {
      class ConstructionForbidden;
      typedef T ConstructionAllowed;

      typedef T ConversionForbidden;
      typedef SelectorTypeBlacklistDummyType< UnderlyingType, T > ConversionAllowed;

      class ComparisonForbidden;
      typedef T ComparisonAllowed;
     };

   template < class UnderlyingType >
   struct SelectorTypeBlacklist< UnderlyingType, UnderlyingType, false >
     {
      class ConstructionForbidden;
      typedef UnderlyingType ConstructionAllowed;

      class ConversionForbidden;
      typedef UnderlyingType ConversionAllowed;

      class ComparisonForbidden;
      typedef UnderlyingType ComparisonAllowed;
     };

   template < class UnderlyingType >
   struct SelectorTypeBlacklist< UnderlyingType, UnderlyingType, true >
     {
      class ConstructionForbidden;
      typedef UnderlyingType ConstructionAllowed;

      class ConversionForbidden;
      typedef UnderlyingType ConversionAllowed;

      class ComparisonForbidden;
      typedef UnderlyingType ComparisonAllowed;
     };

   template < class Tag, class UnderlyingType, UnderlyingType defaultValue = 0 >
   class SelectorType
     {
      private:
         UnderlyingType value;
         
         template < class T > struct Blacklist :SelectorTypeBlacklist< UnderlyingType, T > {};
         
         // Dummy constructor, unimplemented
            template < class T > SelectorType( SelectorTypeBlacklistDummyType< UnderlyingType, T > );
         
         // Forbidden constructors, unimplemented:
            SelectorType( typename Blacklist<          bool      >::ConstructionForbidden );
            SelectorType( typename Blacklist<          char      >::ConstructionForbidden );
            SelectorType( typename Blacklist<   signed char      >::ConstructionForbidden );
            SelectorType( typename Blacklist<   signed short     >::ConstructionForbidden );
            SelectorType( typename Blacklist<   signed int       >::ConstructionForbidden );
            SelectorType( typename Blacklist<   signed long      >::ConstructionForbidden );
            SelectorType( typename Blacklist<   signed long long >::ConstructionForbidden );
            SelectorType( typename Blacklist< unsigned char      >::ConstructionForbidden );
            SelectorType( typename Blacklist< unsigned short     >::ConstructionForbidden );
            SelectorType( typename Blacklist< unsigned int       >::ConstructionForbidden );
            SelectorType( typename Blacklist< unsigned long      >::ConstructionForbidden );
            SelectorType( typename Blacklist< unsigned long long >::ConstructionForbidden );
            
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
            friend void operator==( SelectorType, typename Blacklist<          bool      >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist<          char      >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist<   signed char      >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist<   signed short     >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist<   signed int       >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist<   signed long      >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist<   signed long long >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist< unsigned char      >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist< unsigned short     >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist< unsigned int       >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist< unsigned long      >::ComparisonForbidden );
            friend void operator==( SelectorType, typename Blacklist< unsigned long long >::ComparisonForbidden );

            friend void operator==( typename Blacklist<          bool      >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist<          char      >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist<   signed char      >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist<   signed short     >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist<   signed int       >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist<   signed long      >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist<   signed long long >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist< unsigned char      >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist< unsigned short     >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist< unsigned int       >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist< unsigned long      >::ComparisonForbidden, SelectorType );
            friend void operator==( typename Blacklist< unsigned long long >::ComparisonForbidden, SelectorType );

            friend void operator!=( SelectorType, typename Blacklist<          bool      >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist<          char      >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist<   signed char      >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist<   signed short     >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist<   signed int       >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist<   signed long      >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist<   signed long long >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist< unsigned char      >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist< unsigned short     >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist< unsigned int       >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist< unsigned long      >::ComparisonForbidden );
            friend void operator!=( SelectorType, typename Blacklist< unsigned long long >::ComparisonForbidden );

            friend void operator!=( typename Blacklist<          bool      >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist<          char      >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist<   signed char      >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist<   signed short     >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist<   signed int       >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist<   signed long      >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist<   signed long long >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist< unsigned char      >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist< unsigned short     >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist< unsigned int       >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist< unsigned long      >::ComparisonForbidden, SelectorType );
            friend void operator!=( typename Blacklist< unsigned long long >::ComparisonForbidden, SelectorType );

      public:
         SelectorType()                                              : value( defaultValue )    {}

         static SelectorType Make( UnderlyingType v )                { return SelectorType( v ); }
         UnderlyingType Get() const                                  { return value; }
         
         friend bool operator==( SelectorType a, SelectorType b )    { return a.Get() == b.Get(); }
         friend bool operator!=( SelectorType a, SelectorType b )    { return a.Get() != b.Get(); }
      
         SelectorType( typename Blacklist<          bool      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist<          char      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist<   signed char      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist<   signed short     >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist<   signed int       >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist<   signed long      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist<   signed long long >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist< unsigned char      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist< unsigned short     >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist< unsigned int       >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist< unsigned long      >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         SelectorType( typename Blacklist< unsigned long long >::ConstructionAllowed v ) : value( static_cast<UnderlyingType>( v ) )  {}
         
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

         friend bool operator==( SelectorType a, typename Blacklist<          bool      >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist<          char      >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist<   signed char      >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist<   signed short     >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist<   signed int       >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist<   signed long      >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist<   signed long long >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist< unsigned char      >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist< unsigned short     >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist< unsigned int       >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist< unsigned long      >::ComparisonAllowed b )    { return a == SelectorType(b); }
         friend bool operator==( SelectorType a, typename Blacklist< unsigned long long >::ComparisonAllowed b )    { return a == SelectorType(b); }

         friend bool operator==( typename Blacklist<          bool      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist<          char      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist<   signed char      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist<   signed short     >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist<   signed int       >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist<   signed long      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist<   signed long long >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist< unsigned char      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist< unsigned short     >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist< unsigned int       >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist< unsigned long      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }
         friend bool operator==( typename Blacklist< unsigned long long >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) == b; }

         friend bool operator!=( SelectorType a, typename Blacklist<          bool      >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist<          char      >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist<   signed char      >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist<   signed short     >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist<   signed int       >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist<   signed long      >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist<   signed long long >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist< unsigned char      >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist< unsigned short     >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist< unsigned int       >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist< unsigned long      >::ComparisonAllowed b )    { return a != SelectorType(b); }
         friend bool operator!=( SelectorType a, typename Blacklist< unsigned long long >::ComparisonAllowed b )    { return a != SelectorType(b); }

         friend bool operator!=( typename Blacklist<          bool      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist<          char      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist<   signed char      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist<   signed short     >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist<   signed int       >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist<   signed long      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist<   signed long long >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist< unsigned char      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist< unsigned short     >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist< unsigned int       >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist< unsigned long      >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
         friend bool operator!=( typename Blacklist< unsigned long long >::ComparisonAllowed a, SelectorType b )    { return SelectorType(a) != b; }
     };
  }

#endif
