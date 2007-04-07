/*	===============
 *	SerialDevice.cc
 *	===============
 */

#include "Genie/IO/SerialDevice.hh"

// Boost
#include "boost/shared_ptr.hpp"

// ClassicToolbox
#include "ClassicToolbox/Serial.h"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
#if !TARGET_API_MAC_CARBON
	
	class SerialDeviceHandle : public DeviceHandle
	{
		private:
			NN::Owned< N::DriverRefNum > itsOutputRefNum;
			NN::Owned< N::DriverRefNum > itsInputRefNum;
		
		public:
			SerialDeviceHandle( const std::string& portName = "A" );
			
			static TypeCode Type()  { return kDeviceFileType; }  // FIXME:  Need a new type
			
			TypeCode ActualType() const  { return Type(); }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
	};
	
#endif
	
	
	boost::shared_ptr< IOHandle > OpenSerialDevice()
	{
	#if TARGET_API_MAC_CARBON
		
		N::ThrowOSStatus( fnfErr );
		
		return boost::shared_ptr< IOHandle >();
		
	#else
		
		static boost::weak_ptr< IOHandle > gSerialDevice;
		
		if ( gSerialDevice.expired() )
		{
			boost::shared_ptr< IOHandle > result( new SerialDeviceHandle() );
			
			gSerialDevice = result;
			
			return result;
		}
		
		return boost::shared_ptr< IOHandle >( gSerialDevice );
		
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
		bool readable = N::SerGetBuf( itsInputRefNum ) > 0;
		
		unsigned readability = readable ? kPollRead : 0;
		
		return readability | kPollWrite | kPollExcept;
	}
	
	int SerialDeviceHandle::SysRead( char* data, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		std::size_t bytesAvailable = N::SerGetBuf( itsInputRefNum );
		
		// Assume blocking I/O for now
		do
		{
			Yield();
			bytesAvailable = N::SerGetBuf( itsInputRefNum );
		}
		while ( bytesAvailable == 0 );
		
		byteCount = std::min( byteCount, bytesAvailable );
		
		if ( byteCount == 0 )
		{
			throw io::no_input_pending();
		}
		
		return N::Read( itsInputRefNum, data, byteCount );
	}
	
	int SerialDeviceHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return N::Write( itsOutputRefNum, data, byteCount );
	}
	
#endif
	
}

