// ArrowProxy.h

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
