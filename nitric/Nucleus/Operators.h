// Nucleus/Operators.h
// -------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


/*
   This file creates a namespace, Nucleus::Operators, into which every 
   previously-defined Nucleus operator is redeclared.  The intent is that
   this file will be included at the end of every Nucleus header which
   defines nonmember operator functions, and that, therefore, the declaration
   "using namespace Nucleus::Operators" will bring all Nucleus operators
   into scope.
*/

#ifndef NUCLEUS_OPERATORS_H
#define NUCLEUS_OPERATORS_H

#include <cstddef>

namespace Nucleus
  {
   // Declare some operators, so there's something for the using-declarations to see:
   // All of them are intentionally left undefined
   
   class NucleusOperatorDummyType;

   void operator+( NucleusOperatorDummyType );
   void operator-( NucleusOperatorDummyType );
   void operator*( NucleusOperatorDummyType );
   void operator/( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator%( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator^( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator&( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator|( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator~( NucleusOperatorDummyType );

   void operator!( NucleusOperatorDummyType );
   void operator<( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator>( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator+=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator-=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator*=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator/=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator%=( NucleusOperatorDummyType, NucleusOperatorDummyType );

   void operator^=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator&=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator|=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator<<( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator>>( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator>>=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator<<=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator==( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator!=( NucleusOperatorDummyType, NucleusOperatorDummyType );


   void operator<=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator>=( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator&&( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator||( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator++( NucleusOperatorDummyType );
   void operator--( NucleusOperatorDummyType );
   void operator,( NucleusOperatorDummyType, NucleusOperatorDummyType );
   void operator->*( NucleusOperatorDummyType, NucleusOperatorDummyType );
  }

#endif

/*
   These declarations are intentionally left outside the include-guard.
   Since they can only refer to textually preceding operators, it can be
   useful to repeat them in order to collect more operators.
*/

namespace Nucleus
  {
   namespace Operators
     {
      using Nucleus::operator+;
      using Nucleus::operator-;
      using Nucleus::operator*;
      using Nucleus::operator/;
      using Nucleus::operator%;
      using Nucleus::operator^;
      using Nucleus::operator&;
      using Nucleus::operator|;
      using Nucleus::operator~;

      using Nucleus::operator!;
      using Nucleus::operator<;
      using Nucleus::operator>;
      using Nucleus::operator+=;
      using Nucleus::operator-=;
      using Nucleus::operator*=;
      using Nucleus::operator/=;
      using Nucleus::operator%=;

      using Nucleus::operator^=;
      using Nucleus::operator&=;
      using Nucleus::operator|=;
      using Nucleus::operator<<;
      using Nucleus::operator>>;
      using Nucleus::operator>>=;
      using Nucleus::operator<<=;
      using Nucleus::operator==;
      using Nucleus::operator!=;
      
      using Nucleus::operator<=;
      using Nucleus::operator>=;
      using Nucleus::operator&&;
      using Nucleus::operator||;
      using Nucleus::operator++;
      using Nucleus::operator--;
      using Nucleus::operator,;
      using Nucleus::operator->*;
     }
  }
