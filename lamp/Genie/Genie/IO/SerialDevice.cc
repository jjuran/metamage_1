/*	===============
 *	SerialDevice.cc
 *	===============
 */

#include "Genie/IO/SerialDevice.hh"

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

// Standard C++
#include <map>

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Nitrogen/Gestalt.hh"

// ClassicToolbox
#include "ClassicToolbox/Serial.hh"

// Genie
#include "Genie/Process.hh"


enum
{
	gestaltSerialPortArbitratorAttr = 'arb ',
	
	gestaltSerialPortArbitratorExists = 0
};

namespace Nitrogen
{
	
	static const GestaltSelector gestaltSerialPortArbitratorAttr = GestaltSelector( ::gestaltSerialPortArbitratorAttr );
	
	template <> struct GestaltDefault< gestaltSerialPortArbitratorAttr > : GestaltAttrDefaults {};
	
}

namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
#if !TARGET_API_MAC_CARBON
	
	class SerialDeviceHandle : public DeviceHandle
	{
		private:
			n::shared< N::DriverRefNum >  itsOutputRefNum;
			n::shared< N::DriverRefNum >  itsInputRefNum;
			bool                          itIsPassive;
		
		protected:
			bool IsShared() const  { return !itsOutputRefNum.sole(); }
		
		public:
			SerialDeviceHandle( const std::string& portName, bool passive );
			
			SerialDeviceHandle( const SerialDeviceHandle& other, bool passive );
			
			SerialDeviceHandle( const SerialDeviceHandle& other );
			
			bool Preempted() const  { return itIsPassive && IsShared(); }
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
	};
	
	struct SerialDevicePair
	{
		boost::weak_ptr< IOHandle > passive, active;
	};
	
	static SerialDevicePair& GetSerialDevicePair( const std::string& portName )
	{
		static std::map< std::string, SerialDevicePair > gSerialDevices;
		
		SerialDevicePair& pair = gSerialDevices[ portName ];
		
		return pair;
	}
	
	template < class Type >
	static inline boost::shared_ptr< IOHandle > NewSerialDeviceHandle( Type param, bool isPassive )
	{
		return seize_ptr( new SerialDeviceHandle( param, isPassive ) );
	}
	
#endif
	
	
	boost::shared_ptr< IOHandle > OpenSerialDevice( const std::string&  portName,
	                                                bool                isPassive,
	                                                bool                nonblocking )
	{
	#if TARGET_API_MAC_CARBON
		
		N::ThrowOSStatus( fnfErr );
		
		return boost::shared_ptr< IOHandle >();
		
	#else
		
		SerialDevicePair& pair = GetSerialDevicePair( portName );
		
		boost::weak_ptr< IOHandle >& same  = isPassive ? pair.passive : pair.active;
		boost::weak_ptr< IOHandle >& other = isPassive ? pair.active : pair.passive;
		
		while ( !same.expired() )
		{
			TryAgainLater( nonblocking );
		}
		
		boost::shared_ptr< IOHandle > result = other.expired() ? NewSerialDeviceHandle( portName, isPassive )
		                                                       : NewSerialDeviceHandle( static_cast< const SerialDeviceHandle& >( *other.lock().get() ), isPassive );
		
		same = result;
		
		return result;
		
	#endif
	}
	
