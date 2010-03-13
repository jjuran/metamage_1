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
#include "nucleus/enumeration_traits.hh"

// Nitrogen
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
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
	
	template <> struct disposer< Nitrogen::DriverRefNum > : public std::unary_function< Nitrogen::DriverRefNum, void >
	{
		void operator()( Nitrogen::DriverRefNum driverRefNum ) const
		{
			NUCLEUS_REQUIRE_ERRORS( DeviceManager );
			
			::Nitrogen::HandleDestructionOSStatus( ::CloseDriver( driverRefNum ) );
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
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	// 1158
	nucleus::owned< DriverRefNum > MacOpenDriver( ConstStr255Param name );
	nucleus::owned< DriverRefNum > MacOpenDriver( const std::string& name );
	
	// 1175
	void MacCloseDriver( nucleus::owned< DriverRefNum > driverRefNum );
	
	// 1190
	void Control( DriverRefNum       driverRefNum,
	              ControlStatusCode  csCode,
	              const void*        csParamPtr );
	
	template < ControlStatusCode csCode >
	void Control( DriverRefNum                                        driverRefNum,
	              typename Control_Default< csCode >::parameter_type  csParam )
	{
		Control( driverRefNum, ControlStatusCode( csCode ), &csParam );
	}
	
	template < ControlStatusCode csCode >
	void Control( DriverRefNum driverRefNum )
	{
		Control( driverRefNum,
		         ControlStatusCode( csCode ),
		         Control_Default< csCode >::DefaultParam() );
	}
	
	// 1205
	void Status( DriverRefNum       driverRefNum,
	             ControlStatusCode  csCode,
	             void*              csParamPtr );
	
	// 1220
	void KillIO( DriverRefNum driverRefNum );
	
	void PBControlSync( ParamBlockRec& paramBlock );
	
	void OpenDeskAcc( ConstStr255Param deskAccName );
	
	int Read ( DriverRefNum driverRefNum,       char* data, std::size_t byteCount );
	int Write( DriverRefNum driverRefNum, const char* data, std::size_t byteCount );
	
}

namespace Io
{
	
	using Nitrogen::Read;
	using Nitrogen::Write;
	
}

#endif

