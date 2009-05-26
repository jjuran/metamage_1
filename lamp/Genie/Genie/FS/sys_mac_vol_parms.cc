/*	====================
 *	sys_mac_vol_parms.cc
 *	====================
 */

#include "Genie/FS/sys_mac_vol_parms.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
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
		typedef std::string Result;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			using BitsAndBytes::EncodeAsHex;
			
			return EncodeAsHex( &parmsInfo.vMAttrib, sizeof parmsInfo.vMAttrib );
		}
	};
	
	struct GetVolumeParmsHandle
	{
		typedef std::string Result;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMLocalHand == 0 )
			{
				throw FSTree_Property::Undefined();
			}
			
			using BitsAndBytes::EncodeAsHex;
			
			return EncodeAsHex( &parmsInfo.vMLocalHand, sizeof parmsInfo.vMLocalHand );
		}
	};
	
	struct GetVolumeParmsServer
	{
		typedef std::string Result;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMServerAdr == 0 )
			{
				throw FSTree_Property::Undefined();
			}
			
			using BitsAndBytes::EncodeAsHex;
			
			return EncodeAsHex( &parmsInfo.vMServerAdr, sizeof parmsInfo.vMServerAdr );
		}
	};
	
	struct GetVolumeParmsGrade
	{
		typedef SInt32 Result;
		
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
		typedef SInt16 Result;
		
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
		typedef std::string Result;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 3 )
			{
				throw FSTree_Property::Undefined();
			}
			
			using BitsAndBytes::EncodeAsHex;
			
			return EncodeAsHex( &parmsInfo.vMExtendedAttributes, sizeof parmsInfo.vMExtendedAttributes );
		}
	};
	
	struct GetVolumeParmsDeviceID
	{
		typedef const char* Result;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			if ( parmsInfo.vMVersion < 4  ||  parmsInfo.vMDeviceID == NULL )
			{
				throw FSTree_Property::Undefined();
			}
			
			return (const char*) parmsInfo.vMDeviceID;
		}
	};
	
	template < class Accessor >
	struct sys_mac_vol_N_Parms_Property
	{
		typedef N::FSVolumeRefNum Key;
		
		static std::string Read( const FSTree* that, bool binary )
		{
			Key vRefNum = GetKey( that );
			
			HParamBlockRec pb = { 0 };
			
			HIOParam& io = pb.ioParam;
			
			GetVolParmsInfoBuffer parmsInfo = { 0 };
			
			io.ioVRefNum  = vRefNum;
			io.ioBuffer   = (char *) &parmsInfo;
			io.ioReqCount = sizeof parmsInfo;
			
			N::ThrowOSStatus( ::PBHGetVolParmsSync( &pb ) );
			
			return NN::Convert< std::string >( Accessor::Get( parmsInfo ) );
		}
	};
	
	
	template < class Accessor >
	static FSTreePtr Parms_Property_Factory( const FSTreePtr&    parent,
	                                         const std::string&  name )
	{
		typedef sys_mac_vol_N_Parms_Property< Accessor > Property;
		
		return New_FSTree_Property( parent,
		                            name,
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

