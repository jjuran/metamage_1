/*	==========
 *	Devices.hh
 *	==========
 */

#ifndef CLASSICTOOLBOX_DEVICES_HH
#define CLASSICTOOLBOX_DEVICES_HH

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_DEVICES_TYPES_DRIVERREFNUM_HH
#include "Mac/Devices/Types/DriverRefNum.hh"
#endif


namespace Nitrogen
{
	
	enum ControlStatusCode
	{
		kControlStatusCode_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	typedef ControlStatusCode CSCode;
	
}

namespace nucleus
{
	
#if CALL_NOT_IN_CARBON
	
	template <> struct disposer< Mac::DriverRefNum >
	{
		typedef Mac::DriverRefNum  argument_type;
		typedef void               result_type;
		
		void operator()( Mac::DriverRefNum driverRefNum ) const
		{
			NUCLEUS_REQUIRE_ERRORS( DeviceManager );
			
			(void) ::CloseDriver( driverRefNum );
		}
	};
	
#endif
	
}

namespace Nitrogen
{
	
	template < ControlStatusCode csCode >  struct Control_Default;
	
	struct Control_NullParamDefaults
	{
		static inline void* DefaultParam()  { return NULL; }
	};
	
	// 1158
	nucleus::owned< Mac::DriverRefNum > MacOpenDriver( ConstStr255Param name );
	
	// 1175
	void MacCloseDriver( nucleus::owned< Mac::DriverRefNum > driverRefNum );
	
	// 1190
	void Control( Mac::DriverRefNum  driverRefNum,
	              ControlStatusCode  csCode,
	              const void*        csParamPtr );
	
	template < ControlStatusCode csCode >
	inline void Control( Mac::DriverRefNum                                   driverRefNum,
	                     typename Control_Default< csCode >::parameter_type  csParam )
	{
		Control( driverRefNum, ControlStatusCode( csCode ), &csParam );
	}
	
	template < ControlStatusCode csCode >
	inline void Control( Mac::DriverRefNum driverRefNum )
	{
		Control( driverRefNum,
		         ControlStatusCode( csCode ),
		         Control_Default< csCode >::DefaultParam() );
	}
	
	// 1205
	void Status( Mac::DriverRefNum  driverRefNum,
	             ControlStatusCode  csCode,
	             void*              csParamPtr );
	
	// 1220
	void KillIO( Mac::DriverRefNum driverRefNum );
	
	void PBControlSync( ParamBlockRec& paramBlock );
	
	int Read ( Mac::DriverRefNum driverRefNum,       char* data, std::size_t byteCount );
	int Write( Mac::DriverRefNum driverRefNum, const char* data, std::size_t byteCount );
	
}

#endif
