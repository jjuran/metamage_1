// Nucleus/OnlyOnce.h
// ------------------

// Part of the Nitrogen project.
//
// Written 2002-2004 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_ONLYONCE_H
#define NUCLEUS_ONLYONCE_H

namespace Nucleus
  {
   template < void (*function)() >
   void OnlyOnce()
     {
      static bool dummy = ( function(), false );
      (void)dummy;
     }
  }

#endif
