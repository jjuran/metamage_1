// ArrowProxy.h

#ifndef NITROGEN_ARROWPROXY_H
#define NITROGEN_ARROWPROXY_H

namespace Nitrogen
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
