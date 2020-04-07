/*
	Genie/FS/sys/mac/vol/list/N/parms.cc
	------------------------------------
*/

#include "Genie/FS/sys/mac/vol/list/N/parms.hh"

// mac-types
#include "mac_types/GetVolParmsInfoBuffer.hh"

// mac-sys-utils
#include "mac_sys/volume_params.hh"

// gear
#include "gear/parse_decimal.hh"

// plus
#include "plus/deconstruct.hh"
#include "plus/freeze.hh"
#include "plus/serialize.hh"
#include "plus/stringify.hh"
#include "plus/var_string.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"


namespace Genie
{
	
	using mac::types::GetVolParmsInfoBuffer;
	
	
	static short GetKeyFromParent( const vfs::node* parent )
	{
		return -gear::parse_unsigned_decimal( parent->name().c_str() );
	}
	
	static short GetKey( const vfs::node* that )
	{
		return GetKeyFromParent( that->owner() );
	}
	
	
	struct GetVolumeParmsAttrib : plus::serialize_hex< uint32_t >
	{
		static uint32_t Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			return parmsInfo.vMAttrib;
		}
	};
	
	struct GetVolumeParmsHandle : plus::serialize_pointer
	{
		static void* Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMLocalHand == 0 )
			{
				throw vfs::undefined_property();
			}
			
			return parmsInfo.vMLocalHand;
		}
	};
	
	struct GetVolumeParmsServer : plus::serialize_hex< uint32_t >
	{
		static uint32_t Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMServerAdr == 0 )
			{
				throw vfs::undefined_property();
			}
			
			return parmsInfo.vMServerAdr;
		}
	};
	
	struct GetVolumeParmsGrade : plus::serialize_int< int32_t >
	{
		static int32_t Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 2  ||  parmsInfo.vMVolumeGrade == 0 )
			{
				throw vfs::undefined_property();
			}
			
			return parmsInfo.vMVolumeGrade;
		}
	};
	
	struct GetVolumeParmsPrivID : plus::serialize_int< int16_t >
	{
		static int16_t Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 2 )
			{
				throw vfs::undefined_property();
			}
			
			return parmsInfo.vMForeignPrivID;
		}
	};
	
	struct GetVolumeParmsExtended : plus::serialize_hex< uint32_t >
	{
		static uint32_t Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 3 )
			{
				throw vfs::undefined_property();
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
				throw vfs::undefined_property();
			}
			
			return (const char*) parmsInfo.vMDeviceID;
		}
	};
	
	
	#define DEFINE_GETTER( p )  \
	static void p##_get( plus::var_string& result, const vfs::node* that, bool binary )  \
	{  \
		typedef p Accessor;                                             \
		GetVolParmsInfoBuffer parmsInfo;                                \
		mac::sys::get_volume_params( parmsInfo, GetKey( that ) );       \
		const Accessor::result_type data = Accessor::Get( parmsInfo );  \
		Accessor::deconstruct::apply( result, data, binary );           \
	}
	
	DEFINE_GETTER( GetVolumeParmsAttrib   );
	DEFINE_GETTER( GetVolumeParmsHandle   );
	DEFINE_GETTER( GetVolumeParmsServer   );
	DEFINE_GETTER( GetVolumeParmsGrade    );
	DEFINE_GETTER( GetVolumeParmsPrivID   );
	DEFINE_GETTER( GetVolumeParmsExtended );
	DEFINE_GETTER( GetVolumeParmsDeviceID );
	
	#define DEFINE_PARAMS( p )  \
	static const vfs::property_params p##_params = {p::fixed_size, &p##_get}
	
	DEFINE_PARAMS( GetVolumeParmsAttrib   );
	DEFINE_PARAMS( GetVolumeParmsHandle   );
	DEFINE_PARAMS( GetVolumeParmsServer   );
	DEFINE_PARAMS( GetVolumeParmsGrade    );
	DEFINE_PARAMS( GetVolumeParmsPrivID   );
	DEFINE_PARAMS( GetVolumeParmsExtended );
	DEFINE_PARAMS( GetVolumeParmsDeviceID );
	
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_vol_N_parms_Mappings[] =
	{
		{ "attrib",   PROPERTY( GetVolumeParmsAttrib   ) },
		{ "handle",   PROPERTY( GetVolumeParmsHandle   ) },
		{ "server",   PROPERTY( GetVolumeParmsServer   ) },
		
		{ "grade",    PROPERTY( GetVolumeParmsGrade    ) },
		{ "priv",     PROPERTY( GetVolumeParmsPrivID   ) },
		
		{ "extended", PROPERTY( GetVolumeParmsExtended ) },
		
		{ "device",   PROPERTY( GetVolumeParmsDeviceID ) },
		
		{ NULL, NULL }
		
	};
	
}
