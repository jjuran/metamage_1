/*	==========
 *	Devices.cc
 *	==========
 */

#include "Genie/Devices.hh"

// Standard C++ library
#include <algorithm>

// Nitrogen
#include "Nitrogen/MacErrors.h"

// Io
#include "Io/Exceptions.hh"
#include "Io/Handle.hh"

// Genie
#include "Genie/IOHandle.hh"
#include "Genie/SystemConsole.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	// General
	
	static int ReadNull( void*, char*, std::size_t )
	{
		throw Io::EndOfInput();
	}
	
	static int WriteVoid( void*, const char*, std::size_t byteCount )
	{
		return byteCount;
	}
	
	static void DeleteNothing( void* )
	{
	}
	
	// Zero
	
	static int ReadZero( void*, char* data, std::size_t byteCount )
	{
		std::fill( data,
		           data + byteCount,
		           '\0' );
		
		return byteCount;
	}
	
	// Console
	
	static int WriteConsole( void*, const char* data, std::size_t byteCount )
	{
		return WriteToSystemConsole( data, byteCount );
	}
	
	
	typedef int (*Reader)( void*, char*, std::size_t );
	typedef int (*Writer)( void*, const char*, std::size_t );
	
	struct DeviceIOSpec
	{
		const char* name;
		Reader reader;
		Writer writer;
	};
	
	static DeviceIOSpec gDeviceIOSpecs[] =
	{
		{ "/dev/null",    ReadNull, WriteVoid    },
		{ "/dev/zero",    ReadZero, WriteVoid    },
		{ "/dev/console", ReadNull, WriteConsole },
		{ NULL,           NULL,     NULL         }
	};
	
	
	
	typedef std::map< std::string, DeviceIOSpec > DeviceIOMap;
	
	static DeviceIOMap BuildDeviceMap()
	{
		DeviceIOMap result;
		
		for ( DeviceIOSpec* dev = gDeviceIOSpecs;  dev->name != NULL;  ++dev )
		{
			result[ dev->name ] = *dev;
		}
		
		return result;
	}
	
	static DeviceIOMap& DeviceMap()
	{
		static DeviceIOMap gDeviceMap = BuildDeviceMap();
		
		return gDeviceMap;
	}
	
	IORef GetSimpleDeviceHandle( const char* path )
	{
		DeviceIOMap::const_iterator found = DeviceMap().find( path );
		
		if ( found == DeviceMap().end() )
		{
			throw N::FNFErr();
		}
		
		const DeviceIOSpec& dev = found->second;
		
		void* const object = NULL;
		
		return NewGenericIO( Io::Handle( object,
		                                 Io::SetBlockingNoOp,
		                                 dev.reader,
		                                 dev.writer,
		                                 DeleteNothing ) );
	}
	
}

