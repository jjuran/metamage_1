// Nucleus/ArrowProxy.h
// --------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_ARROWPROXY_H
#define NUCLEUS_ARROWPROXY_H

namespace Nucleus
  {
   template < class StarType >
   class ArrowProxy
     {
      private:
         StarType star;
      
      public:
         ArrowProxy( StarType s )   : star( s )    {}
         
         StarType& operator*()      { return star; }
         StarType *operator->()     { return &star; }
     };
  }

#endif
