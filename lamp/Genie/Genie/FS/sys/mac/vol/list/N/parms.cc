/*
	Genie/FS/sys/mac/vol/list/N/parms.cc
	------------------------------------
*/

#include "Genie/FS/sys/mac/vol/list/N/parms.hh"

// Iota
#include "iota/decimal.hh"

// plus
#include "plus/deconstruct.hh"
#include "plus/freeze.hh"
#include "plus/serialize.hh"
#include "plus/stringify.hh"
#include "plus/var_string.hh"

// Nitrogen
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#include "Nitrogen/OSStatus.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static Mac::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		return Mac::FSVolumeRefNum( -iota::parse_unsigned_decimal( parent->Name().c_str() ) );
	}
	
	static Mac::FSVolumeRefNum GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef()->ParentRef() );
	}
	
	
	struct GetVolumeParmsAttrib : plus::serialize_hex< UInt32 >
	{
		static UInt32 Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			return parmsInfo.vMAttrib;
		}
	};
	
	struct GetVolumeParmsHandle : plus::serialize_pointer
	{
		static ::Handle Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMLocalHand == 0 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMLocalHand;
		}
	};
	
	struct GetVolumeParmsServer : plus::serialize_hex< UInt32 >
	{
		static UInt32 Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMServerAdr == 0 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMServerAdr;
		}
	};
	
	struct GetVolumeParmsGrade : plus::serialize_int< SInt32 >
	{
		static SInt32 Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 2  ||  parmsInfo.vMVolumeGrade == 0 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMVolumeGrade;
		}
	};
	
	struct GetVolumeParmsPrivID : plus::serialize_int< SInt16 >
	{
		static SInt16 Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 2 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMForeignPrivID;
		}
	};
	
	struct GetVolumeParmsExtended : plus::serialize_hex< UInt32 >
	{
		static UInt32 Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 3 )
			{
				throw FSTree_Property::Undefined();
			}
			
			return parmsInfo.vMExtendedAttributes;
		}
	};
	
	struct GetVolumeParmsDeviceID : plus::serialize_c_string_contents
	{
		static const char* Get( const GetVolParmsInfoBuffer& parmsInfo )
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
		const Mac::FSVolumeRefNum vRefNum = GetKey( that );
		
		memset( &parmsInfo, '\0', sizeof parmsInfo );
		
		HParamBlockRec pb = { 0 };
		
		HIOParam& io = pb.ioParam;
		
		io.ioVRefNum  = vRefNum;
		io.ioBuffer   = (char *) &parmsInfo;
		io.ioReqCount = sizeof parmsInfo;
		
		N::ThrowOSStatus( ::PBHGetVolParmsSync( &pb ) );
	}
	
	
	template < class Accessor >
	struct sys_mac_vol_N_Parms_Property
	{
		typedef Mac::FSVolumeRefNum Key;
		
		static void Read( plus::var_string& result, const FSTree* that, bool binary )
		{
			GetVolParmsInfoBuffer parmsInfo;
			
			GetVolParmsInfo( parmsInfo, that );
			
			const typename Accessor::result_type data = Accessor::Get( parmsInfo );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	
	template < class Accessor >
	static FSTreePtr Parms_Property_Factory( const FSTreePtr&     parent,
	                                         const plus::string&  name,
	                                         const void*          args )
	{
		typedef sys_mac_vol_N_Parms_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
		                            Accessor::fixed_size,
		                            &Property::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_vol_N_parms_Mappings[] =
	{
		{ "attrib",   &Parms_Property_Factory< GetVolumeParmsAttrib   > },
		{ "handle",   &Parms_Property_Factory< GetVolumeParmsHandle   > },
		{ "server",   &Parms_Property_Factory< GetVolumeParmsServer   > },
		
		{ "grade",    &Parms_Property_Factory< GetVolumeParmsGrade    > },
		{ "priv",     &Parms_Property_Factory< GetVolumeParmsPrivID   > },
		
		{ "extended", &Parms_Property_Factory< GetVolumeParmsExtended > },
		
		{ "device",   &Parms_Property_Factory< GetVolumeParmsDeviceID > },
		
		{ NULL, NULL }
		
	};
	
}

