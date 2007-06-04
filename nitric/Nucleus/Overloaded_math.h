// Nucleus/Overloaded_math.h
// -------------------------

// Part of the Nitrogen project.
//
// Written 2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


/*
   The 1998 C++ standard specifies that <cmath> should contain versions of the
   functions from <math.h> that are overloaded for float, double, and long double.
   (Such overloading is essential for mathematical templates.)
   
   The 1999 C standard added a bunch of new math functions to <math.h>, which one
   might expect to be similarly overloaded.  But a strict C++ library will
   only overload the specific function mentioned in the 1998 standard.
   
   These functions make up for the trouble of using a strict C++ compiler.
   
   This file requires that <math.h> has been updated for C99.
*/

#ifndef NUCLEUS_OVERLOADED_MATH_H
#define NUCLEUS_OVERLOADED_MATH_H

#include <math.h>

namespace Nucleus
  {
   namespace CStd
     {
      template < class T > T         nearbyint               ( T           x )   { return x; }
      template <> inline float       nearbyint< float       >( float       x )   { return ::nearbyintf(x); }
      template <> inline double      nearbyint< double      >( double      x )   { return ::nearbyint (x); }
      template <> inline long double nearbyint< long double >( long double x )   { return ::nearbyintl(x); }
     }
  }

#endif
