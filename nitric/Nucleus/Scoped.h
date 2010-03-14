// Nucleus/Scoped.h
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2010 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_SCOPED_H
#define NUCLEUS_SCOPED_H

#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif

namespace Nucleus
  {
   template < class Value >
   class Scoped
     {
      private:
         typedef Value&  Reference;
         typedef Value  *Pointer;
         
         Reference reference;
         Value oldValue;
         
      public:
         explicit Scoped( Reference theProperty )
           : reference( theProperty ),
             oldValue( reference )
           {
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
     };
  }

namespace nucleus
{
	
	template < class Value >
	struct convert_input_traits< Nucleus::Scoped< Value > >
	{
		typedef Value converter_input_type;
	};
	
}

#endif
