// FunctionTraits.h

#ifndef NITROGEN_FUNCTIONTRAITS_H
#define NITROGEN_FUNCTIONTRAITS_H

namespace Nitrogen
  {
   template < class FunctionType > struct FunctionTraits;
   
   template < class R >
   class FunctionTraits< R(*)() >
     {
      static const int parameterCount = 0;
      typedef R Result;
     };

   template < class R, class P0 >
   class FunctionTraits< R(*)( P0 ) >
     {
      static const int parameterCount = 1;
      typedef R Result;
      typedef P0 Parameter0;
     };

   template < class R, class P0, class P1 >
   class FunctionTraits< R(*)( P0, P1 ) >
     {
      static const int parameterCount = 2;
      typedef R Result;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
     };

   template < class R, class P0, class P1, class P2 >
   class FunctionTraits< R(*)( P0, P1, P2 ) >
     {
      static const int parameterCount = 3;
      typedef R Result;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
     };

   template < class R, class P0, class P1, class P2, class P3 >
   class FunctionTraits< R(*)( P0, P1, P2, P3 ) >
     {
      static const int parameterCount = 4;
      typedef R Result;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
     };

   template < class R, class P0, class P1, class P2, class P3, class P4 >
   class FunctionTraits< R(*)( P0, P1, P2, P3, P4 ) >
     {
      static const int parameterCount = 5;
      typedef R Result;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
     };

   template < class R, class P0, class P1, class P2, class P3, class P4, class P5 >
   class FunctionTraits< R(*)( P0, P1, P2, P3, P4, P5 ) >
     {
      static const int parameterCount = 6;
      typedef R Result;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
      typedef P5 Parameter5;
     };

   template < class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6 >
   class FunctionTraits< R(*)( P0, P1, P2, P3, P4, P5, P6 ) >
     {
      static const int parameterCount = 7;
      typedef R Result;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
      typedef P5 Parameter5;
      typedef P6 Parameter6;
     };

   template < class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7 >
   class FunctionTraits< R(*)( P0, P1, P2, P3, P4, P5, P6, P7 ) >
     {
      static const int parameterCount = 8;
      typedef R Result;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
      typedef P5 Parameter5;
      typedef P6 Parameter6;
      typedef P7 Parameter7;
     };

   template < class R, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8 >
   class FunctionTraits< R(*)( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) >
     {
      static const int parameterCount = 9;
      typedef R Result;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
      typedef P5 Parameter5;
      typedef P6 Parameter6;
      typedef P7 Parameter7;
      typedef P8 Parameter8;
     };

   template < class R, class C >
   class FunctionTraits< R(C::*)() >
     {
      static const int parameterCount = 0;
      typedef R Result;
      typedef C Class;
     };

   template < class R, class C, class P0 >
   class FunctionTraits< R(C::*)( P0 ) >
     {
      static const int parameterCount = 1;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
     };

   template < class R, class C, class P0, class P1 >
   class FunctionTraits< R(C::*)( P0, P1 ) >
     {
      static const int parameterCount = 2;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
     };

   template < class R, class C, class P0, class P1, class P2 >
   class FunctionTraits< R(C::*)( P0, P1, P2 ) >
     {
      static const int parameterCount = 3;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
     };

   template < class R, class C, class P0, class P1, class P2, class P3 >
   class FunctionTraits< R(C::*)( P0, P1, P2, P3 ) >
     {
      static const int parameterCount = 4;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
     };

   template < class R, class C, class P0, class P1, class P2, class P3, class P4 >
   class FunctionTraits< R(C::*)( P0, P1, P2, P3, P4 ) >
     {
      static const int parameterCount = 5;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
     };

   template < class R, class C, class P0, class P1, class P2, class P3, class P4, class P5 >
   class FunctionTraits< R(C::*)( P0, P1, P2, P3, P4, P5 ) >
     {
      static const int parameterCount = 6;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
      typedef P5 Parameter5;
     };

   template < class R, class C, class P0, class P1, class P2, class P3, class P4, class P5, class P6 >
   class FunctionTraits< R(C::*)( P0, P1, P2, P3, P4, P5, P6 ) >
     {
      static const int parameterCount = 7;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
      typedef P5 Parameter5;
      typedef P6 Parameter6;
     };

   template < class R, class C, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7 >
   class FunctionTraits< R(C::*)( P0, P1, P2, P3, P4, P5, P6, P7 ) >
     {
      static const int parameterCount = 8;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
      typedef P5 Parameter5;
      typedef P6 Parameter6;
      typedef P7 Parameter7;
     };

   template < class R, class C, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8 >
   class FunctionTraits< R(C::*)( P0, P1, P2, P3, P4, P5, P6, P7, P8 ) >
     {
      static const int parameterCount = 9;
      typedef R Result;
      typedef C Class;
      typedef P0 Parameter0;
      typedef P1 Parameter1;
      typedef P2 Parameter2;
      typedef P3 Parameter3;
      typedef P4 Parameter4;
      typedef P5 Parameter5;
      typedef P6 Parameter6;
      typedef P7 Parameter7;
      typedef P8 Parameter8;
     };
  }

#endif
