/*	============
 *	PPCToolbox.h
 *	============
 */

#ifndef CLASSICTOOLBOX_PPCTOOLBOX_H
#define CLASSICTOOLBOX_PPCTOOLBOX_H

// Universal Interfaces
#ifndef __PPCTOOLBOX__
#include <PPCToolbox.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif

// Standard C++
#include <algorithm>
#include <string>

// Nitrogen
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_APPLETALK_H
#include "ClassicToolbox/AppleTalk.h"
#endif


namespace Nitrogen
{
	
	void RegisterPPCToolboxErrors();
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	using ::PPCXTIAddress;
	using ::PPCAddrRec;
	using ::LocationNameRec;
	using ::PPCPortRec;
	using ::PortInfoRec;
	using ::IPCListPortsPBRec;
	
	struct PPCServiceType_Tag {};
	typedef SelectorType< PPCServiceType_Tag, UInt8, 0 > PPCServiceType;
	
	struct PPCLocationKind_Tag {};
	typedef SelectorType< PPCLocationKind_Tag, SInt16, ppcNoLocation > PPCLocationKind;
	
	struct PPCPortKinds_Tag {};
	typedef SelectorType< PPCPortKinds_Tag, SInt16, 0 > PPCPortKinds;
	
	struct PPCSessionOrigin_Tag {};
	typedef SelectorType< PPCSessionOrigin_Tag, UInt8, 0 > PPCSessionOrigin;
	
	struct PPCPortRefNum_Tag {};
	typedef IDType< PPCPortRefNum_Tag, short, 0 > PPCPortRefNum;
	
	struct PPCSessRefNum_Tag {};
	typedef IDType< PPCSessRefNum_Tag, long, 0 > PPCSessRefNum;
	
	struct PPCXTIAddressType_Tag {};
	typedef SelectorType< PPCXTIAddressType_Tag, SInt16, 0 > PPCXTIAddressType;
	
	#pragma mark -
	#pragma mark ¥ Typed constants ¥
	
	static PPCServiceType sPPCServiceRealTime = ppcServiceRealTime;
	
	static PPCLocationKind sPPCNoLocation      = ppcNoLocation;
	static PPCLocationKind sPPCNBPLocation     = ppcNBPLocation;
	static PPCLocationKind sPPCNBPTypeLocation = ppcNBPTypeLocation;
	static PPCLocationKind sPPCXTIAddrLocation = ppcXTIAddrLocation;
	
	static PPCPortKinds sPPCByCreatorAndType = ppcByCreatorAndType;
	static PPCPortKinds sPPCByString         = ppcByString;
	
	static PPCSessionOrigin sPPCLocalOrigin  = ppcLocalOrigin;
	static PPCSessionOrigin sPPCRemoteOrigin = ppcRemoteOrigin;
	
	static std::size_t sMaxPPCXTIAddress = kMaxPPCXTIAddress;
	
	static PPCXTIAddressType sINETAddrType = kINETAddrType;
	static PPCXTIAddressType sDNSAddrType  = kDNSAddrType;
	
	#pragma mark -
	#pragma mark ¥ Specializations ¥
	
	template <>  struct Maker< PPCXTIAddress >
	{
		PPCXTIAddress operator()( const std::string& address ) const
		{
			if ( address.size() > kMaxPPCXTIAddress )
			{
				// FIXME:  Should throw or something
			}
			
			PPCXTIAddress xtiAddr;
			
			xtiAddr.fAddressType = kDNSAddrType;
			std::fill( xtiAddr.fAddress,
			           xtiAddr.fAddress + kMaxPPCXTIAddress + 1,
			           '\0' );
			std::copy( address.begin(), 
			           address.begin() + std::min< std::size_t >( address.size(),
			                                                      kMaxPPCXTIAddress ), 
			           xtiAddr.fAddress );
			
			return xtiAddr;
		}
	};
	
	template <>  struct Maker< PPCAddrRec >
	{
		PPCAddrRec operator()( const PPCXTIAddress& xtiAddr ) const
		{
			const UInt8* terminator = std::find( xtiAddr.fAddress,
			                                     xtiAddr.fAddress + kMaxPPCXTIAddress,
			                                     '\0' );
			
			PPCAddrRec xti;
			
			xti.Reserved[0] = 0;
			xti.Reserved[1] = 0;
			xti.Reserved[2] = 0;
			xti.xtiAddrLen = sizeof (::PPCXTIAddressType) + (terminator + 1 - xtiAddr.fAddress);
			xti.xtiAddr = xtiAddr;
			
			return xti;
		}
	};
	
	template <>  struct Maker< LocationNameRec >
	{
		LocationNameRec operator()() const
		{
			LocationNameRec location;
			
			location.locationKindSelector = ppcNoLocation;
			
			return location;
		}
		
		LocationNameRec operator()( const EntityName& entityName ) const
		{
			LocationNameRec location;
			
			location.locationKindSelector = ppcNBPLocation;
			location.u.nbpEntity = entityName;
			
			return location;
		}
		
		LocationNameRec operator()( ConstStr32Param nbpType ) const
		{
			LocationNameRec location;
			
			location.locationKindSelector = ppcNBPTypeLocation;
			CopyToPascalString( nbpType, location.u.nbpType, 32 );
			
			return location;
		}
		
		LocationNameRec operator()( const PPCAddrRec& xtiType ) const
		{
			LocationNameRec location;
			
			location.locationKindSelector = ppcXTIAddrLocation;
			location.u.xtiType = xtiType;
			
			return location;
		}
	};
	
	template <>  struct Maker< PPCPortRec >
	{
		PPCPortRec operator()( ConstStr32Param name, ConstStr32Param portTypeStr = "\p=" ) const
		{
			PPCPortRec port;
			
			port.nameScript = smSystemScript;  // FIXME
			
			CopyToPascalString( name,        port.name,          32 );
			CopyToPascalString( portTypeStr, port.u.portTypeStr, 32 );
			
			port.portKindSelector = ppcByString;
			
			return port;
		}
	};
	
	template <>  struct Maker< IPCListPortsPBRec >
	{
		IPCListPortsPBRec operator()( const PPCPortRec* name, const LocationNameRec* location, PortInfoRec* portInfo ) const
		{
			IPCListPortsPBRec pb;
			
			pb.startIndex = 0;
			pb.requestCount = 1;
			pb.portName = const_cast< PPCPortRec* >( name );
			pb.locationName = const_cast< LocationNameRec* >( location );
			pb.bufferPtr = portInfo;
			
			return pb;
		}
		
		IPCListPortsPBRec operator()( const PPCPortRec* name, PortInfoRec* portInfo ) const
		{
			return Make< IPCListPortsPBRec >( name, static_cast< const LocationNameRec* >( NULL ), portInfo );
		}
	};
	
#if CALL_NOT_IN_CARBON
	
	void IPCListPortsSync( IPCListPortsPBRec& pb );
	
	PortInfoRec IPCListPortsSync( const PPCPortRec& name, const LocationNameRec& location );
	PortInfoRec IPCListPortsSync( const PPCPortRec& name );
	
#endif  // #if CALL_NOT_IN_CARBON
	
}

#endif
