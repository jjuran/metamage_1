/*	===============
 *	SerialDevice.cc
 *	===============
 */

#include "Genie/IO/SerialDevice.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/StatFile.hh"


namespace Genie
{
	
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
		
		byteCount = std::min( byteCount, N::SerGetBuf( itsInputRefNum ) );
		
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
	
}

