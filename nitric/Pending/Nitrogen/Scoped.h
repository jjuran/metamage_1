// Scoped.h

#ifndef NITROGEN_SCOPED_H
#define NITROGEN_SCOPED_H

#ifndef NITROGEN_REFERENCETRAITS_H
#include "Nitrogen/ReferenceTraits.h"
#endif
#ifndef NITROGEN_CONVERT_H
#include "Nitrogen/Convert.h"
#endif

namespace Nitrogen
  {
   template < class Reference >
   class Scoped
     {
      private:
         typedef typename ReferenceTraits<Reference>::Value Value;
         typedef typename ReferenceTraits<Reference>::Pointer Pointer;
         
         Reference reference;
         Value oldValue;
         
      public:
         explicit Scoped()
         :
         	reference(),
         	oldValue( reference )
         {}
         
         explicit Scoped( Reference theProperty )
           : reference( theProperty ),
             oldValue( reference )
           {
           }

         explicit Scoped( Value newValue )
           : reference(),
             oldValue( reference )
           {
            reference = newValue;
           }
         
         explicit Scoped( Reference theProperty, Value newValue )
           : reference( theProperty ),
             oldValue( reference )
           {
            reference = newValue;
           }
         
         Scoped( const Scoped& source )
           : reference( source.reference ),
             oldValue( reference )
           {
           }
         
         ~Scoped()
           {
            reference = oldValue;
           }

         Pointer operator&() const                                             { return &reference; }

         Value Get() const                                                     { return reference;   }
         void Set( Value value ) const                                         { reference = value; }
         
         operator Value() const                                                { return Get(); }
         
         const Scoped& operator=( Value value ) const                          { Set( value ); return *this; }
         const Scoped& operator=( const Scoped& source ) const                 { Set( source.Get() ); return *this; }

         template < class T > const Scoped& operator+=( const T& rhs ) const   { Set( Get() + rhs ); return *this; }
         template < class T > const Scoped& operator-=( const T& rhs ) const   { Set( Get() - rhs ); return *this; }
         template < class T > const Scoped& operator*=( const T& rhs ) const   { Set( Get() * rhs ); return *this; }
         template < class T > const Scoped& operator/=( const T& rhs ) const   { Set( Get() / rhs ); return *this; }
         template < class T > const Scoped& operator%=( const T& rhs ) const   { Set( Get() % rhs ); return *this; }

         template < class T > const Scoped& operator&=( const T& rhs ) const   { Set( Get() & rhs ); return *this; }
         template < class T > const Scoped& operator|=( const T& rhs ) const   { Set( Get() | rhs ); return *this; }
         template < class T > const Scoped& operator^=( const T& rhs ) const   { Set( Get() ^ rhs ); return *this; }

         template < class T > const Scoped& operator<<=( const T& rhs ) const  { Set( Get() << rhs ); return *this; }
         template < class T > const Scoped& operator>>=( const T& rhs ) const  { Set( Get() >> rhs ); return *this; }
     };

   template < class Reference >
   struct ConvertInputTraits< Scoped<Reference> >
     {
      typedef typename ReferenceTraits<Reference>::Value ConverterInputType;
     };
  }

#endif
