// MacMemory.h

#ifndef NITROGEN_MACMEMORY_H
#define NITROGEN_MACMEMORY_H

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

#include <cstddef>

namespace Nitrogen
  {
   void RegisterMemoryManagerErrors();
   
   class Handle
     {
      private:
         typedef void *const *InternalType;
         
         InternalType handle;
      
      public:
         Handle()                      : handle( 0 )  {}
         
         template < class T >
         Handle( T **theHandle )       : handle( reinterpret_cast<InternalType>( theHandle ) ) {}
         
         template < class T >
         Handle( T *const *theHandle ) : handle( reinterpret_cast<InternalType>( theHandle ) ) {}
         
         ::Handle Get() const          { return reinterpret_cast< ::Handle >( const_cast< void** >( handle ) ); }
         operator ::Handle() const     { return Get(); }
         
         void *operator*() const       { return *handle; }
     };
   
   template <> struct Disposer<Handle>: public std::unary_function< Handle, void >,
                                        private DefaultDestructionOSStatusPolicy
     {
      void operator()( Handle h ) const
        {
         OnlyOnce<RegisterMemoryManagerErrors>();
         ::DisposeHandle( h );
         HandleDestructionOSStatus( ::MemError() );
        }
     };
   
   template < class T >
   T** Handle_Cast( Handle h )
     {
      return reinterpret_cast<T**>( h.Get() );
     }
   
   void MemError();
   
   Owned<Handle> NewHandle( std::size_t size );
   
   template < class T >
   Owned< T**, Disposer<Handle> > NewHandle( std::size_t elementCount = 1 )
     {
      return Handle_Cast<T>( Nitrogen::NewHandle( elementCount * sizeof(T) ) );
     }
  }

#endif
