/*
	sys_mac_vol.cc
	--------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/sys_mac_vol.hh"

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

// iota
#include "iota/decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Folders.hh"

// Genie
#include "Genie/FS/ReadableSymLink.hh"
#include "Genie/FS/sys_mac_vol_list.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	class sys_mac_vol_boot : public FSTree_ReadableSymLink
	{
		public:
			sys_mac_vol_boot( const FSTreePtr&    parent,
			                  const std::string&  name )
			:
				FSTree_ReadableSymLink( parent, name )
			{
			}
			
			std::string ReadLink() const;
	};
	
	std::string sys_mac_vol_boot::ReadLink() const
	{
		const N::FSDirSpec system_folder = N::FindFolder( N::kOnAppropriateDisk,
		                                                  N::kSystemFolderType,
		                                                  false );
		
		const N::FSVolumeRefNum vRefNum = system_folder.vRefNum;
		
		std::string result = "list/";
		
		result += iota::inscribe_decimal( -vRefNum );
		
		return result;
	}
	
	
	class sys_mac_vol_ram : public FSTree_ReadableSymLink
	{
		public:
			sys_mac_vol_ram( const FSTreePtr&    parent,
			                 const std::string&  name )
			:
				FSTree_ReadableSymLink( parent, name )
			{
			}
			
			bool Exists() const;
			
			std::string ReadLink() const;
	};
	
	struct volume_is_ram_disk
	{
		static bool applies( N::FSVolumeRefNum vRefNum );
		
		bool operator()( N::FSVolumeRefNum vRefNum ) const
		{
			return applies( vRefNum );
		}
	};
	
	static void PBHGetVInfoSync( HVolumeParam& pb, N::FSVolumeRefNum vRefNum, StringPtr name = NULL )
	{
		pb.ioNamePtr  = name;
		pb.ioVRefNum  = vRefNum;
		pb.filler2    = 0;
		pb.ioVolIndex = 0;
		
		N::ThrowOSStatus( ::PBHGetVInfoSync( (HParamBlockRec*) &pb ) );
	}
	
#if !TARGET_API_MAC_CARBON
	
	static inline AuxDCEHandle* GetUTableBase()
	{
		return (AuxDCEHandle*) LMGetUTableBase();
	}
	
	extern const unsigned char* GetDriverName_WithinHandle( AuxDCEHandle dceHandle );
	
#endif
	
	bool volume_is_ram_disk::applies( N::FSVolumeRefNum vRefNum )
	{
	#if !TARGET_API_MAC_CARBON
		
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, vRefNum );
		
		if ( pb.ioVDRefNum == 0 )
		{
			return false;
		}
		
		const SInt16 unit_number = ~pb.ioVDRefNum;
		
		const AuxDCEHandle dceHandle = GetUTableBase()[ unit_number ];
		
		const unsigned char* name = GetDriverName_WithinHandle( dceHandle );
		
		return std::equal( name,
		                   name + 1 + name[0],
		                   "\p" ".EDisk" );
		
	#endif
		
		return false;
	}
	
	static N::FSVolumeRefNum find_ram_disk()
	{
		typedef N::Volume_Container::const_iterator iterator;
		
		N::Volume_Container sequence = N::Volumes();
		
		const iterator it = std::find_if( sequence.begin(),
		                                  sequence.end(),
		                                  volume_is_ram_disk() );
		
		return it != sequence.end() ? *it : N::FSVolumeRefNum( 0 );
	}
	
	bool sys_mac_vol_ram::Exists() const
	{
		return find_ram_disk();
	}
	
	std::string sys_mac_vol_ram::ReadLink() const
	{
		const SInt16 vRefNum = find_ram_disk();
		
		if ( vRefNum == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		std::string result = "list/";
		
		result += iota::inscribe_decimal( -vRefNum );
		
		return result;
	}
	
	
	const FSTree_Premapped::Mapping sys_mac_vol_Mappings[] =
	{
		{ "list", &New_FSTree_sys_mac_vol },
		
		{ "boot", &Basic_Factory< sys_mac_vol_boot > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "ram",  &Basic_Factory< sys_mac_vol_ram  >, true },
		
	#endif
		
		{ NULL, NULL }
		
	};
	
}