#if !TARGET_API_MAC_CARBON
	
	static N::Str255 MakeDriverName( const std::string&   port_name,
	                                 const char          *direction,
	                                 size_t               direction_length )
	{
		const size_t total_length = 1 + port_name.length() + direction_length;
		
		if ( total_length > 255 )
		{
			N::ThrowOSStatus( bdNamErr );
		}
		
		Str255 result = { total_length, '.' };
		
		memcpy( &result[ 2 ], port_name.data(), port_name.size() );
		
		memcpy( &result[ 2 + port_name.size() ], direction, direction_length );
		
		return result;
	}
	
	static inline bool SerialPortsAreArbitrated()
	{
		return N::Gestalt_Bit< N::gestaltSerialPortArbitratorAttr, ::gestaltSerialPortArbitratorExists >();
	}
	
	static bool DriverIsOpen( const unsigned char* driverName )
	{
		for ( int unit = 0;  unit < LMGetUnitTableEntryCount();  ++unit )
		{
			DCtlHandle dceHandle = GetDCtlEntry( -unit );
			
			if ( dceHandle != NULL  &&  dceHandle[0]->dCtlDriver != NULL )
			{
				const bool ramBased = dceHandle[0]->dCtlFlags & dRAMBasedMask;
				
				DRVRHeaderPtr header = ramBased ? *reinterpret_cast< DRVRHeader** >( dceHandle[0]->dCtlDriver )
				                                :  reinterpret_cast< DRVRHeader*  >( dceHandle[0]->dCtlDriver );
				
				ConstStr255Param name = header->drvrName;
				
				if ( ::EqualString( name, driverName, false, true ) )
				{
					return dceHandle[0]->dCtlFlags & dOpenedMask;
				}
			}
		}
		
		return false;
	}
	
	static bool SerialDriverMayBeOpened( const unsigned char* driverName )
	{
		return SerialPortsAreArbitrated() || !DriverIsOpen( driverName );
	}
	
	static n::owned< N::DriverRefNum > OpenSerialDriver( const unsigned char* driverName )
	{
		if ( !SerialDriverMayBeOpened( driverName ) )
		{
			N::ThrowOSStatus( portInUse );
		}
		
		return N::OpenDriver( driverName );
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const std::string& portName, bool passive )
	:
		DeviceHandle( O_RDWR ),
		itsOutputRefNum( OpenSerialDriver( MakeDriverName( portName, STR_LEN( "Out" ) ) ) ),
		itsInputRefNum ( OpenSerialDriver( MakeDriverName( portName, STR_LEN( "In"  ) ) ) ),
		itIsPassive( passive )
	{
		
		using N::kSERDHandshake;
		using N::baud19200;
		using N::data8;
		using N::noParity;
		using N::stop10;
		
		N::Control< kSERDHandshake >( itsOutputRefNum, n::make< N::SerShk >() );
		
		N::SerReset( itsOutputRefNum, baud19200 | data8 | noParity | stop10 );
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const SerialDeviceHandle& other, bool passive )
	:
		DeviceHandle( O_RDWR ),
		itsOutputRefNum( other.itsOutputRefNum ),
		itsInputRefNum ( other.itsInputRefNum  ),
		itIsPassive    ( passive               )
	{
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const SerialDeviceHandle& other )
	:
		DeviceHandle( O_RDWR ),
		itsOutputRefNum( other.itsOutputRefNum ),
		itsInputRefNum ( other.itsInputRefNum  ),
		itIsPassive    ( other.itIsPassive     )
	{
	}
	
	unsigned int SerialDeviceHandle::SysPoll()
	{
		bool unblocked = !Preempted();
		
		bool readable = unblocked  &&  N::SerGetBuf( itsInputRefNum ) > 0;
		bool writable = unblocked;
		
		unsigned readability = readable * kPollRead;
		unsigned writability = writable * kPollWrite;
		
		return readability | writability;
	}
	
	ssize_t SerialDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		while ( true )
		{
			if ( !Preempted() )
			{
				if ( byteCount == 0 )
				{
					return 0;
				}
				
				if ( std::size_t bytesAvailable = N::SerGetBuf( itsInputRefNum ) )
				{
					byteCount = std::min( byteCount, bytesAvailable );
					
					break;
				}
			}
			
			TryAgainLater();
		}
		
		return N::Read( itsInputRefNum, data, byteCount );
	}
	
	ssize_t SerialDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		while ( Preempted() )
		{
			TryAgainLater();
		}
		
		return N::Write( itsOutputRefNum, data, byteCount );
	}
	
#endif
	
}

