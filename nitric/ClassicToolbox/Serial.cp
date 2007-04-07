/*	=========
 *	Serial.cc
 *	=========
 */

#ifndef CLASSICTOOLBOX_SERIAL_H
#include "ClassicToolbox/Serial.h"
#endif


namespace Nitrogen
{
	
	SerialDriverErrorsRegistrationDependency::SerialDriverErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterSerialDriverErrors();
	
	
	class SerialDriverErrorsRegistration
	{
		public:
			SerialDriverErrorsRegistration()  { RegisterSerialDriverErrors(); }
	};
	
	static SerialDriverErrorsRegistration theRegistration;
	
	
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
	
	std::size_t SerGetBuf( DriverRefNum inputDriverRefNum )
	{
		long count;
		ThrowOSStatus( ::SerGetBuf( inputDriverRefNum, &count ) );
		return count;
	}
	
	void SerSetBuf( DriverRefNum inputDriverRefNum, void* buf, std::size_t bufSize )
	{
		const std::size_t maxSize = 32767;
		
		ThrowOSStatus( ::SerSetBuf( inputDriverRefNum,
		                            reinterpret_cast< ::Ptr >( buf ),
		                            std::min( bufSize, maxSize ) ) );
	}
	
	SerStaRec SerStatus( DriverRefNum outputDriverRefNum )
	{
		SerStaRec serStatus;
		ThrowOSStatus( ::SerStatus( outputDriverRefNum, &serStatus ) );
		return serStatus;
	}
	
	void SerReset( DriverRefNum outputDriverRefNum, SerConfig serConfig )
	{
		ThrowOSStatus( ::SerReset( outputDriverRefNum, serConfig ) );
	}
	
	void SerHShake( DriverRefNum outputDriverRefNum, const SerShk& serShk )
	{
		Control< kSERDHandshake >( outputDriverRefNum, serShk );
	}
	
	void AssertDTR( DriverRefNum outputDriverRefNum )
	{
		Control< kSERDAssertDTR >( outputDriverRefNum );
	}
	
	void NegateDTR( DriverRefNum outputDriverRefNum )
	{
		Control< kSERDNegateDTR >( outputDriverRefNum );
	}
	
	void ClearXOFF( DriverRefNum outputDriverRefNum )
	{
		Control< kSERDClearXOffFlag >( outputDriverRefNum );
	}
	
	
	/*
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
	
	int Read( DriverRefNum inputDriverRefNum, char* data, std::size_t byteCount )
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
	*/
	
#endif  // CALL_NOT_IN_CARBON
	
	void RegisterSerialDriverErrors()
	{
		RegisterOSStatus< openErr   >();
		RegisterOSStatus< portInUse >();
		RegisterOSStatus< portNotCf >();
	}
	
}

