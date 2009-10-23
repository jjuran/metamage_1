// Nitrogen/MacMemory.h
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_MACMEMORY_H
#define NITROGEN_MACMEMORY_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MACMEMORY__
#include FRAMEWORK_HEADER(CarbonCore,MacMemory.h)
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_PSEUDOREFERENCE_H
#include "Nucleus/Pseudoreference.h"
#endif
#ifndef NUCLEUS_SCOPED_H
#include "Nucleus/Scoped.h"
#endif
#include "Nucleus/ErrorsRegistered.h"

#include <cstddef>


#if !CALL_NOT_IN_CARBON

inline Handle NewHandleSys     ( long bytes )  { return NewHandle     ( bytes ); }
inline Handle NewHandleSysClear( long bytes )  { return NewHandleClear( bytes ); }

inline Ptr    NewPtrSys        ( long bytes )  { return NewPtr        ( bytes ); }
inline Ptr    NewPtrSysClear   ( long bytes )  { return NewPtrClear   ( bytes ); }

#endif

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( MemoryManager );
	
   
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
  }

namespace Nucleus
  {
	template <> struct Disposer< Nitrogen::Ptr > : public std::unary_function< Nitrogen::Ptr, void >,
	                                               private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::Ptr ptr ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::MemoryManager );
			
			::DisposePtr( ptr );
			HandleDestructionOSStatus( ::MemError() );
		}
	};
  }

namespace Nitrogen
  {
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
   
   using ::StringHandle;
  }

namespace Nucleus
  {
   template <> struct Disposer<Nitrogen::Handle>: public std::unary_function< Nitrogen::Handle, void >,
                                                  private Nitrogen::DefaultDestructionOSStatusPolicy
     {
      void operator()( Nitrogen::Handle h ) const
        {
         NUCLEUS_REQUIRE_ERRORS( Nitrogen::MemoryManager );
         
         ::DisposeHandle( h );
         HandleDestructionOSStatus( ::MemError() );
        }
     };
   
	template <>  struct Disposer_Traits< Nitrogen::StringHandle > : Disposer_Traits< Nitrogen::Handle >  {};
  }

