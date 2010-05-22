/*	=============
 *	PPCToolbox.hh
 *	=============
 */

#ifndef CLASSICTOOLBOX_PPCTOOLBOX_HH
#define CLASSICTOOLBOX_PPCTOOLBOX_HH

// Mac OS
#ifndef __PPCTOOLBOX__
#include <PPCToolbox.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/errors_registered.hh"
#include "nucleus/make.hh"

// Nitrogen
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( PPCToolbox );
	
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	using ::PPCXTIAddress;
	using ::PPCAddrRec;
	using ::LocationNameRec;
	using ::PPCPortRec;
	using ::PortInfoRec;
	using ::IPCListPortsPBRec;
	
	enum PPCServiceType
	{
		ppcServiceRealTime = ::ppcServiceRealTime,
		
		kPPCServiceType_Max = nucleus::enumeration_traits< UInt8 >::max
	};
	
	enum PPCLocationKind
	{
		ppcNoLocation      = ::ppcNoLocation,
		ppcNBPLocation     = ::ppcNBPLocation,
		ppcNBPTypeLocation = ::ppcNBPTypeLocation,
		ppcXTIAddrLocation = ::ppcXTIAddrLocation,
		
		kPPCLocationKind_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	enum PPCPortKinds
	{
		ppcByCreatorAndType = ::ppcByCreatorAndType,
		ppcByString         = ::ppcByString,
		
		kPPCPortKinds_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	enum PPCSessionOrigin
	{
		ppcLocalOrigin  = ::ppcLocalOrigin,
		ppcRemoteOrigin = ::ppcRemoteOrigin,
		
		kPPCSessionOrigin_Max = nucleus::enumeration_traits< UInt8 >::max
	};
	
	enum PPCPortRefNum
	{
		kPPCPortRefNum_Max = nucleus::enumeration_traits< short >::max
	};
	
	enum PPCSessRefNum
	{
		kPPCSessRefNum_Max = nucleus::enumeration_traits< long >::max
	};
	
	static const std::size_t kMaxPPCXTIAddress = ::kMaxPPCXTIAddress;
	
	enum PPCXTIAddressType
	{
		kINETAddrType = ::kINETAddrType,
		kDNSAddrType  = ::kDNSAddrType,
		
		kPPCXTIAddressType_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	#pragma mark -
	#pragma mark ¥ Typed constants ¥
	
}

namespace nucleus
{
	
	#pragma mark -
	#pragma mark ¥ Specializations ¥
	
	template <>  struct maker< PPCXTIAddress >
	{
		PPCXTIAddress operator()( const char* address_data, std::size_t address_size ) const;
		
		template < class String >
		PPCXTIAddress operator()( const String& address ) const
		{
			const char*        address_data = iota::get_string_data( address );
			const std::size_t  address_size = iota::get_string_size( address );
			
			return operator()( address_data, address_size );
		}
	};
	
	template <>  struct maker< PPCAddrRec >
	{
		PPCAddrRec operator()( const PPCXTIAddress& xtiAddr ) const;
	};
	
	template <>  struct maker< LocationNameRec >
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
	
	template <>  struct maker< PPCPortRec >
	{
		PPCPortRec operator()( ConstStr32Param  name,
		                       ConstStr32Param  portTypeStr = "\p" "=" ) const;
	};
	
	template <>  struct maker< IPCListPortsPBRec >
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
			return make< IPCListPortsPBRec >( name, static_cast< const LocationNameRec* >( NULL ), portInfo );
		}
	};
	
}

namespace Nitrogen
{
	
	void IPCListPortsSync( IPCListPortsPBRec& pb );
	
	PortInfoRec IPCListPortsSync( const PPCPortRec& name, const LocationNameRec& location );
	PortInfoRec IPCListPortsSync( const PPCPortRec& name );
	
}

#endif
