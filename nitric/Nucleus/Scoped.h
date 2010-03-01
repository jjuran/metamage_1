// Nucleus/Scoped.h
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_SCOPED_H
#define NUCLEUS_SCOPED_H

#ifndef NUCLEUS_REFERENCETRAITS_H
#include "Nucleus/ReferenceTraits.h"
#endif
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif

namespace Nucleus
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
  }

namespace nucleus
{
	
	template < class Reference >
	struct convert_input_traits< Nucleus::Scoped< Reference > >
	{
		typedef typename Nucleus::ReferenceTraits< Reference >::Value converter_input_type;
	};
	
}

#endif
