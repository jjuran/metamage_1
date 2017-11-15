/*	===============
 *	SerialDevice.cc
 *	===============
 */

#include "Genie/IO/SerialDevice.hh"

// Standard C++
#include <map>

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/is_driver_open.hh"

// nucleus
#include "nucleus/shared.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// ClassicToolbox
#if !TARGET_API_MAC_CARBON
#include "ClassicToolbox/Serial.hh"
#endif

// vfs
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"

// relix-kernel
#include "relix/api/try_again.hh"


enum
{
	gestaltSerialPortArbitratorAttr = 'arb ',
	
	gestaltSerialPortArbitratorExists = 0
};

namespace Genie
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
#if !TARGET_API_MAC_CARBON
	
	namespace N = Nitrogen;
	
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
	}
	
	
	class SerialDeviceHandle : public vfs::filehandle
	{
		private:
			plus::string                    itsPortName;
			n::shared< Mac::DriverRefNum >  itsOutputRefNum;
			n::shared< Mac::DriverRefNum >  itsInputRefNum;
			bool                            itIsPassive;
		
		protected:
			bool IsShared() const  { return !itsOutputRefNum.sole(); }
		
		public:
			SerialDeviceHandle( const plus::string& portName, bool passive );
			
			SerialDeviceHandle( const SerialDeviceHandle& other, bool passive );
			
			~SerialDeviceHandle();
			
			bool Preempted() const  { return itIsPassive && IsShared(); }
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
	};
	
	
	static unsigned serial_poll( vfs::filehandle* that )
	{
		return static_cast< SerialDeviceHandle& >( *that ).SysPoll();
	}
	
	static ssize_t serial_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		return static_cast< SerialDeviceHandle& >( *that ).SysRead( buffer, n );
	}
	
	static ssize_t serial_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		return static_cast< SerialDeviceHandle& >( *that ).SysWrite( buffer, n );
	}
	
	static const vfs::stream_method_set serial_stream_methods =
	{
		&serial_poll,
		&serial_read,
		&serial_write,
	};
	
	static const vfs::filehandle_method_set serial_methods =
	{
		NULL,
		NULL,
		&serial_stream_methods,
	};
	
	
	struct SerialDevicePair
	{
		vfs::filehandle*  passive;
		vfs::filehandle*  active;
		
		SerialDevicePair() : passive(), active()
		{
		}
	};
	
	typedef std::map< plus::string, SerialDevicePair > SerialDeviceMap;
	
	static SerialDeviceMap gSerialDevices;
	
	static SerialDevicePair& GetSerialDevicePair( const plus::string& portName )
	{
		SerialDevicePair& pair = gSerialDevices[ portName ];
		
		return pair;
	}
	
	static void UnreferenceSerialDevice( const plus::string& portName, bool isPassive )
	{
		SerialDeviceMap::iterator it = gSerialDevices.find( portName );
		
		if ( it != gSerialDevices.end() )
		{
			SerialDevicePair& pair = it->second;
			
			(isPassive ? pair.passive : pair.active) = NULL;
		}
	}
	
	static inline
	vfs::filehandle* fresh_device( const plus::string& param, bool isPassive )
	{
		return new SerialDeviceHandle( param, isPassive );
	}
	
	static inline
	vfs::filehandle* other_device( vfs::filehandle& other, bool isPassive )
	{
		SerialDeviceHandle& counterpart = (SerialDeviceHandle&) other;
		
		return new SerialDeviceHandle( counterpart, isPassive );
	}
	
#endif
	
	
	vfs::filehandle_ptr OpenSerialDevice( const plus::string&  portName,
	                                      bool                 isPassive,
	                                      bool                 nonblocking )
	{
	#if TARGET_API_MAC_CARBON
		
		p7::throw_errno( ENOENT );
		
		return vfs::filehandle_ptr();
		
	#else
		
		SerialDevicePair& pair = GetSerialDevicePair( portName );
		
		vfs::filehandle*& same  = isPassive ? pair.passive : pair.active;
		vfs::filehandle*  other = isPassive ? pair.active : pair.passive;
		
		while ( same != NULL )
		{
			relix::try_again( nonblocking );
		}
		
		vfs::filehandle* result = ! other ? fresh_device( portName, isPassive )
		                                  : other_device( *other,   isPassive );
		
		same = result;
		
		return result;
		
	#endif
	}
	
