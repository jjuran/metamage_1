/*	==========
 *	Devices.cc
 *	==========
 */

#include "Genie/Devices.hh"

// Standard C++ library
#include <algorithm>

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
	
	static inline bool operator==( const DeviceIOSpec& device, const plus::string& s )
	{
		return device.name == s;
	}
	
	static const DeviceIOSpec* FindDevice( const plus::string& name )
	{
		const DeviceIOSpec* begin = gDeviceIOSpecs;
		const DeviceIOSpec* end   = begin + sizeof gDeviceIOSpecs / sizeof gDeviceIOSpecs[0];
		
		const DeviceIOSpec* it = std::find( begin, end, name );
		
		return it != end ? it : NULL;
	}
	
	boost::shared_ptr< IOHandle > GetSimpleDeviceHandle( const plus::string& path )
	{
		if ( const DeviceIOSpec* device = FindDevice( path ) )
		{
			return seize_ptr( new SimpleDeviceHandle( *device ) );
		}
		
		throw p7::errno_t( ENOENT );
	}
	
}

