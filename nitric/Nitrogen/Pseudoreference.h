// Pseudoreference.h

#ifndef NITROGEN_PSEUDOREFERENCE_H
#define NITROGEN_PSEUDOREFERENCE_H

#ifndef NITROGEN_FUNCTIONTRAITS_H
#include "Nitrogen/FunctionTraits.h"
#endif
#ifndef NITROGEN_REFERENCETRAITS_H
#include "Nitrogen/ReferenceTraits.h"
#endif
#ifndef NITROGEN_VALUETRAITS_H
#include "Nitrogen/ValueTraits.h"
#endif
#ifndef NITROGEN_PSEUDOPOINTER_H
#include "Nitrogen/Pseudopointer.h"
#endif
#ifndef NITROGEN_CONVERT_H
#include "Nitrogen/Convert.h"
#endif

namespace Nitrogen
  {
   template < class Context, class Value, class Getter, Getter get > class ConstPseudoreference;

   template < class Context, class Value, class Getter, Getter get, class Setter, Setter set > class Pseudoreference;

   template < class ContextType,
              class ValueType,
              class GetterType,
              GetterType get >
   class ConstPseudoreference
     {
      public:
         typedef ContextType     Context;
         typedef ValueType       Value;
         typedef GetterType      Getter;
         
         typedef typename FunctionTraits< Getter >::Result           ValueResult;
         
         typedef ConstPseudoreference< Context, Value, Getter, get > Reference;
         typedef ConstPseudoreference< Context, Value, Getter, get > ConstReference;

         typedef Pseudopointer< Reference >                          Pointer;
         typedef Pseudopointer< ConstReference >                     ConstPointer;
         
      private:
         const Context context;
      
      public:         
         ConstPseudoreference()                                                         : context()              {}
         explicit ConstPseudoreference( Context theContext )                            : context( theContext )  {}
          
         Pointer operator&() const                                                      { return Pointer( context ); }

         ValueResult Get() const                                                        { return get(context);   }
         
         operator ValueResult() const                                                   { return Get(); }
     };

   template < class ContextType,
              class ValueType,
              class GetterType,
              GetterType get,
              class SetterType,
              SetterType set >
   class Pseudoreference
     {
      public:
         typedef ContextType     Context;
         typedef ValueType       Value;
         typedef GetterType      Getter;
         typedef SetterType      Setter;
         
         typedef typename FunctionTraits< Getter >::Result      ValueResult;
         typedef typename FunctionTraits< Setter >::Parameter1  ValueParameter;
         
         typedef Pseudoreference     < Context, Value, Getter, get, Setter, set > Reference;
         typedef ConstPseudoreference< Context, Value, Getter, get >              ConstReference;

         typedef Pseudopointer< Reference >                                       Pointer;
         typedef Pseudopointer< ConstReference >                                  ConstPointer;
         
      private:
         const Context context;
      
      public:         
         Pseudoreference()                                                              : context()              {}
         explicit Pseudoreference( Context theContext )                                 : context( theContext )  {}
          
         Pointer operator&() const                                                      { return Pointer( context ); }
 
         operator ConstReference() const                                                { return ConstReference( context ); }

         ValueResult Get() const                                                        { return get(context);   }
         void Set( ValueParameter value ) const                                         { set( context, value ); }
         
         operator ValueResult() const                                                   { return Get(); }
         const Pseudoreference& operator=( ValueParameter value ) const                 { Set( value ); return *this; }
         
         const Pseudoreference& operator=( const Pseudoreference& rhs ) const           { Set( rhs.Get() ); return *this; }
         
         template < class T > const Pseudoreference& operator+=( const T& rhs ) const   { Set( Get() + rhs ); return *this; }
         template < class T > const Pseudoreference& operator-=( const T& rhs ) const   { Set( Get() - rhs ); return *this; }
         template < class T > const Pseudoreference& operator*=( const T& rhs ) const   { Set( Get() * rhs ); return *this; }
         template < class T > const Pseudoreference& operator/=( const T& rhs ) const   { Set( Get() / rhs ); return *this; }
         template < class T > const Pseudoreference& operator%=( const T& rhs ) const   { Set( Get() % rhs ); return *this; }

         template < class T > const Pseudoreference& operator&=( const T& rhs ) const   { Set( Get() & rhs ); return *this; }
         template < class T > const Pseudoreference& operator|=( const T& rhs ) const   { Set( Get() | rhs ); return *this; }
         template < class T > const Pseudoreference& operator^=( const T& rhs ) const   { Set( Get() ^ rhs ); return *this; }

         template < class T > const Pseudoreference& operator<<=( const T& rhs ) const  { Set( Get() << rhs ); return *this; }
         template < class T > const Pseudoreference& operator>>=( const T& rhs ) const  { Set( Get() >> rhs ); return *this; }
     };



   template < class Context, class Value, class Getter, Getter get >
   struct ReferenceTraits< ConstPseudoreference< Context, Value, Getter, get > >
     {
      typedef ConstPseudoreference< Context, Value, Getter, get > Reference;
      
      typedef typename Reference::Value           Value;
      typedef typename Reference::Pointer         Pointer;
      typedef typename Reference::ConstReference  ConstReference;
      typedef typename Reference::ConstPointer    ConstPointer;
     };

   template < class Context, class Value, class Getter, Getter get >
   struct ConvertInputTraits< ConstPseudoreference< Context, Value, Getter, get > >
     {
      typedef Value ConverterInputType;
     };
   
   template < class Context, class Value, class Getter, Getter get, class Setter, Setter set >
   struct ReferenceTraits< Pseudoreference< Context, Value, Getter, get, Setter, set > >
     {
      typedef Pseudoreference< Context, Value, Getter, get, Setter, set > Reference;
      
      typedef typename Reference::Value           Value;
      typedef typename Reference::Pointer         Pointer;
      typedef typename Reference::ConstReference  ConstReference;
      typedef typename Reference::ConstPointer    ConstPointer;
     };

   template < class Context, class Value, class Getter, Getter get, class Setter, Setter set >
   struct ConvertInputTraits< Pseudoreference< Context, Value, Getter, get, Setter, set > >
     {
      typedef Value ConverterInputType;
     };

   template < class Context, class Value, class Getter, Getter get >
   struct ValueTraits< ConstPseudoreference< Context, Value, Getter, get > >
     {
      typedef ConstPseudoreference< Context, Value, Getter, get > Reference;
      
      typedef typename Reference::Value           Value;
      typedef typename Reference::Pointer         Pointer;
      typedef typename Reference::ConstReference  ConstReference;
      typedef typename Reference::ConstPointer    ConstPointer;
     };

   template < class Context, class Value, class Getter, Getter get, class Setter, Setter set >
   struct ValueTraits< Pseudoreference< Context, Value, Getter, get, Setter, set > >
     {
      typedef Pseudoreference< Context, Value, Getter, get, Setter, set > Reference;
      
      typedef typename Reference::Value           Value;
      typedef typename Reference::Pointer         Pointer;
      typedef typename Reference::ConstReference  ConstReference;
      typedef typename Reference::ConstPointer    ConstPointer;
     };
  }

#endif
