// Tentative.h

#ifndef NITROGEN_TENTATIVE_H
#define NITROGEN_TENTATIVE_H

#ifndef NITROGEN_REFERENCETRAITS_H
#include "Nitrogen/ReferenceTraits.h"
#endif
#ifndef NITROGEN_CONVERT_H
#include "Nitrogen/Convert.h"
#endif

namespace Nitrogen
  {
   template < class Reference >
   class Tentative
     {
      private:
         typedef typename ReferenceTraits<Reference>::Value Value;
         typedef typename ReferenceTraits<Reference>::Pointer Pointer;
         
         Reference reference;
         Value oldValue;
         bool committed;
         
      public:
         explicit Tentative( Reference theProperty )
           : reference( theProperty ),
             oldValue( reference ),
             committed( false )
           {
           }

         explicit Tentative( Value newValue )
           : reference(),
             oldValue( reference ),
             committed( false )
           {
            reference = newValue;
           }
         
         explicit Tentative( Reference theProperty, Value newValue )
           : reference( theProperty ),
             oldValue( reference ),
             committed( false )
           {
            reference = newValue;
           }
         
         Tentative( const Tentative& source )
           : reference( source.reference ),
             oldValue( reference ),
             committed( false )
           {
           }
         
         void Commit()
           {
            committed = true;
           }
         
         ~Tentative()
           {
            if ( !committed )
               reference = oldValue;
           }

         Pointer operator&() const                                               { return &reference; }

         Value Get() const                                                       { return reference;   }
         void Set( Value value ) const                                           { reference = value; }
         
         operator Value() const                                                  { return Get(); }
         
         const Tentative& operator=( Value value ) const                         { Set( value ); return *this; }
         const Tentative& operator=( const Tentative& source ) const             { Set( source.Get() ); return *this; }

         template < class T > const Tentative& operator+=( const T& rhs ) const  { Set( Get() + rhs ); return *this; }
         template < class T > const Tentative& operator-=( const T& rhs ) const  { Set( Get() - rhs ); return *this; }
         template < class T > const Tentative& operator*=( const T& rhs ) const  { Set( Get() * rhs ); return *this; }
         template < class T > const Tentative& operator/=( const T& rhs ) const  { Set( Get() / rhs ); return *this; }
         template < class T > const Tentative& operator%=( const T& rhs ) const  { Set( Get() % rhs ); return *this; }

         template < class T > const Tentative& operator&=( const T& rhs ) const  { Set( Get() & rhs ); return *this; }
         template < class T > const Tentative& operator|=( const T& rhs ) const  { Set( Get() | rhs ); return *this; }
         template < class T > const Tentative& operator^=( const T& rhs ) const  { Set( Get() ^ rhs ); return *this; }

         template < class T > const Tentative& operator<<=( const T& rhs ) const { Set( Get() << rhs ); return *this; }
         template < class T > const Tentative& operator>>=( const T& rhs ) const { Set( Get() >> rhs ); return *this; }
     };

   template < class Reference >
   struct ConvertInputTraits< Tentative<Reference> >
     {
      typedef typename ReferenceTraits<Reference>::Value ConverterInputType;
     };
  }

#endif
