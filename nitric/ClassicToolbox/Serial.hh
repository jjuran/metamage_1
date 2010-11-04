/*	=========
 *	Serial.hh
 *	=========
 */

#ifndef CLASSICTOOLBOX_SERIAL_HH
#define CLASSICTOOLBOX_SERIAL_HH

// Mac OS
#ifndef __SERIAL__
#include <Serial.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_DEVICES_HH
#include "ClassicToolbox/Devices.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( SerialDriver );
	
	
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
		
		kSerConfig_Max = nucleus::enumeration_traits< short >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( SerConfig )
	
	static const CSCode kSERDHandshake     = CSCode( ::kSERDHandshake     );
	static const CSCode kSERDAssertDTR     = CSCode( ::kSERDAssertDTR     );
	static const CSCode kSERDNegateDTR     = CSCode( ::kSERDNegateDTR     );
	static const CSCode kSERDClearXOffFlag = CSCode( ::kSERDClearXOffFlag );
	
	using ::SerStaRec;
	using ::SerShk;
	
}

namespace nucleus
{
	
	template <>
	struct maker< SerShk >
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
	
	// 261
	void SerReset( Mac::DriverRefNum outputDriverRefNum, SerConfig serConfig );
	
	// 275
	void SerSetBuf( Mac::DriverRefNum inputDriverRefNum, void* buf, std::size_t bufSize );
	
	// 290
	void SerHShake( Mac::DriverRefNum outputDriverRefNum, const SerShk& serShk = nucleus::make< SerShk >() );
	
	// 328
	std::size_t SerGetBuf( Mac::DriverRefNum inputDriverRefNum );
	
	// 342
	SerStaRec SerStatus( Mac::DriverRefNum outputDriverRefNum );
	
	void AssertDTR( Mac::DriverRefNum outputDriverRefNum );
	void NegateDTR( Mac::DriverRefNum outputDriverRefNum );
	void ClearXOFF( Mac::DriverRefNum outputDriverRefNum );
	
	int Read ( Mac::DriverRefNum inputDriverRefNum,        char* data, std::size_t byteCount );
	int Write( Mac::DriverRefNum outputDriverRefNum, const char* data, std::size_t byteCount );
	
}

#endif

