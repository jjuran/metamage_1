// Operators.h

/*
   This file creates a namespace, Nitrogen::Operators, into which every 
   previously-defined Nitrogen operator is redeclared.  The intent is that
   this file will be included at the end of every Nitrogen header which
   defines nonmember operator functions, and that, therefore, the declaration
   "using namespace Nitrogen::Operators" will bring all Nitrogen operators
   into scope.
*/

#ifndef NITROGEN_OPERATORS_H
#define NITROGEN_OPERATORS_H

#include <cstddef>

namespace Nitrogen
  {
   // Declare some operators, so there's something for the using-declarations to see:
   // All of them are intentionally left undefined
   
   class NitrogenOperatorDummyType;

   void operator+( NitrogenOperatorDummyType );
   void operator-( NitrogenOperatorDummyType );
   void operator*( NitrogenOperatorDummyType );
   void operator/( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator%( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator^( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator&( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator|( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator~( NitrogenOperatorDummyType );

   void operator!( NitrogenOperatorDummyType );
   void operator<( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator>( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator+=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator-=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator*=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator/=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator%=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );

   void operator^=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator&=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator|=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator<<( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator>>( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator>>=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator<<=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator==( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator!=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );


   void operator<=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator>=( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator&&( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator||( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator++( NitrogenOperatorDummyType );
   void operator--( NitrogenOperatorDummyType );
   void operator,( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
   void operator->*( NitrogenOperatorDummyType, NitrogenOperatorDummyType );
  }

#endif

/*
   These declarations are intentionally left outside the include-guard.
   Since they can only refer to textually preceding operators, it can be
   useful to repeat them in order to collect more operators.
*/

namespace Nitrogen
  {
   namespace Operators
     {
      using Nitrogen::operator+;
      using Nitrogen::operator-;
      using Nitrogen::operator*;
      using Nitrogen::operator/;
      using Nitrogen::operator%;
      using Nitrogen::operator^;
      using Nitrogen::operator&;
      using Nitrogen::operator|;
      using Nitrogen::operator~;

      using Nitrogen::operator!;
      using Nitrogen::operator<;
      using Nitrogen::operator>;
      using Nitrogen::operator+=;
      using Nitrogen::operator-=;
      using Nitrogen::operator*=;
      using Nitrogen::operator/=;
      using Nitrogen::operator%=;

      using Nitrogen::operator^=;
      using Nitrogen::operator&=;
      using Nitrogen::operator|=;
      using Nitrogen::operator<<;
      using Nitrogen::operator>>;
      using Nitrogen::operator>>=;
      using Nitrogen::operator<<=;
      using Nitrogen::operator==;
      using Nitrogen::operator!=;
      
      using Nitrogen::operator<=;
      using Nitrogen::operator>=;
      using Nitrogen::operator&&;
      using Nitrogen::operator||;
      using Nitrogen::operator++;
      using Nitrogen::operator--;
      using Nitrogen::operator,;
      using Nitrogen::operator->*;
     }
  }
