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
		static const bool hexEncoded = true;
		
		static const bool alwaysStringified = false;
		
		typedef UInt32 Result;
		
		static Result Get( const GetVolParmsInfoBuffer& parmsInfo )
		{
			return parmsInfo.vMAttrib;
		}
	};
	
	struct GetVolumeParmsHandle
	{
		static const bool hexEncoded = true;
		
		static const bool alwaysStringified = false;
		
		typedef ::Handle Result;
		
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
		static const bool hexEncoded = true;
		
		static const bool alwaysStringified = false;
		
		typedef UInt32 Result;
		
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
		static const bool hexEncoded = false;
		
		static const bool alwaysStringified = false;
		
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
		static const bool hexEncoded = false;
		
		static const bool alwaysStringified = false;
		
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
		static const bool hexEncoded = true;
		
		static const bool alwaysStringified = false;
		
		typedef UInt32 Result;
		
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
		static const bool hexEncoded = false;
		
		static const bool alwaysStringified = true;
		
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
			
			const typename Accessor::Result data = Accessor::Get( parmsInfo );
			
			const bool raw = !Accessor::alwaysStringified  &&  binary;
			const bool hex =  Accessor::hexEncoded;
			
			using BitsAndBytes::EncodeAsHex;
			
			std::string result = raw ? std::string( (char*) &data, sizeof data )
			                   : hex ? EncodeAsHex(         &data, sizeof data )
			                   :       NN::Convert< std::string >( data );
			
			return result;
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

