/*	==========
 *	Devices.cc
 *	==========
 */

#include "Genie/Devices.hh"

// Standard C++ library
#include <algorithm>
#include <map>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/SystemConsole.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	// General
	
	static ssize_t ReadNull( char*, std::size_t )
	{
		return 0;
	}
	
	static ssize_t WriteVoid( const char*, std::size_t byteCount )
	{
		return byteCount;
	}
	
	// Zero
	
	static ssize_t ReadZero( char* data, std::size_t byteCount )
	{
		std::fill( data,
		           data + byteCount,
		           '\0' );
		
		return byteCount;
	}
	
	// Console
	
	static ssize_t WriteConsole( const char* data, std::size_t byteCount )
	{
		return WriteToSystemConsole( data, byteCount );
	}
	
	
	typedef ssize_t (*Reader)( char      *, std::size_t );
	typedef ssize_t (*Writer)( char const*, std::size_t );
	
	static DeviceIOSpec gDeviceIOSpecs[] =
	{
		{ "null",    ReadNull, WriteVoid    },
		{ "zero",    ReadZero, WriteVoid    },
		{ "console", ReadNull, WriteConsole },
		{ NULL,      NULL,     NULL         }
	};
	
	typedef std::map< std::string, const DeviceIOSpec* > DeviceIOMap;
	
	static DeviceIOMap BuildDeviceMap()
	{
		DeviceIOMap result;
		
		for ( DeviceIOSpec* dev = gDeviceIOSpecs;  dev->name != NULL;  ++dev )
		{
			result[ dev->name ] = dev;
		}
		
		return result;
	}
	
	static DeviceIOMap& DeviceMap()
	{
		static DeviceIOMap gDeviceMap = BuildDeviceMap();
		
		return gDeviceMap;
	}
	
	boost::shared_ptr< IOHandle > GetSimpleDeviceHandle( const std::string& path )
	{
		DeviceIOMap::const_iterator found = DeviceMap().find( path );
		
		if ( found == DeviceMap().end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return seize_ptr( new SimpleDeviceHandle( *found->second ) );
	}
	
}

