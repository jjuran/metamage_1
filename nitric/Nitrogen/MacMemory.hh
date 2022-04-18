// Nitrogen/MacMemory.hh
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_MACMEMORY_HH
#define NITROGEN_MACMEMORY_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif
#ifndef NUCLEUS_SAVED_HH
#include "nucleus/saved.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif
#ifndef MAC_MEMORY_TYPES_PTR_HH
#include "Mac/Memory/Types/Ptr.hh"
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
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( MemoryManager );
	
	using Mac::Handle;
	using Mac::Ptr;
	
}

namespace nucleus
  {
	template <>  struct disposer_class< StringHandle > : disposer_class< Nitrogen::Handle >  {};
  }

namespace Nitrogen
  {
	template < class T >
	inline T* Ptr_Cast( Ptr p )
	{
		return reinterpret_cast< T* >( p.Get() );
	}
	
	template < class T >
	inline nucleus::owned< T*, nucleus::disposer< Ptr > > Ptr_Cast( nucleus::owned< Ptr > p )
	{
		return nucleus::owned< T*, nucleus::disposer< Ptr > >::seize( reinterpret_cast< T* >( p.release().Get() ) );
	}
   
   template < class T >
   inline T** Handle_Cast( Handle h )
     {
      return reinterpret_cast<T**>( h.Get() );
     }
   
   template < class T >
   inline nucleus::owned< T**, nucleus::disposer<Handle> > Handle_Cast( nucleus::owned< Handle > h )
     {
      return nucleus::owned< T**, nucleus::disposer<Handle> >::seize( reinterpret_cast< T** >( h.release().Get() ) );
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
	nucleus::owned< Handle > NewHandle        ( std::size_t size );
	nucleus::owned< Handle > NewHandleSys     ( std::size_t size );
	nucleus::owned< Handle > NewHandleClear   ( std::size_t size );
	nucleus::owned< Handle > NewHandleSysClear( std::size_t size );
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Handle > > NewHandle( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandle( size ) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Handle > > NewHandleSys( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleSys( size ) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Handle > > NewHandleClear( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleClear( size ) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Handle > > NewHandleSysClear( std::size_t size )
	{
		return Handle_Cast< T >( Nitrogen::NewHandleSysClear( size ) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Handle > > NewHandleArray( std::size_t elementCount = 1 )
	{
		return NewHandle< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Handle > > NewHandleArraySys( std::size_t elementCount = 1 )
	{
		return NewHandleSys< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Handle > > NewHandleArrayClear( std::size_t elementCount = 1 )
	{
		return NewHandleClear< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Handle > > NewHandleArraySysClear( std::size_t elementCount = 1 )
	{
		return NewHandleSysClear< T >( elementCount * sizeof (T) );
	}
	
	// 539, 555, 572, 588
	nucleus::owned< Ptr > NewPtr        ( std::size_t size );
	nucleus::owned< Ptr > NewPtrClear   ( std::size_t size );
	nucleus::owned< Ptr > NewPtrSys     ( std::size_t size );
	nucleus::owned< Ptr > NewPtrSysClear( std::size_t size );
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Ptr > > NewPtr( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtr( size ) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Ptr > > NewPtrSys( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrSys( size ) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Ptr > > NewPtrClear( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrClear( size ) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Ptr > > NewPtrSysClear( std::size_t size )
	{
		return Ptr_Cast< T >( Nitrogen::NewPtrSysClear( size ) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Ptr > > NewPtrArray( std::size_t elementCount = 1 )
	{
		return NewPtr< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Ptr > > NewPtrArraySys( std::size_t elementCount = 1 )
	{
		return NewPtrSys< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Ptr > > NewPtrArrayClear( std::size_t elementCount = 1 )
	{
		return NewPtrClear< T >( elementCount * sizeof (T) );
	}
	
	template < class T >
	inline nucleus::owned< T**, nucleus::disposer< Ptr > > NewPtrArraySysClear( std::size_t elementCount = 1 )
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
	
#if ! __LP64__
	
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
	
#endif
	
	inline void HLockHi( Handle h )
	{
		::HLockHi( h );
		MemError();
	}
	
	nucleus::owned< Handle > TempNewHandle( std::size_t size );
	
#if ! __LP64__
	
	inline void MoveHHi( Handle h )
	{
		::MoveHHi( h );
		MemError();
	}
	
#endif
	
	// 1058
	inline void DisposePtr( nucleus::owned< Ptr > )  {}
	
	inline std::size_t GetPtrSize( Ptr p )
	{
		std::size_t size = ::GetPtrSize( p );
		
		MemError();
		
		return size;
	}
	
	// 1102
	inline void DisposeHandle( nucleus::owned< Handle > )  {}
	
	// 1117
	void SetHandleSize( Handle h, std::size_t size );
	
	// 1141
	inline std::size_t GetHandleSize( Handle h )
	{
		std::size_t size = ::GetHandleSize( h );
		MemError();
		return size;
	}
	
	enum Handle_Flags
	{
		kHandle_Flags_Max = nucleus::enumeration_traits< SInt8 >::max
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
	
	
	class Handle_State
	{
		private:
			Handle h;
			
		public:
			typedef Handle_Flags value_type;
			typedef Handle_Flags param_type;
			
			static const bool can_swap = false;
			
			Handle_State( Handle h )            : h( h )  {}
			
			value_type get() const              { return HGetState( h ); }
			void set( param_type state ) const  { HSetState( h, state ); }
	};
	
	struct PtrFlattener
	{
		typedef                 Ptr    argument_type;
		typedef nucleus::owned< Ptr >  result_type;
		
		template < class Putter >
		static void Put( argument_type toPut, Putter put )
		{
			const char* begin = toPut;
			const std::size_t size = GetPtrSize( toPut );
			
			put( begin, begin + size );
		}
		
		template < class Getter >
		static result_type Get( Getter get )
		{
			const std::size_t size = get.size();
			
			result_type result = NewPtr( size );
			
			char* begin = result.get();
			
			get( begin, begin + size );
			
			return result;
		}
		
		static const std::size_t static_size = 0;
	};
	
	template < class T >
	struct TypedPtrFlattener
	{
		typedef                 T*    argument_type;
		typedef nucleus::owned< T* >  result_type;
		
		typedef T   DataType;
		typedef T*  PtrType;
		
		template < class Putter >
		static void Put( argument_type toPut, Putter put )
		{
			PtrFlattener::Put( Ptr( toPut ), put );
		}
		
		template < class Getter >
		static result_type Get( Getter get )
		{
			return Ptr_Cast< T >( PtrFlattener::Get( get ) );
		}
		
		static const std::size_t static_size = 0;
	};
	
	struct HandleFlattener
	{
		typedef                 Handle    argument_type;
		typedef nucleus::owned< Handle >  result_type;
		
		template < class Putter >
		static void Put( argument_type toPut, Putter put )
		{
		#if !TARGET_API_MAC_OSX
			
			nucleus::saved< Handle_State > hState( Handle_State( toPut ) );
			
			HLock( toPut );
			
		#endif
			
			const char* begin = *toPut.Get();
			const std::size_t size = GetHandleSize( toPut );
			
			put( begin, begin + size );
		}
		
		template < class Getter >
		static result_type Get( Getter get )
		{
			const std::size_t size = get.size();
			
			result_type result = NewHandle( size );
			
			if ( !TARGET_API_MAC_OSX )
			{
				// A new handle's state is known and need not be saved
				HLock( result );
			}
			
			char* begin = *result.get().Get();
			
			get( begin, begin + size );
			
			if ( !TARGET_API_MAC_OSX )
			{
				// If we get this far, the handle will be unlocked as before.
				// On exception, the handle is disposed anyway, locked or not.
				HUnlock( result );
			}
			
			return result;
		}
		
		static const std::size_t static_size = 0;
	};
	
	template < class T >
	struct TypedHandleFlattener
	{
		typedef                 T**    argument_type;
		typedef nucleus::owned< T** >  result_type;
		
		typedef T   DataType;
		typedef T** HandleType;
		
		template < class Putter >
		static void Put( argument_type toPut, Putter put )
		{
			HandleFlattener::Put( Handle( toPut ), put );
		}
		
		template < class Getter >
		static result_type Get( Getter get )
		{
			return Handle_Cast< T >( HandleFlattener::Get( get ) );
		}
		
		static const std::size_t static_size = 0;
	};
	
}

#endif
