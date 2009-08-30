// IDType.h

#ifndef NITROGEN_IDTYPE_H
#define NITROGEN_IDTYPE_H

namespace Nitrogen
  {

#ifndef JOSHUA_JURAN_EXPERIMENTAL

   template < class UnderlyingType, class T >
   struct IDTypeBlacklist
     {
      typedef T ConstructionForbidden;
      typedef T ConversionForbidden;
      typedef T ComparisonForbidden;
     };
   
   template < class UnderlyingType >
   struct IDTypeBlacklist< UnderlyingType, UnderlyingType >
     {
      class ConstructionForbidden;
      class ConversionForbidden;
      class ComparisonForbidden;
     };

#endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL

   template < class Tag, class UnderlyingType, UnderlyingType defaultValue = 0 >
   class IDType
     {
      private:
         UnderlyingType value;
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         
         template < class T > struct Blacklist :IDTypeBlacklist< UnderlyingType, T > {};
         
         // Forbidden constructors, unimplemented:
            IDType( typename Blacklist<          bool      >::ConstructionForbidden );
            IDType( typename Blacklist<          char      >::ConstructionForbidden );
            IDType( typename Blacklist<   signed char      >::ConstructionForbidden );
            IDType( typename Blacklist<   signed short     >::ConstructionForbidden );
            IDType( typename Blacklist<   signed int       >::ConstructionForbidden );
            IDType( typename Blacklist<   signed long      >::ConstructionForbidden );
            IDType( typename Blacklist<   signed long long >::ConstructionForbidden );
            IDType( typename Blacklist< unsigned char      >::ConstructionForbidden );
            IDType( typename Blacklist< unsigned short     >::ConstructionForbidden );
            IDType( typename Blacklist< unsigned int       >::ConstructionForbidden );
            IDType( typename Blacklist< unsigned long      >::ConstructionForbidden );
            IDType( typename Blacklist< unsigned long long >::ConstructionForbidden );
            
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
            friend void operator==( IDType, typename Blacklist<          bool      >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist<          char      >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist<   signed char      >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist<   signed short     >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist<   signed int       >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist<   signed long      >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist<   signed long long >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist< unsigned char      >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist< unsigned short     >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist< unsigned int       >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist< unsigned long      >::ComparisonForbidden );
            friend void operator==( IDType, typename Blacklist< unsigned long long >::ComparisonForbidden );

            friend void operator==( typename Blacklist<          bool      >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist<          char      >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist<   signed char      >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist<   signed short     >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist<   signed int       >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist<   signed long      >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist<   signed long long >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist< unsigned char      >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist< unsigned short     >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist< unsigned int       >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist< unsigned long      >::ComparisonForbidden, IDType );
            friend void operator==( typename Blacklist< unsigned long long >::ComparisonForbidden, IDType );

            friend void operator!=( IDType, typename Blacklist<          bool      >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist<          char      >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist<   signed char      >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist<   signed short     >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist<   signed int       >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist<   signed long      >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist<   signed long long >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist< unsigned char      >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist< unsigned short     >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist< unsigned int       >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist< unsigned long      >::ComparisonForbidden );
            friend void operator!=( IDType, typename Blacklist< unsigned long long >::ComparisonForbidden );

            friend void operator!=( typename Blacklist<          bool      >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist<          char      >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist<   signed char      >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist<   signed short     >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist<   signed int       >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist<   signed long      >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist<   signed long long >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist< unsigned char      >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist< unsigned short     >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist< unsigned int       >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist< unsigned long      >::ComparisonForbidden, IDType );
            friend void operator!=( typename Blacklist< unsigned long long >::ComparisonForbidden, IDType );
      
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
      
      public:
         IDType()                                              : value( defaultValue )    {}
         IDType( UnderlyingType theValue )                     : value( theValue )        {}
         
         operator UnderlyingType() const                       { return value; }

         static IDType Make( UnderlyingType v )                { return IDType( v ); }
         UnderlyingType Get() const                            { return value; }
         
      #ifndef JOSHUA_JURAN_EXPERIMENTAL
         friend bool operator==( IDType a, IDType b )          { return a.Get() == b.Get(); }
      
         friend bool operator==( IDType a, UnderlyingType b )  { return a.Get() == b; }
         friend bool operator==( UnderlyingType a, IDType b )  { return a == b.Get(); }

         friend bool operator!=( IDType a, IDType b )          { return a.Get() != b.Get(); }
      
         friend bool operator!=( IDType a, UnderlyingType b )  { return a.Get() != b; }
         friend bool operator!=( UnderlyingType a, IDType b )  { return a != b.Get(); }
      #endif  // #ifndef JOSHUA_JURAN_EXPERIMENTAL
      
     };
  }
   
#endif
