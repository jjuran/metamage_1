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

// Nucleus
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif

// Nitrogen
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
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
	
	typedef Nucleus::Selector< struct PPCServiceType_Tag,    UInt8  >::Type PPCServiceType;
	typedef Nucleus::Selector< struct PPCLocationKind_Tag,   SInt16 >::Type PPCLocationKind;
	typedef Nucleus::Selector< struct PPCPortKinds_Tag,      SInt16 >::Type PPCPortKinds;
	typedef Nucleus::Selector< struct PPCSessionOrigin_Tag,  UInt8  >::Type PPCSessionOrigin;
	typedef Nucleus::ID      < struct PPCPortRefNum_Tag,     short  >::Type PPCPortRefNum;
	typedef Nucleus::ID      < struct PPCSessRefNum_Tag,     long   >::Type PPCSessRefNum;
	typedef Nucleus::Selector< struct PPCXTIAddressType_Tag, SInt16 >::Type PPCXTIAddressType;
	
	#pragma mark -
	#pragma mark ¥ Typed constants ¥
	
	static const PPCServiceType ppcServiceRealTime = PPCServiceType( ::ppcServiceRealTime );
	
	static const PPCLocationKind ppcNoLocation      = PPCLocationKind( ::ppcNoLocation      );
	static const PPCLocationKind ppcNBPLocation     = PPCLocationKind( ::ppcNBPLocation     );
	static const PPCLocationKind ppcNBPTypeLocation = PPCLocationKind( ::ppcNBPTypeLocation );
	static const PPCLocationKind ppcXTIAddrLocation = PPCLocationKind( ::ppcXTIAddrLocation );
	
	static const PPCPortKinds ppcByCreatorAndType = PPCPortKinds( ::ppcByCreatorAndType );
	static const PPCPortKinds ppcByString         = PPCPortKinds( ::ppcByString         );
	
	static const PPCSessionOrigin ppcLocalOrigin  = PPCSessionOrigin( ::ppcLocalOrigin  );
	static const PPCSessionOrigin ppcRemoteOrigin = PPCSessionOrigin( ::ppcRemoteOrigin );
	
	static const std::size_t kMaxPPCXTIAddress = ::kMaxPPCXTIAddress;
	
	static const PPCXTIAddressType kINETAddrType = PPCXTIAddressType( ::kINETAddrType );
	static const PPCXTIAddressType kDNSAddrType  = PPCXTIAddressType( ::kDNSAddrType  );
	
}

namespace Nucleus
{
	
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
			Nitrogen::CopyToPascalString( nbpType, location.u.nbpType, 32 );
			
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
			
			Nitrogen::CopyToPascalString( name,        port.name,          32 );
			Nitrogen::CopyToPascalString( portTypeStr, port.u.portTypeStr, 32 );
			
			port.portKindSelector = ppcByString;
			
			return port;
		}
	};
	
	template <>  struct Maker< IPCListPortsPBRec >
	{
		IPCListPortsPBRec operator()( const PPCPortRec*       name,
		                              const LocationNameRec*  location,
		                              PortInfoRec*            portInfo ) const
		{
			IPCListPortsPBRec pb;
			
			pb.startIndex   = 0;
			pb.requestCount = 1;
			pb.portName     = const_cast< PPCPortRec*      >( name     );
			pb.locationName = const_cast< LocationNameRec* >( location );
			pb.bufferPtr = portInfo;
			
			return pb;
		}
		
		IPCListPortsPBRec operator()( const PPCPortRec* name, PortInfoRec* portInfo ) const
		{
			return Make< IPCListPortsPBRec >( name, static_cast< const LocationNameRec* >( NULL ), portInfo );
		}
	};
	
}

namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	void IPCListPortsSync( IPCListPortsPBRec& pb );
	
	PortInfoRec IPCListPortsSync( const PPCPortRec& name, const LocationNameRec& location );
	PortInfoRec IPCListPortsSync( const PPCPortRec& name );
	
#endif  // #if CALL_NOT_IN_CARBON
	
}

#endif
