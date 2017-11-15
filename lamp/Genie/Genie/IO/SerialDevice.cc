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

// more-libc
#include "more/string.h"

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/is_driver_open.hh"

// plus
#include "plus/string.hh"

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
	
	class serial_driver_pair
	{
		private:
			typedef struct unspecified* boolean;
			
			short its_output_refnum;
			short its_input_refnum;
		
		public:
			serial_driver_pair() : its_output_refnum(), its_input_refnum()
			{
			}
			
			serial_driver_pair( const plus::string& base_name );
			
		#if ! TARGET_API_MAC_CARBON
			
			void destroy() const
			{
				::CloseDriver( its_input_refnum  );
				::CloseDriver( its_output_refnum );
			}
			
			typedef Mac::DriverRefNum MacType;
			
			MacType output() const  { return MacType( its_output_refnum ); }
			MacType input () const  { return MacType( its_input_refnum  ); }
			
		#endif
			
			operator boolean() const  { return (boolean) (its_output_refnum < 0); }
	};
	
}

namespace nucleus
{
	
	template <> struct disposer< Genie::serial_driver_pair >
	{
		typedef Genie::serial_driver_pair  argument_type;
		typedef void                       result_type;
		
		void operator()( const Genie::serial_driver_pair& pair ) const
		{
		#if ! TARGET_API_MAC_CARBON
			
			pair.destroy();
			
		#endif
		}
	};
	
}

namespace Genie
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
	}
	
	
	struct serial_device_extra
	{
		plus::string                     base_name;
		n::shared< serial_driver_pair >  drivers;
		bool                             passive;
		
		serial_device_extra( const plus::string&                     name,
		                     const n::shared< serial_driver_pair >&  drivers,
		                     bool                                    passive )
		:
			base_name( name ),
			drivers( drivers ),
			passive( passive )
		{
		}
		
		~serial_device_extra();
	};
	
	serial_device_extra::~serial_device_extra()
	{
	}
	
	static
	bool preempted( const serial_device_extra& extra )
	{
		return extra.passive  &&  ! extra.drivers.sole();
	}
	
	class SerialDeviceHandle : public vfs::filehandle
	{
		public:
			SerialDeviceHandle( const plus::string& portName, bool passive );
			
			SerialDeviceHandle( const SerialDeviceHandle& other, bool passive );
			
			~SerialDeviceHandle();
			
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
	
	static inline bool SerialPortsAreArbitrated()
	{
		return mac::sys::gestalt_bit_set( gestaltSerialPortArbitratorAttr, gestaltSerialPortArbitratorExists );
	}
	
	static bool SerialDriverMayBeOpened( const unsigned char* driverName )
	{
		using mac::sys::is_driver_open;
		
		return SerialPortsAreArbitrated()  ||  ! is_driver_open( driverName );
	}
	
	serial_driver_pair::serial_driver_pair( const plus::string& base_name )
	{
		const size_t max_basename_len = 255 - 1 - 3;  // ".<basename>Out"
		
		const char*   base_addr = base_name.data();
		const size_t  base_size = base_name.size();
		
		if ( base_size > max_basename_len )
		{
			p7::throw_errno( ENAMETOOLONG );
		}
		
		Str255 driver_name;
		
		uint8_t* p = driver_name;
		
		*p++ = base_size + 1 + 3;
		
		*p++ = '.';
		
		p = (uint8_t*) mempcpy( p, base_addr, base_size );
		
		*p++ = 'O';
		*p++ = 'u';
		*p   = 't';
		
	#if ! TARGET_API_MAC_CARBON
		
		if ( ! SerialDriverMayBeOpened( driver_name ) )
		{
			Mac::ThrowOSStatus( portInUse );
		}
		
		Mac::ThrowOSStatus( OpenDriver( driver_name, &its_output_refnum ) );
		
	#endif
		
		*--p = 'n';
		*--p = 'I';
		
		--driver_name[ 0 ];
		
	#if ! TARGET_API_MAC_CARBON
		
		if ( OSErr err = OpenDriver( driver_name, &its_input_refnum ) )
		{
			::CloseDriver( its_output_refnum );
			
			Mac::ThrowOSStatus( err );
		}
		
	#endif
	}
	
	typedef serial_driver_pair dyad;
	
	SerialDeviceHandle::SerialDeviceHandle( const plus::string& portName, bool passive )
	:
		vfs::filehandle( O_RDWR, &serial_methods, sizeof (serial_device_extra) )
	{
		n::shared< dyad > drivers = n::owned< dyad >::seize( dyad( portName ) );
		
	#if ! TARGET_API_MAC_CARBON
		
		const Mac::DriverRefNum output = drivers.get().output();
		
		namespace N = Nitrogen;
		
		using N::kSERDHandshake;
		using N::baud19200;
		using N::data8;
		using N::noParity;
		using N::stop10;
		
		N::Control< kSERDHandshake >( output, n::make< SerShk >() );
		
		N::SerReset( output, baud19200 | data8 | noParity | stop10 );
		
	#endif
		
		serial_device_extra& extra = *(serial_device_extra*) this->extra();
		
		new (&extra) serial_device_extra( portName, drivers, passive );
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const SerialDeviceHandle& other, bool passive )
	:
		vfs::filehandle( O_RDWR, &serial_methods, sizeof (serial_device_extra) )
	{
		serial_device_extra const* extra = (serial_device_extra*) other.extra();
		serial_device_extra*   new_extra = (serial_device_extra*) this->extra();
		
		new (new_extra) serial_device_extra( extra->base_name,
		                                     extra->drivers,
		                                     passive );
	}
	
	SerialDeviceHandle::~SerialDeviceHandle()
	{
		serial_device_extra& extra = *(serial_device_extra*) this->extra();
		
		UnreferenceSerialDevice( extra.base_name, extra.passive );
		
		extra.~serial_device_extra();
	}
	
	unsigned int SerialDeviceHandle::SysPoll()
	{
		serial_device_extra& extra = *(serial_device_extra*) this->extra();
		
	#if ! TARGET_API_MAC_CARBON
		
		namespace N = Nitrogen;
		
		const Mac::DriverRefNum input = extra.drivers.get().input();
		
		bool unblocked = ! preempted( extra );
		
		bool readable = unblocked  &&  N::SerGetBuf( input ) > 0;
		bool writable = unblocked;
		
		unsigned readability = readable * vfs::Poll_read;
		unsigned writability = writable * vfs::Poll_write;
		
		return readability | writability;
		
	#endif
		
		return 0;
	}
	
	ssize_t SerialDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		serial_device_extra& extra = *(serial_device_extra*) this->extra();
		
	#if ! TARGET_API_MAC_CARBON
		
		namespace N = Nitrogen;
		
		const Mac::DriverRefNum input = extra.drivers.get().input();
		
		while ( true )
		{
			if ( ! preempted( extra ) )
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
		
	#endif
		
		return 0;
	}
	
	ssize_t SerialDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		serial_device_extra& extra = *(serial_device_extra*) this->extra();
		
	#if ! TARGET_API_MAC_CARBON
		
		namespace N = Nitrogen;
		
		const Mac::DriverRefNum output = extra.drivers.get().output();
		
		while ( preempted( extra ) )
		{
			relix::try_again( is_nonblocking( *this ) );
		}
		
		return N::Write( output, data, byteCount );
		
	#endif
		
		return 0;
	}
	
}