namespace Nitrogen
  {
	template < class T >
	inline T* Ptr_Cast( Ptr p )
	{
		return reinterpret_cast< T* >( p.Get() );
	}
	
	template < class T >
	inline Nucleus::Owned< T*, Nucleus::Disposer< Ptr > > Ptr_Cast( Nucleus::Owned< Ptr > p )
	{
		return Nucleus::Owned< T*, Nucleus::Disposer< Ptr > >::Seize( reinterpret_cast< T* >( p.Release().Get() ) );
	}
   
   template < class T >
   inline T** Handle_Cast( Handle h )
     {
      return reinterpret_cast<T**>( h.Get() );
     }
   
   template < class T >
   inline Nucleus::Owned< T**, Nucleus::Disposer<Handle> > Handle_Cast( Nucleus::Owned< Handle > h )
     {
      return Nucleus::Owned< T**, Nucleus::Disposer<Handle> >::Seize( reinterpret_cast< T** >( h.Release().Get() ) );
     }
   
   // 394
   void MemError();
   
	template < class Type >
	inline Type* CheckMemory( Type* ptr )
	{
		if ( static_cast< const void* >( ptr ) == NULL )
		{
			MemError();
			//throw ParamErr();
		}
		return ptr;
	}
	
	// 425, 441, 458, 474
	Nucleus::Owned< Handle > NewHandle        ( std::size_t size );
	Nucleus::Owned< Handle > NewHandleSys     ( std::size_t size );
	Nucleus::Owned< Handle > NewHandleClear   ( std::size_t size );
	Nucleus::Owned< Handle > NewHandleSysClear( std::size_t size );
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Handle > > NewHandle( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandle( size ) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Handle > > NewHandleSys( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleSys( size ) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Handle > > NewHandleClear( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleClear( size ) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Handle > > NewHandleSysClear( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleSysClear( size ) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Handle > > NewHandleArray( std::size_t elementCount = 1 )
	{
		return NewHandle< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Handle > > NewHandleArraySys( std::size_t elementCount = 1 )
	{
		return NewHandleSys< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Handle > > NewHandleArrayClear( std::size_t elementCount = 1 )
	{
		return NewHandleClear< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Handle > > NewHandleArraySysClear( std::size_t elementCount = 1 )
	{
		return NewHandleSysClear< T >( elementCount * sizeof (T) );
	}
	
	// 539, 555, 572, 588
	Nucleus::Owned< Ptr > NewPtr        ( std::size_t size );
	Nucleus::Owned< Ptr > NewPtrClear   ( std::size_t size );
	Nucleus::Owned< Ptr > NewPtrSys     ( std::size_t size );
	Nucleus::Owned< Ptr > NewPtrSysClear( std::size_t size );
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Ptr > > NewPtr( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtr( size ) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Ptr > > NewPtrSys( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrSys( size ) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Ptr > > NewPtrClear( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrClear( size ) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Ptr > > NewPtrSysClear( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrSysClear( size ) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Ptr > > NewPtrArray( std::size_t elementCount = 1 )
	{
		return NewPtr< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Ptr > > NewPtrArraySys( std::size_t elementCount = 1 )
	{
		return NewPtrSys< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Ptr > > NewPtrArrayClear( std::size_t elementCount = 1 )
	{
		return NewPtrClear< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline Nucleus::Owned< T**, Nucleus::Disposer< Ptr > > NewPtrArraySysClear( std::size_t elementCount = 1 )
	{
		return NewPtrSysClear< T >( elementCount * sizeof (T) );
	}
	
	
	// 701
	inline void HLock( Handle h )
	{
		::HLock( h );
		MemError();
	}
	
	inline void HUnlock( Handle h )
	{
		::HUnlock( h );
		MemError();
	}
	
	inline void HPurge( Handle h )
	{
		::HPurge( h );
		MemError();
	}
	
	inline void HNoPurge( Handle h )
	{
		::HNoPurge( h );
		MemError();
	}
	
	inline void HLockHi( Handle h )
	{
		::HLockHi( h );
		MemError();
	}
	
	Nucleus::Owned< Handle > TempNewHandle( std::size_t size );
	
	inline void MoveHHi( Handle h )
	{
		::MoveHHi( h );
		MemError();
	}
	
	// 1058
	inline void DisposePtr( Nucleus::Owned< Ptr > )  {}
	
	inline std::size_t GetPtrSize( Ptr p )
	{
		std::size_t size = ::GetPtrSize( p );
		
		MemError();
		
		return size;
	}
	
	template < class T >
	inline std::size_t GetPtrArraySize( T* p )
	{
		return Nitrogen::GetPtrSize( Ptr( p ) ) / sizeof (T);
	}
	
	// 1102
	inline void DisposeHandle( Nucleus::Owned< Handle > )  {}
	
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
	
	enum Handle_Flags
	{
		kHandle_Flags_Max = Nucleus::Enumeration_Traits< SInt8 >::max
	};
	
	// 1253
	inline Handle_Flags HGetState( Handle h )
	{
		Handle_Flags flags = Handle_Flags( ::HGetState( h ) );
		
		MemError();
		
		return flags;
	}
	
	// 1268
	inline void HSetState( Handle h, Handle_Flags state )
	{
		::HSetState( h, state );
		
		MemError();
	}
	
	
	class HandleState_Value
	{
		private:
			Handle h;
			
		public:
			typedef Handle_Flags Value;
			typedef Handle_Flags GetResult;
			typedef Handle_Flags SetParameter;
			
			static const bool hasSwap = false;
			
			HandleState_Value( Handle h )         : h( h )  {}
			
			GetResult Get() const                 { return HGetState( h ); }
			void Set( SetParameter state ) const  { HSetState( h, state ); }
	};
	
	typedef HandleState_Value HandleState_Details;
	
	typedef Nucleus::Pseudoreference< HandleState_Details > HandleState;
   	
	struct PtrFlattener
	{
		typedef Ptr Put_Parameter;
		
		template < class Putter > void Put( Put_Parameter toPut, Putter put )
		{
			const char* begin = toPut;
			const std::size_t size = GetPtrSize( toPut );
			
			put( begin, begin + size );
		}
		
		typedef Nucleus::Owned< Ptr > Get_Result;
		
		template < class Getter > Get_Result Get( Getter get )
		{
			const std::size_t size = get.size();
			Get_Result result = NewPtr( size );
			
			char* begin = result.Get();
			
			get( begin, begin + size );
			
			return result;
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
	template < class T >
	struct TypedPtrFlattener
	{
		typedef T   DataType;
		typedef T*  PtrType;
		
		typedef PtrType Put_Parameter;
		
		template < class Putter > void Put( Put_Parameter toPut, Putter put )
		{
			PtrFlattener().Put( Ptr( toPut ), put );
		}
		
		typedef Nucleus::Owned< PtrType > Get_Result;
		
		template < class Getter > Get_Result Get( Getter get )
		{
			return Ptr_Cast< T >( PtrFlattener().Get( get ) );
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
	struct HandleFlattener
	{
		typedef Handle Put_Parameter;
		
		template < class Putter > void Put( Put_Parameter toPut, Putter put )
		{
			Nucleus::Scoped< HandleState > hState( HandleState( toPut ) );
			HLock( toPut );
			
			const char* begin = *toPut.Get();
			const std::size_t size = GetHandleSize( toPut );
			
			put( begin, begin + size );
		}
		
		typedef Nucleus::Owned< Handle > Get_Result;
		
		template < class Getter > Get_Result Get( Getter get )
		{
			const std::size_t size = get.size();
			Get_Result result = NewHandle( size );
			
			// A new handle's state is known and need not be saved
			HLock( result );
			
			char* begin = *result.Get();
			
			get( begin, begin + size );
			
			// If we get this far, the handle will be unlocked as before.
			// On exception, the handle is disposed anyway, locked or not.
			HUnlock( result );
			
			return result;
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
	template < class T >
	struct TypedHandleFlattener
	{
		typedef T   DataType;
		typedef T** HandleType;
		
		typedef HandleType Put_Parameter;
		
		template < class Putter > void Put( Put_Parameter toPut, Putter put )
		{
			HandleFlattener().Put( Handle( toPut ), put );
		}
		
		typedef Nucleus::Owned< HandleType > Get_Result;
		
		template < class Getter > Get_Result Get( Getter get )
		{
			return Handle_Cast< T >( HandleFlattener().Get( get ) );
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
}

#endif