#if !TARGET_API_MAC_CARBON
	
	static N::Str255 MakeDriverName( const plus::string&   port_name,
	                                 const char           *direction,
	                                 size_t                direction_length )
	{
		const size_t total_length = 1 + port_name.length() + direction_length;
		
		if ( total_length > 255 )
		{
			Mac::ThrowOSStatus( bdNamErr );
		}
		
		Str255 result = { total_length, '.' };
		
		memcpy( &result[ 2 ], port_name.data(), port_name.size() );
		
		memcpy( &result[ 2 + port_name.size() ], direction, direction_length );
		
		return result;
	}
	
	static inline bool SerialPortsAreArbitrated()
	{
		return mac::sys::gestalt_bit_set( gestaltSerialPortArbitratorAttr, gestaltSerialPortArbitratorExists );
	}
	
	static bool SerialDriverMayBeOpened( const unsigned char* driverName )
	{
		using mac::sys::is_driver_open;
		
		return SerialPortsAreArbitrated()  ||  ! is_driver_open( driverName );
	}
	
	static n::owned< Mac::DriverRefNum > OpenSerialDriver( const unsigned char* driverName )
	{
		if ( !SerialDriverMayBeOpened( driverName ) )
		{
			Mac::ThrowOSStatus( portInUse );
		}
		
		return N::OpenDriver( driverName );
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const plus::string& portName, bool passive )
	:
		vfs::filehandle( O_RDWR, &serial_methods ),
		itsPortName( portName ),
		itsOutputRefNum( OpenSerialDriver( MakeDriverName( portName, STR_LEN( "Out" ) ) ) ),
		itsInputRefNum ( OpenSerialDriver( MakeDriverName( portName, STR_LEN( "In"  ) ) ) ),
		itIsPassive( passive )
	{
		const Mac::DriverRefNum output = itsOutputRefNum;
		
		using N::kSERDHandshake;
		using N::baud19200;
		using N::data8;
		using N::noParity;
		using N::stop10;
		
		N::Control< kSERDHandshake >( output, n::make< SerShk >() );
		
		N::SerReset( output, baud19200 | data8 | noParity | stop10 );
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const SerialDeviceHandle& other, bool passive )
	:
		vfs::filehandle( O_RDWR, &serial_methods ),
		itsPortName( other.itsPortName ),
		itsOutputRefNum( other.itsOutputRefNum ),
		itsInputRefNum ( other.itsInputRefNum  ),
		itIsPassive    ( passive               )
	{
	}
	
	SerialDeviceHandle::~SerialDeviceHandle()
	{
		UnreferenceSerialDevice( itsPortName, itIsPassive );
	}
	
	unsigned int SerialDeviceHandle::SysPoll()
	{
		const Mac::DriverRefNum input = itsInputRefNum;
		
		bool unblocked = !Preempted();
		
		bool readable = unblocked  &&  N::SerGetBuf( input ) > 0;
		bool writable = unblocked;
		
		unsigned readability = readable * vfs::Poll_read;
		unsigned writability = writable * vfs::Poll_write;
		
		return readability | writability;
	}
	
	ssize_t SerialDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		const Mac::DriverRefNum input = itsInputRefNum;
		
		while ( true )
		{
			if ( !Preempted() )
			{
				if ( byteCount == 0 )
				{
					return 0;
				}
				
				if ( std::size_t bytesAvailable = N::SerGetBuf( input ) )
				{
					byteCount = min( byteCount, bytesAvailable );
					
					break;
				}
			}
			
			relix::try_again( is_nonblocking( *this ) );
		}
		
		return N::Read( input, data, byteCount );
	}
	
	ssize_t SerialDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		const Mac::DriverRefNum output = itsOutputRefNum;
		
		while ( Preempted() )
		{
			relix::try_again( is_nonblocking( *this ) );
		}
		
		return N::Write( output, data, byteCount );
	}
	
#endif
	
}
