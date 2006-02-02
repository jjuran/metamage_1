/*	=============
 *	MakeHandle.hh
 *	=============
 */

#pragma once

#ifndef IO_MAKEHANDLE_HH
#define IO_MAKEHANDLE_HH

// Io
#include "Io/Handle.hh"


namespace Io
{
	
	template < class Object, class Details >
	struct Copying
	{
		static void SetBlocking( void* object, bool blocking )
		{
			return Details::SetBlocking( *reinterpret_cast< Object* >( object ), blocking );
		}
		
		static int Read ( void* object,       char* data, std::size_t byteCount )
		{
			return Details::Read( *reinterpret_cast< Object* >( object ), data, byteCount );
		}
		
		static int Write( void* object, const char* data, std::size_t byteCount )
		{
			return Details::Write( *reinterpret_cast< Object* >( object ), data, byteCount );
		}
		
		static void Delete( void* object )
		{
			delete reinterpret_cast< Object* >( object );
		}
		
		static Handle Make( Object object )
		{
			return Handle( new Object( object ), SetBlocking, Read, Write, Delete );
		}
	};
	
	template < class Object, class Details >
	struct Casting
	{
		typedef typename Details::Storage  Storage;
		typedef typename Details::Value    Value;
		
		static void SetBlocking( void* object, bool blocking )
		{
			return Details::SetBlocking( Value( reinterpret_cast< Storage >( object ) ),
			                             blocking );
		}
		
		static int Read ( void* object,       char* data, std::size_t byteCount )
		{
			return Details::Read ( Value( reinterpret_cast< Storage >( object ) ),
			                       data,
			                       byteCount );
		}
		
		static int Write( void* object, const char* data, std::size_t byteCount )
		{
			return Details::Write( Value( reinterpret_cast< Storage >( object ) ),
			                       data,
			                       byteCount );
		}
		
		static void Delete( void* object )  {}
		
		static Handle Make( Object object )
		{
			return Handle( reinterpret_cast< void* >( object ), SetBlocking, Read, Write, Delete );
		}
	};
	
	template < class Details,
	           template < class, class > class Policy,
	           class Object >
	Handle MakeHandle( Object object )
	{
		return Policy< Object, Details >::Make( object );
	}
	
	template < class Details, class Object >
	Handle MakeHandleFromCopy( Object object )
	{
		return Copying< Object, Details >::Make( object );
	}
	
	template < class Details, class Object >
	Handle MakeHandleFromCast( Object object )
	{
		return Casting< Object, Details >::Make( object );
	}
	
}

#endif

