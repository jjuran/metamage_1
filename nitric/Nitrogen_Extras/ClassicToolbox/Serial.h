/*	========
 *	Serial.h
 *	========
 */

#ifndef CLASSICTOOLBOX_SERIAL_H
#define CLASSICTOOLBOX_SERIAL_H

// Universal Interfaces
#ifndef __SERIAL__
#include <Serial.h>
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_DEVICES_H
#include "ClassicToolbox/Devices.h"
#endif


namespace Nitrogen
{
	
	void RegisterSerialDriverErrors();
	
	struct SerConfig_Tag  {};
	typedef FlagType< SerConfig_Tag, short > SerConfig;
	
	using ::SerStaRec;
	using ::SerShk;
	
	template <>
	struct Maker< SerShk >
	{
		SerShk operator()( Byte           fXOn = 0,
		                   Byte           fCTS = 0,
		                   unsigned char  xOn  = 0,
		                   unsigned char  xOff = 0,
		                   Byte           errs = 0,
		                   Byte           evts = 0,
		                   Byte           fInX = 0,
		                   Byte           fDTR = 0 ) const
		{
			SerShk result;
			result.fXOn = fXOn;
			result.fCTS = fCTS;
			result.xOn  = xOn;
			result.xOff = xOff;
			result.errs = errs;
			result.evts = evts;
			result.fInX = fInX;
			result.fDTR = fDTR;
			return result;
		}
	};
	
	struct SerialDevice_Record;
	
	typedef SerialDevice_Record* SerialDeviceRef;
	
	struct SerialDevice_Record
	{
		Owned< DriverRefNum > output;
		Owned< DriverRefNum > input;
		
		SerialDevice_Record()  {}
		
		SerialDevice_Record( Owned< DriverRefNum > output,
		                     Owned< DriverRefNum > input )
		:
			output( output ),
			input ( input  )
		{}
		
	};
	
	template <> struct Disposer< SerialDeviceRef > : public DisposeWithDelete  {};
	
	template <>  struct Control_Default< kSERDHandshake >
	{
		typedef SerShk             value_type;
		typedef const value_type&  parameter_type;
	};
	
	template <>  struct Control_Default< kSERDAssertDTR     > : Control_NullParamDefaults  {};
	template <>  struct Control_Default< kSERDNegateDTR     > : Control_NullParamDefaults  {};
	template <>  struct Control_Default< kSERDClearXOffFlag > : Control_NullParamDefaults  {};
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	// 261
	void SerReset( SerialDeviceRef serialDevice, SerConfig serConfig );
	
	// 275
	void SerSetBuf( SerialDeviceRef serialDevice, void* buf, std::size_t bufSize );
	
	// 290
	void SerHShake( SerialDeviceRef serialDevice, const SerShk& serShk = Make< SerShk >() );
	
	// 328
	std::size_t SerGetBuf( SerialDeviceRef serialDevice );
	
	// 342
	SerStaRec SerStatus( SerialDeviceRef serialDevice );
	
	void AssertDTR( SerialDeviceRef serialDevice );
	void NegateDTR( SerialDeviceRef serialDevice );
	void ClearXOFF( SerialDeviceRef serialDevice );
	
	int Read ( SerialDeviceRef serialDevice,       char* data, std::size_t byteCount );
	int Write( SerialDeviceRef serialDevice, const char* data, std::size_t byteCount );
	
	Owned< SerialDeviceRef > Open_SerialDevice( const std::string& portName );
	
	void Close_SerialDevice( Owned< SerialDeviceRef > serialDevice );
	
}

namespace Io
{
	
	using Nitrogen::Read;
	using Nitrogen::Write;
	
}

#endif

