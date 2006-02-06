// ResourceTransfer.h

#ifndef NUCLEUS_RESOURCETRANSFER_H
#define NUCLEUS_RESOURCETRANSFER_H

namespace Nucleus
  {
   template < class Transferred >
   class ResourceTransfer
     {
      private:
         Transferred& transferred;
         
      public:
         explicit ResourceTransfer( Transferred& t )
           : transferred( t )
           {}

         Transferred& operator*() const         { return transferred; }
         Transferred *operator->() const        { return &transferred; }
     };

   template < class Destination, class Resource >
   class ConvertingResourceTransfer
     {
      private:
         Resource converted;
         
      public:
         explicit ConvertingResourceTransfer( const Resource& source )
           : converted( source )
           {}

         Resource& operator*()               { return converted; }
         Resource *operator->()              { return &converted; }
     };
  }

#endif
