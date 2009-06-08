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

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/MacErrors.h"

// ClassicToolbox
#include "ClassicToolbox/Serial.h"

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
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
#if !TARGET_API_MAC_CARBON
	
	class SerialDeviceHandle : public DeviceHandle
	{
		private:
			NN::Shared< N::DriverRefNum > itsOutputRefNum;
			NN::Shared< N::DriverRefNum > itsInputRefNum;
		
		protected:
			bool IsShared() const  { return !itsOutputRefNum.Sole(); }
		
		public:
			SerialDeviceHandle( const std::string& portName );
			
			SerialDeviceHandle( const SerialDeviceHandle& other );
			
			virtual bool Preempted() const  { return false; }
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
	};
	
	class ActiveSerialDeviceHandle : public SerialDeviceHandle
	{
		public:
			ActiveSerialDeviceHandle( const std::string& portName ) : SerialDeviceHandle( portName )  {}
			
			ActiveSerialDeviceHandle( const SerialDeviceHandle& h ) : SerialDeviceHandle( h )  {}
	};
	
	class PassiveSerialDeviceHandle : public SerialDeviceHandle
	{
		public:
			PassiveSerialDeviceHandle( const std::string& portName ) : SerialDeviceHandle( portName )  {}
			
			PassiveSerialDeviceHandle( const SerialDeviceHandle& h ) : SerialDeviceHandle( h )  {}
			
			bool Preempted() const  { return IsShared(); }
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
	inline boost::shared_ptr< IOHandle > NewSerialDeviceHandle( Type param, bool isPassive )
	{
		SerialDeviceHandle* h = isPassive ? static_cast< SerialDeviceHandle* >( new PassiveSerialDeviceHandle( param ) )
		                                  : static_cast< SerialDeviceHandle* >( new ActiveSerialDeviceHandle ( param ) );
		
		return boost::shared_ptr< IOHandle >( h );
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
		                                                       : NewSerialDeviceHandle( static_cast< const SerialDeviceHandle& >( *other.lock() ), isPassive );
		
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
	
	inline bool SerialPortsAreArbitrated()
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
	
	static NN::Owned< N::DriverRefNum > OpenSerialDriver( const unsigned char* driverName )
	{
		if ( !SerialDriverMayBeOpened( driverName ) )
		{
			throw N::PortInUse();
		}
		
		return N::OpenDriver( driverName );
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const std::string& portName )
	:
		DeviceHandle( O_RDWR ),
		itsOutputRefNum( OpenSerialDriver( MakeDriverName( portName, STR_LEN( "Out" ) ) ) ),
		itsInputRefNum ( OpenSerialDriver( MakeDriverName( portName, STR_LEN( "In"  ) ) ) )
	{
		
		using N::kSERDHandshake;
		using N::baud19200;
		using N::data8;
		using N::noParity;
		using N::stop10;
		
		N::Control< kSERDHandshake >( itsOutputRefNum, NN::Make< N::SerShk >() );
		
		N::SerReset( itsOutputRefNum, baud19200 | data8 | noParity | stop10 );
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const SerialDeviceHandle& other )
	:
		DeviceHandle( O_RDWR ),
		itsOutputRefNum( other.itsOutputRefNum ),
		itsInputRefNum ( other.itsInputRefNum  )
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

