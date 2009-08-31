/*	=========
 *	Devices.h
 *	=========
 */

#ifndef CLASSICTOOLBOX_DEVICES_H
#define CLASSICTOOLBOX_DEVICES_H

// Universal Interfaces
#ifndef __DEVICES__
#include <Devices.h>
#endif

// Nucleus
#include "Nucleus/Enumeration.h"

// Nitrogen
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif


namespace Nitrogen
{
	
	enum ControlStatusCode
	{
		kControlStatusCode_Max = Nucleus::Enumeration_Traits< SInt16 >::max
	};
	
	typedef ControlStatusCode CSCode;
	
}

namespace Nucleus
{
	
#if CALL_NOT_IN_CARBON
	
	template <> struct Disposer< Nitrogen::DriverRefNum > : public std::unary_function< Nitrogen::DriverRefNum, void >,
	                                                        private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::DriverRefNum driverRefNum ) const
		{
			NUCLEUS_REQUIRE_ERRORS( DeviceManager );
			
			HandleDestructionOSStatus( ::CloseDriver( driverRefNum ) );
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
	Nucleus::Owned< DriverRefNum > MacOpenDriver( ConstStr255Param name );
	Nucleus::Owned< DriverRefNum > MacOpenDriver( const std::string& name );
	
	// 1175
	void MacCloseDriver( Nucleus::Owned< DriverRefNum > driverRefNum );
	
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

