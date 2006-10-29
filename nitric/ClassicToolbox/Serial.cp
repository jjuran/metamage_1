/*	=========
 *	Serial.cc
 *	=========
 */

#ifndef CLASSICTOOLBOX_SERIAL_H
#include "ClassicToolbox/Serial.h"
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	template < class Type >
	Type* CheckNULL( Type* ptr )
	{
		if ( static_cast< const void* >( ptr ) == NULL )
		{
			throw ParamErr();  // FIXME:  Should assert
		}
		
		return ptr;
	}
	
	std::size_t SerGetBuf( SerialDeviceRef serialDevice )
	{
		Nucleus::OnlyOnce< RegisterSerialDriverErrors >();
		
		CheckNULL( serialDevice );
		long count;
		ThrowOSStatus( ::SerGetBuf( serialDevice->input.Get(), &count ) );
		return count;
	}
	
	void SerSetBuf( SerialDeviceRef serialDevice, void* buf, std::size_t bufSize )
	{
		Nucleus::OnlyOnce< RegisterSerialDriverErrors >();
		
		const std::size_t maxSize = 32767;
		
		ThrowOSStatus( ::SerSetBuf( serialDevice->input.Get(),
		                            reinterpret_cast< ::Ptr >( buf ),
		                            std::min( bufSize, maxSize ) ) );
	}
	
	SerStaRec SerStatus( SerialDeviceRef serialDevice )
	{
		Nucleus::OnlyOnce< RegisterSerialDriverErrors >();
		
		CheckNULL( serialDevice );
		SerStaRec serStatus;
		ThrowOSStatus( ::SerStatus( serialDevice->output.Get(), &serStatus ) );
		return serStatus;
	}
	
	void SerReset( SerialDeviceRef serialDevice, SerConfig serConfig )
	{
		Nucleus::OnlyOnce< RegisterSerialDriverErrors >();
		
		CheckNULL( serialDevice );
		ThrowOSStatus( ::SerReset( serialDevice->output.Get(), serConfig ) );
	}
	
	void SerHShake( SerialDeviceRef serialDevice, const SerShk& serShk )
	{
		Control< kSERDHandshake >( CheckNULL( serialDevice )->output.Get(), serShk );
	}
	
	void AssertDTR( SerialDeviceRef serialDevice )
	{
		Control< kSERDAssertDTR >( CheckNULL( serialDevice )->output.Get() );
	}
	
	void NegateDTR( SerialDeviceRef serialDevice )
	{
		Control< kSERDNegateDTR >( CheckNULL( serialDevice )->output.Get() );
	}
	
	void ClearXOFF( SerialDeviceRef serialDevice )
	{
		Control< kSERDClearXOffFlag >( CheckNULL( serialDevice )->output.Get() );
	}
	
	
	static std::string MakeDriverName( const std::string&  portName,
	                                   const std::string&  directionName )
	{
		return std::string( "." ) + portName + directionName;
	}
	
	Nucleus::Owned< SerialDeviceRef > Open_SerialDevice( const std::string& portName )
	{
		// Open the output driver FIRST, then the input driver.
		// Open the output driver even if you aren't going to use it.
		Nucleus::Owned< DriverRefNum > output = OpenDriver( MakeDriverName( portName, "Out" ) );
		Nucleus::Owned< DriverRefNum > input  = OpenDriver( MakeDriverName( portName, "In"  ) );
		
		return Nucleus::Owned< SerialDeviceRef >::Seize( new SerialDevice_Record( output, input ) );
	}
	
	void Close_SerialDevice( Nucleus::Owned< SerialDeviceRef > serialDevice )
	{
		// Serial drivers will be closed (and the object deleted) when serialDevice goes out of scope.
	}
	
	
	int Read( SerialDeviceRef serialDevice, char* data, std::size_t byteCount )
	{
		CheckNULL( serialDevice );
		
		if ( byteCount == 0 )
		{
			// Asked to read 0 bytes.  Do nothing and indicate success.
			return 0;
		}
		
		// Don't try to read more data then is buffered, or we'll hang.
		byteCount = std::min( byteCount, SerGetBuf( serialDevice ) );
		
		if ( byteCount == 0 )
		{
			// No data available
			//throw Io::NoDataPending();  // Io is not available in Nitrogen
			throw OTNoDataErr();
		}
		
		return Read( serialDevice->input, data, byteCount );
	}
	
	int Write( SerialDeviceRef serialDevice, const char* data, std::size_t byteCount )
	{
		CheckNULL( serialDevice );
		return Write( serialDevice->output, data, byteCount );
	}
	
#endif  // CALL_NOT_IN_CARBON
	
	void RegisterSerialDriverErrors()
	{
		RegisterOSStatus< openErr   >();
		RegisterOSStatus< portInUse >();
		RegisterOSStatus< portNotCf >();
	}
	
}

