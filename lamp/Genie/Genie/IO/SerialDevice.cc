/*	===============
 *	SerialDevice.cc
 *	===============
 */

#include "Genie/IO/SerialDevice.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#if ! TARGET_API_MAC_CARBON
#ifndef __SERIAL__
#include <Serial.h>
#endif
#endif

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
#include "ClassicToolbox/Devices.hh"
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

static
long serial_bytes_buffered( short refnum )
{
#if ! TARGET_API_MAC_CARBON
	
	long count;
	
	OSErr err = SerGetBuf( refnum, &count );
	
	return err == noErr ? count : 0;
	
#endif
	
	return 0;
}

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
	
	static
	void destroy_serial_device( vfs::filehandle* that );
	
	
	static
	unsigned serial_poll( vfs::filehandle* that );
	
	static
	ssize_t serial_read( vfs::filehandle* that, char* buffer, size_t n );
	
	static
	ssize_t serial_write( vfs::filehandle* that, const char* buffer, size_t n );
	
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
	
	static
	vfs::filehandle* fresh_device( const plus::string& name, bool passive );
	
	static
	vfs::filehandle* other_device( vfs::filehandle& other, bool passive );
	
	
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
		
		Byte* p = driver_name;
		
		*p++ = base_size + 1 + 3;
		
		*p++ = '.';
		
		p = (Byte*) mempcpy( p, base_addr, base_size );
		
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
	
	static
	void destroy_serial_device( vfs::filehandle* that )
	{
		serial_device_extra& extra = *(serial_device_extra*) that->extra();
		
		UnreferenceSerialDevice( extra.base_name, extra.passive );
		
		extra.~serial_device_extra();
	}
	
	static
	unsigned serial_poll( vfs::filehandle* that )
	{
		serial_device_extra& extra = *(serial_device_extra*) that->extra();
		
	#if ! TARGET_API_MAC_CARBON
		
		const Mac::DriverRefNum input = extra.drivers.get().input();
		
		bool unblocked = ! preempted( extra );
		
		bool readable = unblocked  &&  serial_bytes_buffered( input ) > 0;
		bool writable = unblocked;
		
		unsigned readability = readable * vfs::Poll_read;
		unsigned writability = writable * vfs::Poll_write;
		
		return readability | writability;
		
	#endif
		
		return 0;
	}
	
	static
	ssize_t serial_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		serial_device_extra& extra = *(serial_device_extra*) that->extra();
		
	#if ! TARGET_API_MAC_CARBON
		
		namespace N = Nitrogen;
		
		const Mac::DriverRefNum input = extra.drivers.get().input();
		
		while ( true )
		{
			if ( ! preempted( extra ) )
			{
				if ( n == 0 )
				{
					return 0;
				}
				
				if ( size_t bytesAvailable = serial_bytes_buffered( input ) )
				{
					n = min( n, bytesAvailable );
					
					break;
				}
			}
			
			relix::try_again( is_nonblocking( *that ) );
		}
		
		return N::Read( input, buffer, n );
		
	#endif
		
		return 0;
	}
	
	static
	ssize_t serial_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		serial_device_extra& extra = *(serial_device_extra*) that->extra();
		
	#if ! TARGET_API_MAC_CARBON
		
		namespace N = Nitrogen;
		
		const Mac::DriverRefNum output = extra.drivers.get().output();
		
		while ( preempted( extra ) )
		{
			relix::try_again( is_nonblocking( *that ) );
		}
		
		return N::Write( output, buffer, n );
		
	#endif
		
		return 0;
	}
	
	typedef serial_driver_pair dyad;
	
	static
	vfs::filehandle* new_device( const plus::string&       name,
	                             const n::shared< dyad >&  drivers,
	                             bool                      passive )
	{
		vfs::filehandle* result = new vfs::filehandle( NULL,
		                                               O_RDWR,
		                                               &serial_methods,
		                                               sizeof (serial_device_extra),
		                                               &destroy_serial_device );
		
		serial_device_extra& extra = *(serial_device_extra*) result->extra();
		
		// doesn't throw
		new (&extra) serial_device_extra( name, drivers, passive );
		
		return result;
	}
	
	static
	vfs::filehandle* fresh_device( const plus::string& portName, bool passive )
	{
		n::shared< dyad > drivers = n::owned< dyad >::seize( dyad( portName ) );
		
	#if ! TARGET_API_MAC_CARBON
		
		static SerShk handshaking;
		
		const Mac::DriverRefNum output = drivers.get().output();
		
		OSErr err;
		
		(err = Control ( output, kSERDHandshake, &handshaking ))  ||
		(err = SerReset( output, baud19200 | data8 | noParity | stop10 ));
		
		Mac::ThrowOSStatus( err );
		
	#endif
		
		return new_device( portName, drivers, passive );
	}
	
	static
	vfs::filehandle* other_device( vfs::filehandle& other, bool passive )
	{
		serial_device_extra const* extra = (serial_device_extra*) other.extra();
		
		return new_device( extra->base_name, extra->drivers, passive );
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
	
}
