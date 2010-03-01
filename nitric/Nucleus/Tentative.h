// Nucleus/Tentative.h
// -------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_TENTATIVE_H
#define NUCLEUS_TENTATIVE_H

#ifndef NUCLEUS_REFERENCETRAITS_H
#include "Nucleus/ReferenceTraits.h"
#endif
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif

namespace Nucleus
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
  }

namespace nucleus
{
	
	template < class Reference >
	struct convert_input_traits< Nucleus::Tentative< Reference > >
	{
		typedef typename Nucleus::ReferenceTraits< Reference >::Value converter_input_type;
	};
	
}

#endif
