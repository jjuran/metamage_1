// PointerTraits.h

#ifndef NITROGEN_POINTERTRAITS_H
#define NITROGEN_POINTERTRAITS_H

namespace Nitrogen
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
