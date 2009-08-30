// Nucleus/PointerTraits.h
// -----------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_POINTERTRAITS_H
#define NUCLEUS_POINTERTRAITS_H

namespace Nucleus
  {
   template < class T > struct PointerTraits;
   
   template < class T > struct PointerTraits< T* >
     {
      typedef T Value;
      typedef T *Pointer;
      typedef T& Reference;
      typedef const T *ConstPointer;
      typedef const T& ConstReference;
     };
   
   template < class T > struct PointerTraits< const T* >
     {
      typedef T Value;
      typedef const T *Pointer;
      typedef const T& Reference;
      typedef const T *ConstPointer;
      typedef const T& ConstReference;
     };
   
   template < class T > struct PointerTraits< volatile T* >
     {
      typedef T Value;
      typedef volatile T *Pointer;
      typedef volatile T& Reference;
      typedef const volatile T *ConstPointer;
      typedef const volatile T& ConstReference;
     };
   
   template < class T > struct PointerTraits< const volatile T* >
     {
      typedef T Value;
      typedef const volatile T *Pointer;
      typedef const volatile T& Reference;
      typedef const volatile T *ConstPointer;
      typedef const volatile T& ConstReference;
     };
  }

#endif
