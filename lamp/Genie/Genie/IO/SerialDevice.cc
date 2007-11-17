/*	===============
 *	SerialDevice.cc
 *	===============
 */

#include "Genie/IO/SerialDevice.hh"

// Standard C++
#include <map>

// Boost
#include "boost/shared_ptr.hpp"

// Nucleus
#include "Nucleus/Shared.h"

// ClassicToolbox
#include "ClassicToolbox/Serial.h"

// Genie
#include "Genie/Process.hh"


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
			
			virtual bool Preempted() const  { return false; }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
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
	
	
	boost::shared_ptr< IOHandle > OpenSerialDevice( const std::string& portName, bool isPassive )
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
			// FIXME:  allow non-blocking opens
			Yield();
		}
		
		boost::shared_ptr< IOHandle > result = other.expired() ? NewSerialDeviceHandle( portName, isPassive )
		                                                       : NewSerialDeviceHandle( static_cast< const SerialDeviceHandle& >( *other.lock() ), isPassive );
		
		same = result;
		
		return result;
		
	#endif
	}
	
#if !TARGET_API_MAC_CARBON
	
	static std::string MakeDriverName( const std::string&  portName,
	                                   const std::string&  directionName )
	{
		return "." + portName + directionName;
	}
	
	SerialDeviceHandle::SerialDeviceHandle( const std::string& portName ) : itsOutputRefNum( N::OpenDriver( MakeDriverName( portName, "Out" ) ) ),
	                                                                        itsInputRefNum ( N::OpenDriver( MakeDriverName( portName, "In"  ) ) )
	{
		
		using N::kSERDHandshake;
		using N::baud19200;
		using N::data8;
		using N::noParity;
		using N::stop10;
		
		N::Control< kSERDHandshake >( itsOutputRefNum, NN::Make< N::SerShk >() );
		
		N::SerReset( itsOutputRefNum, baud19200 | data8 | noParity | stop10 );
	}
	
	unsigned int SerialDeviceHandle::SysPoll() const
	{
		bool unblocked = !Preempted();
		
		bool readable = unblocked  &&  N::SerGetBuf( itsInputRefNum ) > 0;
		bool writable = unblocked;
		
		unsigned readability = readable * kPollRead;
		unsigned writability = writable * kPollWrite;
		
		return readability | writability | kPollExcept;
	}
	
	int SerialDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		while ( true )
		{
			if ( !Preempted() )
			{
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
	
	int SerialDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		while ( Preempted() )
		{
			TryAgainLater();
		}
		
		return N::Write( itsOutputRefNum, data, byteCount );
	}
	
#endif
	
}

