/*	====================
 *	sys_mac_vol_parms.cc
 *	====================
 */

#include "Genie/FS/sys_mac_vol_parms.hh"

// Iota
#include "iota/decimal.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/stringify.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		return N::FSVolumeRefNum( -std::atoi( parent->Name().c_str() ) );
	}
	
	static N::FSVolumeRefNum GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef()->ParentRef() );
	}
	
	
	struct GetVolumeParmsAttrib
	{
		static const bool alwaysStringified = false;
		
		typedef UInt32 Result;
		
		typedef stringify_32_bit_hex stringify;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			return parmsInfo.vMAttrib;
		}
	};
	
	struct GetVolumeParmsHandle
	{
		static const bool alwaysStringified = false;
		
		typedef ::Handle Result;
		
		typedef stringify_pointer stringify;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMLocalHand == 0 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMLocalHand;
		}
	};
	
	struct GetVolumeParmsServer
	{
		static const bool alwaysStringified = false;
		
		typedef UInt32 Result;
		
		typedef stringify_32_bit_hex stringify;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMServerAdr == 0 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMServerAdr;
		}
	};
	
	struct GetVolumeParmsGrade
	{
		static const bool alwaysStringified = false;
		
		typedef SInt32 Result;
		
		typedef stringify_int stringify;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 2  ||  parmsInfo.vMVolumeGrade == 0 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMVolumeGrade;
		}
	};
	
	struct GetVolumeParmsPrivID
	{
		static const bool alwaysStringified = false;
		
		typedef SInt16 Result;
		
		typedef stringify_short stringify;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 2 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMForeignPrivID;
		}
	};
	
	struct GetVolumeParmsExtended
	{
		static const bool alwaysStringified = false;
		
		typedef UInt32 Result;
		
		typedef stringify_32_bit_hex stringify;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 3 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMExtendedAttributes;
		}
	};
	
	struct GetVolumeParmsDeviceID
	{
		static const bool alwaysStringified = true;
		
		typedef const char* Result;
		
		typedef stringify_string stringify;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 4  ||  parmsInfo.vMDeviceID == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			return (const char*) parmsInfo.vMDeviceID;
		}
	};
	
	static void GetVolParmsInfo( GetVolParmsInfoBuffer&  parmsInfo,
	                             const FSTree*           that )
	{
		const N::FSVolumeRefNum vRefNum = GetKey( that );
		
		memset( &parmsInfo, '\0', sizeof parmsInfo );
		
		HParamBlockRec pb = { 0 };
		
		HIOParam& io = pb.ioParam;
		
		io.ioVRefNum  = vRefNum;
		io.ioBuffer   = (char *) &parmsInfo;
		io.ioReqCount = sizeof parmsInfo;
		
		N::ThrowOSStatus( ::PBHGetVolParmsSync( &pb ) );
	}
	
	static inline std::string stringify( const char* string )
	{
		return string;
	}
	
	static inline std::string stringify( int i )
	{
		return iota::inscribe_decimal( i );
	}
	
	static std::string stringify( ::Handle );
	
	template < class Accessor >
	struct sys_mac_vol_N_Parms_Property
	{
		typedef N::FSVolumeRefNum Key;
		
		static std::string Read( const FSTree* that, bool binary )
		{
			GetVolParmsInfoBuffer parmsInfo;
			
			GetVolParmsInfo( parmsInfo, that );
			
			const typename Accessor::Result data = Accessor::Get( parmsInfo );
			
			std::string result = Accessor::stringify::apply( data, binary );
			
			return result;
		}
	};
	
	
	template < class Accessor >
	static FSTreePtr Parms_Property_Factory( const FSTreePtr&    parent,
	                                         const std::string&  name )
	{
		typedef sys_mac_vol_N_Parms_Property< Accessor > Property;
		
		const bool fixed = !Accessor::alwaysStringified;
		
		const size_t size = fixed ? sizeof (typename Accessor::Result) : 0;
		
		return New_FSTree_Property( parent,
		                            name,
		                            size,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_vol_N_parms_Mappings[] =
	{
		{ "attrib",   &Parms_Property_Factory< GetVolumeParmsAttrib   >       },
		{ "handle",   &Parms_Property_Factory< GetVolumeParmsHandle   >, true },
		{ "server",   &Parms_Property_Factory< GetVolumeParmsServer   >, true },
		
		{ "grade",    &Parms_Property_Factory< GetVolumeParmsGrade    >, true },
		{ "priv",     &Parms_Property_Factory< GetVolumeParmsPrivID   >, true },
		
		{ "extended", &Parms_Property_Factory< GetVolumeParmsExtended >, true },
		
		{ "device",   &Parms_Property_Factory< GetVolumeParmsDeviceID >, true },
		
		{ NULL, NULL }
		
	};
	
}

