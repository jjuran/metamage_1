/*	========
 *	Serial.h
 *	========
 */

#ifndef CLASSICTOOLBOX_SERIAL_H
#define CLASSICTOOLBOX_SERIAL_H

// Universal Interfaces
#ifndef __MACH__
#ifndef __SERIAL__
#include <Serial.h>
#endif
#endif

// Nucleus
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_DEVICES_H
#include "ClassicToolbox/Devices.h"
#endif


namespace Nitrogen
{
	
	class SerialDriverErrorsRegistrationDependency
	{
		public:
			SerialDriverErrorsRegistrationDependency();
	};
	
#if CALL_NOT_IN_CARBON
	
	enum SerConfig
	{
		baud150   = ::baud150,
		baud300   = ::baud300,
		baud600   = ::baud600,
		baud1200  = ::baud1200,
		baud2400  = ::baud2400,
		baud3600  = ::baud3600,
		baud4800  = ::baud4800,
		baud7200  = ::baud7200,
		baud9600  = ::baud9600,
		baud14400 = ::baud14400,
		baud19200 = ::baud19200,
		baud28800 = ::baud28800,
		baud38400 = ::baud38400,
		baud57600 = ::baud57600,
		
		stop10 = ::stop10,
		stop15 = ::stop15,
		stop20 = ::stop20,
		
		noParity   = ::noParity,
		oddParity  = ::oddParity,
		evenParity = ::evenParity,
		
		data5 = ::data5,
		data6 = ::data6,
		data7 = ::data7,
		data8 = ::data8,
		
		kSerConfig_Max = Nucleus::Enumeration_Traits< short >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( SerConfig )
	
	static const CSCode kSERDHandshake     = CSCode( ::kSERDHandshake     );
	static const CSCode kSERDAssertDTR     = CSCode( ::kSERDAssertDTR     );
	static const CSCode kSERDNegateDTR     = CSCode( ::kSERDNegateDTR     );
	static const CSCode kSERDClearXOffFlag = CSCode( ::kSERDClearXOffFlag );
	
	using ::SerStaRec;
	using ::SerShk;
	
}

namespace Nucleus
{
	
	template <>
	struct Maker< Nitrogen::SerShk >
	{
		Nitrogen::SerShk operator()( Byte           fXOn = 0,
		                             Byte           fCTS = 0,
		                             unsigned char  xOn  = 0,
		                             unsigned char  xOff = 0,
		                             Byte           errs = 0,
		                             Byte           evts = 0,
		                             Byte           fInX = 0,
		                             Byte           fDTR = 0 ) const
		{
			Nitrogen::SerShk result;
			
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
	
}

namespace Nitrogen
{
	
	struct SerialDevice_Record;
	
	typedef SerialDevice_Record* SerialDeviceRef;
	
	struct SerialDevice_Record
	{
		Nucleus::Owned< DriverRefNum > output;
		Nucleus::Owned< DriverRefNum > input;
		
		SerialDevice_Record()  {}
		
		SerialDevice_Record( Nucleus::Owned< DriverRefNum > output,
		                     Nucleus::Owned< DriverRefNum > input )
		:
			output( output ),
			input ( input  )
		{}
		
	};
	
}

namespace Nucleus
{
	
	template <> struct Disposer< Nitrogen::SerialDeviceRef > : public DisposeWithDelete  {};
	
}

namespace Nitrogen
{
	
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
	void SerHShake( SerialDeviceRef serialDevice, const SerShk& serShk = Nucleus::Make< SerShk >() );
	
	// 328
	std::size_t SerGetBuf( SerialDeviceRef serialDevice );
	
	// 342
	SerStaRec SerStatus( SerialDeviceRef serialDevice );
	
	void AssertDTR( SerialDeviceRef serialDevice );
	void NegateDTR( SerialDeviceRef serialDevice );
	void ClearXOFF( SerialDeviceRef serialDevice );
	
	int Read ( SerialDeviceRef serialDevice,       char* data, std::size_t byteCount );
	int Write( SerialDeviceRef serialDevice, const char* data, std::size_t byteCount );
	
	Nucleus::Owned< SerialDeviceRef > Open_SerialDevice( const std::string& portName );
	
	void Close_SerialDevice( Nucleus::Owned< SerialDeviceRef > serialDevice );
	
#endif
	
}

namespace Io
{
	
#if CALL_NOT_IN_CARBON
	
	using Nitrogen::Read;
	using Nitrogen::Write;
	
#endif
	
}

#endif

