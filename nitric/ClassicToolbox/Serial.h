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
	
	void RegisterSerialDriverErrors();
	
#if CALL_NOT_IN_CARBON
	
	typedef Nucleus::Flag< struct SerConfig_Tag, short >::Type SerConfig;
	
	NUCLEUS_DEFINE_FLAG_OPS( SerConfig )
	
	static const SerConfig baud150   = SerConfig( ::baud150   );
	static const SerConfig baud300   = SerConfig( ::baud300   );
	static const SerConfig baud600   = SerConfig( ::baud600   );
	static const SerConfig baud1200  = SerConfig( ::baud1200  );
	static const SerConfig baud2400  = SerConfig( ::baud2400  );
	static const SerConfig baud3600  = SerConfig( ::baud3600  );
	static const SerConfig baud4800  = SerConfig( ::baud4800  );
	static const SerConfig baud7200  = SerConfig( ::baud7200  );
	static const SerConfig baud9600  = SerConfig( ::baud9600  );
	static const SerConfig baud14400 = SerConfig( ::baud14400 );
	static const SerConfig baud19200 = SerConfig( ::baud19200 );
	static const SerConfig baud28800 = SerConfig( ::baud28800 );
	static const SerConfig baud38400 = SerConfig( ::baud38400 );
	static const SerConfig baud57600 = SerConfig( ::baud57600 );
	
	static const SerConfig stop10 = SerConfig( ::stop10 );
	static const SerConfig stop15 = SerConfig( ::stop15 );
	static const SerConfig stop20 = SerConfig( ::stop20 );
	
	static const SerConfig noParity   = SerConfig( ::noParity   );
	static const SerConfig oddParity  = SerConfig( ::oddParity  );
	static const SerConfig evenParity = SerConfig( ::evenParity );
	
	static const SerConfig data5 = SerConfig( ::data5 );
	static const SerConfig data6 = SerConfig( ::data6 );
	static const SerConfig data7 = SerConfig( ::data7 );
	static const SerConfig data8 = SerConfig( ::data8 );
	
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

