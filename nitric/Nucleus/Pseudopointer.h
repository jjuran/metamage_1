// Nucleus/Pseudopointer.h
// -----------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_PSEUDOPOINTER_H
#define NUCLEUS_PSEUDOPOINTER_H

#ifndef NUCLEUS_ARROWPROXY_H
#include "Nucleus/ArrowProxy.h"
#endif
#ifndef NUCLEUS_POINTERTRAITS_H
#include "Nucleus/PointerTraits.h"
#endif

#include <functional>

namespace Nucleus
  {
   template < class ReferenceType >
   class Pseudopointer
     {
      public:
         typedef ReferenceType                            Reference;
         
         typedef typename Reference::Details              Details;
         typedef typename Reference::Value                Value;
         typedef typename Reference::ConstReference       ConstReference;

         typedef Pseudopointer< Reference >               Pointer;
         typedef Pseudopointer< ConstReference >          ConstPointer;
               
      private:
         Details details;
      
      public:
         Pseudopointer()                                                            : details()   {}
         Pseudopointer( Details theDetails )                                        : details( theDetails )   {}
         
         template < class R >
         Pseudopointer( const Pseudopointer<R>& p )                                 : details( (&Reference( *p )).Get() ) {}

         Reference operator*() const                                                { return Reference( details ); }
         ArrowProxy<Reference> operator->() const                                   { return Reference( details ); }
         
         operator Details() const                                                   { return details; }
         Details Get() const                                                        { return details; }

         friend bool operator==( const Pseudopointer& a, const Pseudopointer& b )   { return a.Get() == b.Get(); }
         friend bool operator!=( const Pseudopointer& a, const Pseudopointer& b )   { return a.Get() != b.Get(); }

         friend bool operator==( const Pseudopointer& a, const Details& b )         { return a.Get() == b; }
         friend bool operator!=( const Pseudopointer& a, const Details& b )         { return a.Get() != b; }

         friend bool operator==( const Details& a, const Pseudopointer& b )         { return a == b.Get(); }
         friend bool operator!=( const Details& a, const Pseudopointer& b )         { return a != b.Get(); }
     };

   template < class ReferenceType >
   struct PointerTraits< Pseudopointer< ReferenceType > >
     {
      typedef Pseudopointer< ReferenceType > Pointer;

      typedef typename Pointer::Value           Value;
      typedef typename Pointer::Reference       Reference;
      typedef typename Pointer::ConstReference  ConstReference;
      typedef typename Pointer::ConstPointer    ConstPointer;
     };
  }

#endif
