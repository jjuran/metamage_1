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

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/errors_registered.hh"
#include "nucleus/flag_ops.hh"
#include "nucleus/owned.hh"
#include "nucleus/saved.hh"

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
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

namespace nucleus
  {
	template <> struct disposer< Nitrogen::Ptr > : public std::unary_function< Nitrogen::Ptr, void >
	{
		void operator()( Nitrogen::Ptr ptr ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::MemoryManager );
			
			::DisposePtr( ptr );
			
			::Nitrogen::HandleDestructionOSStatus( ::MemError() );
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

namespace nucleus
  {
   template <> struct disposer<Nitrogen::Handle>: public std::unary_function< Nitrogen::Handle, void >
     {
      void operator()( Nitrogen::Handle h ) const
        {
         NUCLEUS_REQUIRE_ERRORS( Nitrogen::MemoryManager );
         
         ::DisposeHandle( h );
         
         ::Nitrogen::HandleDestructionOSStatus( ::MemError() );
        }
     };
   
	template <>  struct disposer_traits< StringHandle > : disposer_traits< Nitrogen::Handle >  {};
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
	
	nucleus::owned< Handle > TempNewHandle( std::size_t size );
	
	inline void MoveHHi( Handle h )
	{
		::MoveHHi( h );
		MemError();
	}
	
	// 1058
	inline void DisposePtr( nucleus::owned< Ptr > )  {}
	
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
	inline void DisposeHandle( nucleus::owned< Handle > )  {}
	
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
		typedef Ptr Put_Parameter;
		
		template < class Putter >
		static void Put( Put_Parameter toPut, Putter put )
		{
			const char* begin = toPut;
			const std::size_t size = GetPtrSize( toPut );
			
			put( begin, begin + size );
		}
		
		typedef nucleus::owned< Ptr > Get_Result;
		
		template < class Getter >
		static Get_Result Get( Getter get )
		{
			const std::size_t size = get.size();
			Get_Result result = NewPtr( size );
			
			char* begin = result.get();
			
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
		
		template < class Putter >
		static void Put( Put_Parameter toPut, Putter put )
		{
			PtrFlattener::Put( Ptr( toPut ), put );
		}
		
		typedef nucleus::owned< PtrType > Get_Result;
		
		template < class Getter >
		static Get_Result Get( Getter get )
		{
			return Ptr_Cast< T >( PtrFlattener::Get( get ) );
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
	struct HandleFlattener
	{
		typedef Handle Put_Parameter;
		
		template < class Putter >
		static void Put( Put_Parameter toPut, Putter put )
		{
		#if !TARGET_API_MAC_OSX
			
			nucleus::saved< Handle_State > hState( Handle_State( toPut ) );
			
			HLock( toPut );
			
		#endif
			
			const char* begin = *toPut.Get();
			const std::size_t size = GetHandleSize( toPut );
			
			put( begin, begin + size );
		}
		
		typedef nucleus::owned< Handle > Get_Result;
		
		template < class Getter >
		static Get_Result Get( Getter get )
		{
			const std::size_t size = get.size();
			Get_Result result = NewHandle( size );
			
			if ( !TARGET_API_MAC_OSX )
			{
				// A new handle's state is known and need not be saved
				HLock( result );
			}
			
			char* begin = *result.get();
			
			get( begin, begin + size );
			
			if ( !TARGET_API_MAC_OSX )
			{
				// If we get this far, the handle will be unlocked as before.
				// On exception, the handle is disposed anyway, locked or not.
				HUnlock( result );
			}
			
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
		
		template < class Putter >
		static void Put( Put_Parameter toPut, Putter put )
		{
			HandleFlattener::Put( Handle( toPut ), put );
		}
		
		typedef nucleus::owned< HandleType > Get_Result;
		
		template < class Getter >
		static Get_Result Get( Getter get )
		{
			return Handle_Cast< T >( HandleFlattener::Get( get ) );
		}
		
		typedef Put_Parameter Parameter;
		typedef Get_Result    Result;
		
		static const bool hasStaticSize = false;
		struct Buffer {};
	};
	
}

#endif
