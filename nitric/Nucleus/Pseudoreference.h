// Nucleus/Pseudoreference.h
// -------------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


/*
   A Pseudoreference binds a getter/setter pair of functions and
   their parameters together into an entity that acts like a
   reference to an object:
   
      The Pseudoreference has a conversion to the value type that
      calls the getter; this is also available through the Get member.
      
      Assigning a value to the Pseudoreference calls the setter;
      this can also be done through the Set member.
      
      Assigning one Pseudoreference to another calls the getter of
      the former and the setter of the latter -- that is, as with
      references, it assigns the referred-to values.
      
      Copy-constructing a Pseudoreference produces a Pseudoreference
      with the same referrent.
      
      Taking the address of a Pseudoreference produces a Pseudopointer,
      which can be dereferenced to get the Pseudoreference back.
   
   Rather than have a long list of template parameters, the details for
   building a Pseudoreference are bundled into a class:
   
   class Details
     {
      public:
         typedef implementation-defined Value;        // type of a variable that could store the value
         typedef implementation-defined GetResult;    // type of the result from Get; often the same as Value
         typedef implementation-defined SetParameter; // type of the parameter to Set; often the same as Value

         GetResult Get() const;
         void Set( SetParameter ) const;
     };
   
   The Set member may be omitted for ConstPseudoreferences.
*/
#ifndef NUCLEUS_PSEUDOREFERENCE_H
#define NUCLEUS_PSEUDOREFERENCE_H

#ifndef NUCLEUS_REFERENCETRAITS_H
#include "Nucleus/ReferenceTraits.h"
#endif
#ifndef NUCLEUS_VALUETRAITS_H
#include "Nucleus/ValueTraits.h"
#endif
#ifndef NUCLEUS_PSEUDOPOINTER_H
#include "Nucleus/Pseudopointer.h"
#endif
#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif

namespace Nucleus
  {
   template < class Details > class ConstPseudoreference;
   template < class Details > class Pseudoreference;

   template < class DetailsType >
   class ConstPseudoreference
     {
      public:
         typedef DetailsType                     Details;
         typedef typename Details::Value         Value;
         typedef typename Details::GetResult     GetResult;
         
         typedef ConstPseudoreference< Details > Reference;
         typedef ConstPseudoreference< Details > ConstReference;

         typedef Pseudopointer< Reference >      Pointer;
         typedef Pseudopointer< ConstReference > ConstPointer;
         
      private:
         const Details details;
         
         // not implemented:
            ConstPseudoreference& operator=( const ConstPseudoreference& );
         
      public:         
         ConstPseudoreference()                                                         : details()              {}
         explicit ConstPseudoreference( Details theDetails )                            : details( theDetails )  {}

         template < class P0, class P1 >
         ConstPseudoreference( P0 p0, P1 p1 )                                           : details( p0, p1 )      {}

         template < class P0, class P1, class P2 >
         ConstPseudoreference( P0 p0, P1 p1, P2 p2 )                                    : details( p0, p1, p2 )  {}
          
         Pointer operator&() const                                                      { return Pointer( details ); }

         GetResult Get() const                                                          { return details.Get();   }
         operator GetResult() const                                                     { return Get(); }
     };

   template < class DetailsType >
   class Pseudoreference
     {
      public:
         typedef DetailsType                     Details;
         typedef typename Details::Value         Value;
         typedef typename Details::GetResult     GetResult;
         typedef typename Details::SetParameter  SetParameter;
         
         typedef Pseudoreference     < Details > Reference;
         typedef ConstPseudoreference< Details > ConstReference;

         typedef Pseudopointer< Reference >      Pointer;
         typedef Pseudopointer< ConstReference > ConstPointer;
         
      private:
         const Details details;
      
      public:         
         Pseudoreference()                                                              : details()              {}
         explicit Pseudoreference( Details theDetails )                                 : details( theDetails )  {}
         
         template < class P0, class P1 >
         Pseudoreference( P0 p0, P1 p1 )                                                : details( p0, p1 )      {}

         template < class P0, class P1, class P2 >
         Pseudoreference( P0 p0, P1 p1, P2 p2 )                                         : details( p0, p1, p2 )  {}
         
         Pointer operator&() const                                                      { return Pointer( details ); }
 
         operator ConstReference() const                                                { return ConstReference( details ); }

         GetResult Get() const                                                          { return details.Get();   }
         void Set( SetParameter value ) const                                           { details.Set( value ); }
         
         operator GetResult() const                                                     { return Get(); }
         const Pseudoreference& operator=( SetParameter value ) const                   { Set( value ); return *this; }
         
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



   template < class Details >
   struct ReferenceTraits< ConstPseudoreference< Details > >
     {
      typedef ConstPseudoreference< Details > Reference;
      
      typedef typename Reference::Value           Value;
      typedef typename Reference::Pointer         Pointer;
      typedef typename Reference::ConstReference  ConstReference;
      typedef typename Reference::ConstPointer    ConstPointer;
     };

   template < class Details >
   struct ConvertInputTraits< ConstPseudoreference< Details > >
     {
      typedef typename Details::Value ConverterInputType;
     };
   
   template < class Details >
   struct ReferenceTraits< Pseudoreference< Details > >
     {
      typedef Pseudoreference< Details > Reference;
      
      typedef typename Reference::Value           Value;
      typedef typename Reference::Pointer         Pointer;
      typedef typename Reference::ConstReference  ConstReference;
      typedef typename Reference::ConstPointer    ConstPointer;
     };

   template < class Details >
   struct ConvertInputTraits< Pseudoreference< Details > >
     {
      typedef typename Details::Value ConverterInputType;
     };

   template < class Details >
   struct ValueTraits< ConstPseudoreference< Details > >
     {
      typedef ConstPseudoreference< Details > Reference;
      
      typedef typename Reference::Value           Value;
      typedef typename Reference::Pointer         Pointer;
      typedef typename Reference::ConstReference  ConstReference;
      typedef typename Reference::ConstPointer    ConstPointer;
     };

   template < class Details >
   struct ValueTraits< Pseudoreference< Details > >
     {
      typedef Pseudoreference< Details > Reference;
      
      typedef typename Reference::Value           Value;
      typedef typename Reference::Pointer         Pointer;
      typedef typename Reference::ConstReference  ConstReference;
      typedef typename Reference::ConstPointer    ConstPointer;
     };
  }

#endif
