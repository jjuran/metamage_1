/*	==========
 *	Devices.cc
 *	==========
 */

#include "Genie/Devices.hh"

// Standard C++ library
#include <algorithm>

// Io
#include "io/io.hh"

// Nitrogen
#include "Nitrogen/MacErrors.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/IO/SimpleDevice.hh"
#include "Genie/SystemConsole.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace P7 = POSeven;
	
	// General
	
	static int ReadNull( char*, std::size_t )
	{
		throw io::end_of_input();
	}
	
	static int WriteVoid( const char*, std::size_t byteCount )
	{
		return byteCount;
	}
	
	// Zero
	
	static int ReadZero( char* data, std::size_t byteCount )
	{
		std::fill( data,
		           data + byteCount,
		           '\0' );
		
		return byteCount;
	}
	
	// Console
	
	static int WriteConsole( const char* data, std::size_t byteCount )
	{
		return WriteToSystemConsole( data, byteCount );
	}
	
	
	typedef int (*Reader)( char*, std::size_t );
	typedef int (*Writer)( const char*, std::size_t );
	
	static DeviceIOSpec gDeviceIOSpecs[] =
	{
		{ "null",    ReadNull, WriteVoid    },
		{ "zero",    ReadZero, WriteVoid    },
		{ "console", ReadNull, WriteConsole },
		{ NULL,      NULL,     NULL         }
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
	
	boost::shared_ptr< IOHandle > GetSimpleDeviceHandle( const std::string& path )
	{
		DeviceIOMap::const_iterator found = DeviceMap().find( path );
		
		if ( found == DeviceMap().end() )
		{
			throw N::FNFErr();
		}
		
		return boost::shared_ptr< IOHandle >( new SimpleDeviceHandle( found->second ) );
	}
	
}

