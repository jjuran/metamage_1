// MacMemory.h

#ifndef NITROGEN_MACMEMORY_H
#define NITROGEN_MACMEMORY_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MACMEMORY__
#include FRAMEWORK_HEADER(CarbonCore,MacMemory.h)
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
   
	class Ptr
	{
		private:
			typedef void *InternalType;
			
			InternalType ptr;
		
		public:
			Ptr()                      : ptr( 0 )  {}
			
			template < class T >
			Ptr( T *thePtr )           : ptr( reinterpret_cast< InternalType >( thePtr ) ) {}
			
			::Ptr Get() const          { return reinterpret_cast< ::Ptr >( ptr ); }
			operator ::Ptr() const     { return Get(); }
			
			//void operator*() const       { return *ptr; }
	};
	
	template <> struct Disposer< Ptr > : public std::unary_function< Ptr, void >,
	                                     private DefaultDestructionOSStatusPolicy
	{
		void operator()( Ptr ptr ) const
		{
			OnlyOnce< RegisterMemoryManagerErrors >();
			::DisposePtr( ptr );
			HandleDestructionOSStatus( ::MemError() );
		}
	};
	
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
   
   Owned< Ptr > NewPtrSysClear( std::size_t size );
   
   Owned<Handle> NewHandle( std::size_t size );
   Owned< Handle > NewHandleSys( std::size_t size );
   Owned< Handle > NewHandleSys( const unsigned char* str );
   
   template < class T >
   Owned< T**, Disposer<Handle> > NewHandle( std::size_t elementCount = 1 )
     {
      return Handle_Cast<T>( Nitrogen::NewHandle( elementCount * sizeof(T) ) );
     }
   
  }

#endif
