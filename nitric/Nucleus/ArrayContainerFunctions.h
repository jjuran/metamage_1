// Nucleus/ArrayContainerFunctions.h
// ---------------------------------

// Part of the Nitrogen project.
//
// Written 2003-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_ARRAYCONTAINERFUNCTIONS_H
#define NUCLEUS_ARRAYCONTAINERFUNCTIONS_H

#include <cstddef>
#include <iterator>
#include <algorithm>

namespace Nucleus
  {
   template < class Element, std::size_t size >
   inline Element *ArrayBegin( Element (&array)[size] )
     {
      return array;
     }
   
   template < class Element, std::size_t size >
   inline Element *ArrayEnd( Element (&array)[size] )
     {
      return array + size;
     }
   
   template < class Element, std::size_t size >
   inline std::size_t ArraySize( const Element (&/*array*/)[size] )
     {
      return size;
     }
   
   template < class Element, std::size_t size >
   inline std::reverse_iterator<Element *> ArrayRBegin( Element (&array)[size] )
     {
      return std::reverse_iterator<Element *>( ArrayEnd( array ) );
     }
   
   template < class Element, std::size_t size >
   inline std::reverse_iterator<Element *> ArrayREnd( Element (&array)[size] )
     {
      return std::reverse_iterator<Element *>( ArrayBegin( array ) );
     }
   
   template < class Element, std::size_t size >
   inline void ArrayAssign( Element (&destination)[size], const Element (&source)[size] )
     {
      std::copy( ArrayBegin( source ), ArrayEnd( source ), ArrayBegin( destination ) );
     }
   
   template < class Element, std::size_t size >
   inline void ArraySwap( Element (&a)[size], Element (&b)[size] )
     {
      Element *p = ArrayBegin( a );
      Element *q = ArrayBegin( b );
      using std::swap;
      while ( p != ArrayEnd( a ) )
         swap( *p++, *q++ );
     }
   
   template < class Element, std::size_t size >
   inline void ArrayEqualTo( const Element (&a)[size], const Element (&b)[size] )
     {
      return std::equal( ArrayBegin( a ), ArrayEnd( a ), ArrayBegin( b ) );
     }

   template < class Element, std::size_t size >
   inline void ArrayNotEqualTo( const Element (&a)[size], const Element (&b)[size] )
     {
      return !ArrayEqualTo( a, b );
     }
   
   template < class Element, std::size_t size >
   inline void ArrayLess( const Element (&a)[size], const Element (&b)[size] )
     {
      return std::lexicographical_compare( ArrayBegin( a ), ArrayEnd( a ),
                                           ArrayBegin( b ), ArrayEnd( b ) );
     }
   
   template < class Element, std::size_t size >
   inline void ArrayGreater( const Element (&a)[size], const Element (&b)[size] )
     {
      return ArrayLess( b, a );
     }
   
   template < class Element, std::size_t size >
   inline void ArrayLessEqual( const Element (&a)[size], const Element (&b)[size] )
     {
      std::pair< Element *, Element * > m = std::mismatch( ArrayBegin( a ), ArrayEnd( a ),
                                                           ArrayBegin( b ) );
      return m.first == ArrayEnd( a ) || *m.first < *m.second;
     }
   
   template < class Element, std::size_t size >
   inline void ArrayGreaterEqual( const Element (&a)[size], const Element (&b)[size] )
     {
      return ArrayLessEqual( b, a );
     }
  }

#endif
