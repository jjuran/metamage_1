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
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_PSEUDOREFERENCE_H
#include "Nitrogen/Pseudoreference.h"
#endif

#include <cstddef>


#if !CALL_NOT_IN_CARBON

inline Handle NewHandleSys     ( long bytes )  { return NewHandle     ( bytes ); }
inline Handle NewHandleSysClear( long bytes )  { return NewHandleClear( bytes ); }

inline Ptr    NewPtrSys        ( long bytes )  { return NewPtr        ( bytes ); }
inline Ptr    NewPtrSysClear   ( long bytes )  { return NewPtrClear   ( bytes ); }

#endif

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
         
			friend bool operator==( Ptr a, Ptr b )    { return a.Get() == b.Get(); }
			friend bool operator!=( Ptr a, Ptr b )    { return a.Get() != b.Get(); }
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
         
         friend bool operator==( Handle a, Handle b )    { return a.Get() == b.Get(); }
         friend bool operator!=( Handle a, Handle b )    { return a.Get() != b.Get(); }
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
   
	template <>  struct OwnedDefaults< StringHandle > : OwnedDefaults< Handle >  {};
	
	template < class T >
	T* Ptr_Cast( Ptr p )
	{
		return reinterpret_cast< T* >( p.Get() );
	}
	
   template < class T >
   T** Handle_Cast( Handle h )
     {
      return reinterpret_cast<T**>( h.Get() );
     }
   
   template < class T >
   Owned< T**, Disposer<Handle> > Handle_Cast( Owned< Handle > h )
     {
      // It seems silly to Release and Seize when we merely want to convert,
      // But Handle doesn't convert directly to T**.
      // A workaround would be to define a class AnyHandle that constructs from Handle
      // and converts to T**, and convert h to Owned< AnyHandle, Disposer< Handle > > 
      // and that to Owned< T**, Disposer< Handle > >.
      return Owned< T**, Disposer<Handle> >::Seize( reinterpret_cast< T** >( h.Release().Get() ) );
     }
   
   // 394
   void MemError();
   
	template < class Type >
	Type* CheckMemory( Type* ptr )
	{
		if ( static_cast< const void* >( ptr ) == NULL )
		{
			MemError();
			//throw ParamErr();
		}
		return ptr;
	}
	
	// 425, 441, 458, 474
	Owned< Handle > NewHandle        ( std::size_t size );
	Owned< Handle > NewHandleSys     ( std::size_t size );
	Owned< Handle > NewHandleClear   ( std::size_t size );
	Owned< Handle > NewHandleSysClear( std::size_t size );
	
	template < class T >
	Owned< T**, Disposer< Handle > > NewHandle( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandle( size ) );
	}
	
	template < class T >
	Owned< T**, Disposer< Handle > > NewHandleSys( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleSys( size ) );
	}
	
	template < class T >
	Owned< T**, Disposer< Handle > > NewHandleClear( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleClear( size ) );
	}
	
	template < class T >
	Owned< T**, Disposer< Handle > > NewHandleSysClear( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleSysClear( size ) );
	}
	
	template < class T >
	Owned< T**, Disposer< Handle > > NewHandleArray( std::size_t elementCount = 1 )
	{
		return NewHandle< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	Owned< T**, Disposer< Handle > > NewHandleArraySys( std::size_t elementCount = 1 )
	{
		return NewHandleSys< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	Owned< T**, Disposer< Handle > > NewHandleArrayClear( std::size_t elementCount = 1 )
	{
		return NewHandleClear< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	Owned< T**, Disposer< Handle > > NewHandleArraySysClear( std::size_t elementCount = 1 )
	{
		return NewHandleSysClear< T >( elementCount * sizeof (T) );
	}
	
	// 539, 555, 572, 588
	Owned< Ptr > NewPtr        ( std::size_t size );
	Owned< Ptr > NewPtrClear   ( std::size_t size );
	Owned< Ptr > NewPtrSys     ( std::size_t size );
	Owned< Ptr > NewPtrSysClear( std::size_t size );
	
	template < class T >
	Owned< T**, Disposer< Ptr > > NewPtr( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtr( size ) );
	}
	
	template < class T >
	Owned< T**, Disposer< Ptr > > NewPtrSys( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrSys( size ) );
	}
	
	template < class T >
	Owned< T**, Disposer< Ptr > > NewPtrClear( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrClear( size ) );
	}
	
	template < class T >
	Owned< T**, Disposer< Ptr > > NewPtrSysClear( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrSysClear( size ) );
	}
	
	template < class T >
	Owned< T**, Disposer< Ptr > > NewPtrArray( std::size_t elementCount = 1 )
	{
		return NewPtr< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	Owned< T**, Disposer< Ptr > > NewPtrArraySys( std::size_t elementCount = 1 )
	{
		return NewPtrSys< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	Owned< T**, Disposer< Ptr > > NewPtrArrayClear( std::size_t elementCount = 1 )
	{
		return NewPtrClear< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	Owned< T**, Disposer< Ptr > > NewPtrArraySysClear( std::size_t elementCount = 1 )
	{
		return NewPtrSysClear< T >( elementCount * sizeof (T) );
	}
	
	
	// 701
	inline void HLock( Handle h )
	{
		::HLock( h );
		MemError();
	}
	
	// 1058
	inline void DisposePtr( Owned< Ptr > )  {}
	
	// 1102
	inline void DisposeHandle( Owned< Handle > )  {}
	
	// 1117
	inline void SetHandleSize( Handle h, std::size_t size )
	{
		::SetHandleSize( h, size );
		MemError();
	}
	
	template < class T >
	inline void SetHandleArraySize( T** h, std::size_t elementCount )
	{
		Nitrogen::SetHandleSize( Handle( h ), elementCount * sizeof (T) );
	}
	
	// 1141
	inline std::size_t GetHandleSize( Handle h )
	{
		std::size_t size = ::GetHandleSize( h );
		MemError();
		return size;
	}
	
	template < class T >
	inline std::size_t GetHandleArraySize( T** h )
	{
		return Nitrogen::GetHandleSize( Handle( h ) ) / sizeof (T);
	}
	
	class HandleFlags_Tag {};
	typedef FlagType< HandleFlags_Tag, SInt8 > Handle_Flags;
	
	// 1253
	inline Handle_Flags HGetState( Handle h )
	{
		Handle_Flags flags( ::HGetState( h ) );
		MemError();
		return flags;
	}
	
	// 1268
	inline void HSetState( Handle h, Handle_Flags state )
	{
		::HSetState( h, state );
		MemError();
	}
	
	
	class HandleState_Details
	{
		private:
			Handle h;
			
		public:
			typedef Handle_Flags Value;
			typedef Value GetResult;
			typedef Value SetParameter;
			
			HandleState_Details( Handle h )     : h( h ) {}
			GetResult Get() const                          { return HGetState( h ); }
			void Set( SetParameter state ) const          { HSetState( h, state ); }
	};
	
	typedef Pseudoreference< HandleState_Details > HandleState;
   	
  }

#endif
