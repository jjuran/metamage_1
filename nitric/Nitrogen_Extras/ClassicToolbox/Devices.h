/*	=========
 *	Devices.h
 *	=========
 */

#ifndef CLASSICTOOLBOX_DEVICES_H
#define CLASSICTOOLBOX_DEVICES_H

// Universal Interfaces
#ifndef __MACH__
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif

// Nitrogen
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif


namespace Nitrogen
{
	
	void RegisterDeviceManagerErrors();
	
#if CALL_NOT_IN_CARBON
	
	struct ControlStatusCode_Tag  {};
	typedef SelectorType< ControlStatusCode_Tag, short, 0 > ControlStatusCode;
	
	typedef ControlStatusCode CSCode;
	
	template <> struct Disposer< DriverRefNum > : public std::unary_function< DriverRefNum, void >,
	                                              private DefaultDestructionOSStatusPolicy
	{
		void operator()( DriverRefNum driverRefNum ) const
		{
			OnlyOnce< RegisterDeviceManagerErrors >();
			HandleDestructionOSStatus( ::CloseDriver( driverRefNum ) );
		}
	};
	
	template < SInt16 csCode >  struct Control_Default;
	
	struct Control_NullParamDefaults
	{
		static inline void* DefaultParam()  { return NULL; }
	};
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	// 1158
	Owned< DriverRefNum > MacOpenDriver( ConstStr255Param name );
	Owned< DriverRefNum > MacOpenDriver( const std::string& name );
	
	// 1175
	void MacCloseDriver( Owned< DriverRefNum > driverRefNum );
	
	// 1190
	void Control( DriverRefNum       driverRefNum,
	              ControlStatusCode  csCode,
	              const void*        csParamPtr );
	
	template < SInt16 csCode >
	void Control( DriverRefNum                                        driverRefNum,
	              typename Control_Default< csCode >::parameter_type  csParam )
	{
		Control( driverRefNum, ControlStatusCode( csCode ), &csParam );
	}
	
	template < SInt16 csCode >
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
	
	int Read ( DriverRefNum driverRefNum,       char* data, std::size_t byteCount );
	int Write( DriverRefNum driverRefNum, const char* data, std::size_t byteCount );
	
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

