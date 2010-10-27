/*
	Genie/FS/sys/mac/vol/list.cc
	----------------------------
*/

#include "Genie/FS/sys/mac/vol/list.hh"

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/deconstruct.hh"
#include "plus/freeze.hh"
#include "plus/serialize.hh"
#include "plus/stringify.hh"
#include "plus/var_string.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Folders.hh"
#include "Nitrogen/Gestalt.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/Drives.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/premapped.hh"
#include "Genie/FS/ResolvableSymLink.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/serialize_Str255.hh"
#include "Genie/FS/SymbolicLink.hh"
#include "Genie/FS/Trigger.hh"
#include "Genie/FS/sys/mac/vol/list/N/dt.hh"
#include "Genie/FS/sys/mac/vol/list/N/parms.hh"
#include "Genie/Utilities/AsyncIO.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Nitrogen
{
	
	static const Gestalt_Selector gestaltFSAttr = Gestalt_Selector( ::gestaltFSAttr );
	
	template <> struct GestaltDefault< gestaltFSAttr > : GestaltAttrDefaults {};
	
}

namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		return N::FSVolumeRefNum( -iota::parse_unsigned_decimal( parent->Name().c_str() ) );
	}
	
	static N::FSVolumeRefNum GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	
#if TARGET_API_MAC_CARBON
	
	static inline bool Has_PBXGetVolInfo()
	{
		return true;
	}
	
	// never called
	static UInt16 GetTotalBlocks( const XVolumeParam& volume )  { return 0; }
	static UInt16 GetFreeBlocks ( const XVolumeParam& volume )  { return 0; }
	
#else
	
	static bool Has_PBXGetVolInfo()
	{
		static bool result = N::Gestalt_Bit< N::gestaltFSAttr, gestaltFSSupports2TBVols >();
		
		return result;
	}
	
	static bool Has4GBVolumes()
	{
		static bool result = N::Gestalt_Bit< N::gestaltFSAttr, gestaltFSSupports4GBVols >();
		
		return result;
	}
	
	static const VCB* FindVCB( ::FSVolumeRefNum vRefNum )
	{
		const VCB* vcb = (VCB*) ::GetVCBQHdr()->qHead;
		
		while ( vcb != NULL )
		{
			if ( vcb->vcbVRefNum == vRefNum )
			{
				break;
			}
			
			vcb = (VCB*) vcb->qLink;
		}
		
		return vcb;
	}
	
	static inline UInt16 GetTotalBlocks( const XVolumeParam& volume )
	{
		const VCB* vcb = Has4GBVolumes() ? FindVCB( volume.ioVRefNum ) : NULL;
		
		return vcb ? vcb->vcbNmAlBlks : volume.ioVNmAlBlks;
	}
	
	static inline UInt16 GetFreeBlocks( const XVolumeParam& volume )
	{
		const VCB* vcb = Has4GBVolumes() ? FindVCB( volume.ioVRefNum ) : NULL;
		
		return vcb ? vcb->vcbFreeBks : volume.ioVFrBlk;
	}
	
#endif
	
	
	static bool is_valid_VolumeRefNum( N::FSVolumeRefNum key )
	{
		try
		{
			(void) MacIO::FSMakeFSSpec< FNF_Throws >( key, N::fsRtDirID, NULL );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != nsvErr )
			{
				throw;
			}
			
			return false;
		}
		
		return true;
	}
	
	struct valid_name_of_vol_number
	{
		typedef canonical_positive_integer well_formed_name;
		
		static bool applies( const plus::string& name )
		{
			if ( well_formed_name::applies( name ) )
			{
				const int i = iota::parse_unsigned_decimal( name.c_str() );
				
				if ( (i & 0xffff8000) == 0 )
				{
					return is_valid_VolumeRefNum( N::FSVolumeRefNum( -i ) );
				}
			}
			
			return false;
		}
	};
	
	
	extern const FSTree_Premapped::Mapping sys_mac_vol_N_Mappings[];
	
	static FSTreePtr vol_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( !valid_name_of_vol_number::applies( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_mac_vol_N_Mappings );
	}
	
	class vol_IteratorConverter
	{
		public:
			FSNode operator()( N::FSVolumeRefNum vRefNum ) const
			{
				const ino_t inode = -vRefNum;
				
				plus::string name = iota::inscribe_decimal( -vRefNum );
				
				return FSNode( inode, name );
			}
	};
	
	static void vol_iterate( FSTreeCache& cache )
	{
		vol_IteratorConverter converter;
		
		N::Volume_Container sequence = N::Volumes();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	struct Volume_Accessor_Defaults
	{
		static const bool needsName = false;
		
		static const bool neverZero = false;
	};
	
	struct GetVolumeName : Volume_Accessor_Defaults,
	                       serialize_Str255_contents
	{
		static const bool needsName = true;
		
		static const unsigned char* Get( const XVolumeParam& volume )
		{
			return volume.ioNamePtr;
		}
	};
	
	struct GetVolumeBlockCount : Volume_Accessor_Defaults,
	                             plus::serialize_unsigned< UInt32 >
	{
		// will break on 16TB volumes
		
		static UInt32 Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVTotalBytes / volume.ioVAlBlkSiz
			                           : GetTotalBlocks( volume );
		}
	};
	
	struct GetVolumeBlockSize : Volume_Accessor_Defaults,
	                            plus::serialize_unsigned< UInt32 >
	{
		static UInt32 Get( const XVolumeParam& volume )
		{
			return volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeBlockCount : Volume_Accessor_Defaults,
	                                 plus::serialize_unsigned< UInt32 >
	{
		static UInt32 Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVFreeBytes / volume.ioVAlBlkSiz
			                           : GetFreeBlocks( volume );
		}
	};
	
	struct GetVolumeCapacity : Volume_Accessor_Defaults,
	                           plus::serialize_unsigned< UInt64 >
	{
		static UInt64 Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVTotalBytes
			                           : GetTotalBlocks( volume ) * volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeSpace : Volume_Accessor_Defaults,
	                            plus::serialize_unsigned< UInt64 >
	{
		static UInt64 Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVFreeBytes
			                           : GetFreeBlocks( volume ) * volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeSignature : Volume_Accessor_Defaults,
	                            plus::serialize_c_string_contents
	{
		static const char* Get( const XVolumeParam& volume )
		{
			static char sigWord[] = "ab";
			
			sigWord[ 0 ] = volume.ioVSigWord >> 8;
			sigWord[ 1 ] = volume.ioVSigWord & 0xff;
			
			return sigWord;
		}
	};
	
	struct GetVolumeFSID : Volume_Accessor_Defaults,
	                       plus::serialize_int< SInt16 >
	{
		static SInt16 Get( const XVolumeParam& volume )
		{
			return volume.ioVFSID;
		}
	};
	
	struct GetVolumeWriteCount : Volume_Accessor_Defaults,
	                             plus::serialize_int< SInt32 >
	{
		static const bool neverZero = true;
		
		static SInt32 Get( const XVolumeParam& volume )
		{
			return volume.ioVWrCnt;
		}
	};
	
	struct GetVolumeFileCount : Volume_Accessor_Defaults,
	                            plus::serialize_int< SInt32 >
	{
		static const bool neverZero = true;
		
		static SInt32 Get( const XVolumeParam& volume )
		{
			return volume.ioVFilCnt;
		}
	};
	
	struct GetVolumeDirCount : Volume_Accessor_Defaults,
	                           plus::serialize_int< SInt32 >
	{
		static const bool neverZero = true;
		
		static SInt32 Get( const XVolumeParam& volume )
		{
			return volume.ioVDirCnt;
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
	
	static inline void PBHGetVInfoSync( XVolumeParam& pb, N::FSVolumeRefNum vRefNum, StringPtr name = NULL )
	{
		PBHGetVInfoSync( (HVolumeParam&) pb, vRefNum, name );
	}
	
	static void PBXGetVolInfoSync( XVolumeParam& pb, N::FSVolumeRefNum vRefNum, StringPtr name = NULL )
	{
		pb.ioNamePtr  = name;
		pb.ioVRefNum  = vRefNum;
		pb.ioXVersion = 0;
		pb.ioVolIndex = 0;
		
		N::ThrowOSStatus( ::PBXGetVolInfoSync( &pb ) );
	}
	
	static void GetVolInfo( XVolumeParam& pb, const FSTree* that, StringPtr name )
	{
		const N::FSVolumeRefNum vRefNum = GetKey( that );
		
		if ( Has_PBXGetVolInfo() )
		{
			PBXGetVolInfoSync( pb, vRefNum, name );
		}
		else
		{
			PBHGetVInfoSync( pb, vRefNum, name );
		}
	}
	
	template < class Accessor >
	struct sys_mac_vol_N_Property
	{
		typedef N::FSVolumeRefNum Key;
		
		static typename Accessor::result_type Get( const FSTree* that )
		{
			XVolumeParam pb;
			
			Str31 name;
			
			GetVolInfo( pb, that, Accessor::needsName ? name : NULL );
			
			return Accessor::Get( pb );
		}
		
		static void Read( plus::var_string& result, const FSTree* that, bool binary )
		{
			const typename Accessor::result_type data = Get( that );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	struct sys_mac_vol_N_name : sys_mac_vol_N_Property< GetVolumeName >
	{
		static void Write( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const N::FSVolumeRefNum vRefNum = GetKey( that );
			
			N::Str27 name( begin, end - begin );
			
			N::ThrowOSStatus( ::HRename( vRefNum, fsRtDirID, "\p", name ) );
		}
	};
	
	class FSTree_Folder_Link : public FSTree_ResolvableSymLink
	{
		private:
			typedef N::FSVolumeRefNum Key;
			
			Key            itsKey;
			N::FolderType  itsType;
		
		public:
			FSTree_Folder_Link( const FSTreePtr&     parent,
			                    const Key&           key,
			                    N::FolderType        type,
			                    const plus::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name ),
				itsKey ( key  ),
				itsType( type )
			{
			}
			
			FSTreePtr ResolveLink() const
			{
				const bool onServer = VolumeIsOnServer( itsKey );
				
				return FSTreeFromFSDirSpec( N::FindFolder( itsKey, itsType, false ), onServer );
			}
	};
	
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		typedef sys_mac_vol_N_Property< Accessor > Property;
		
		FSTreePtr result = New_FSTree_Property( parent,
		                                        name,
		                                        Accessor::fixed_size,
		                                        &Property::Read );
		
		if ( Accessor::neverZero  &&  Property::Get( result.get() ) == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		return result;
	}
	
	static FSTreePtr Volume_Name_Factory( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		typedef sys_mac_vol_N_name Property;
		
		const size_t size = 0;
		
		FSTreePtr result = New_FSTree_Property( parent,
		                                        name,
		                                        size,
		                                        &Property::Read,
		                                        &Property::Write );
		
		return result;
	}
	
	template < class Trigger >
	static FSTreePtr Trigger_Factory( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		N::FSVolumeRefNum key = GetKeyFromParent( parent );
		
		return seize_ptr( new Trigger( parent, name, key ) );
	}
	
	static FSTreePtr Root_Factory( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		N::FSVolumeRefNum key = GetKeyFromParent( parent );
		
		const Mac::FSDirSpec volume = n::make< Mac::FSDirSpec >( key, N::fsRtDirID );
		
		return FSTreeFromFSDirSpec( volume, VolumeIsOnServer( key ) );
	}
	
	static FSTreePtr Drive_Link_Factory( const FSTreePtr&     parent,
	                                     const plus::string&  name,
	                                     const void*          args )
	{
		N::FSVolumeRefNum key = GetKeyFromParent( parent );
		
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		if ( pb.ioVDrvInfo == 1 )
		{
			p7::throw_errno( ENOENT );
		}
		
		plus::string drive = iota::inscribe_decimal( pb.ioVDrvInfo );
		
		return New_FSTree_SymbolicLink( parent, name, "/sys/mac/drive/" + drive );
	}
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		N::FSVolumeRefNum key = GetKeyFromParent( parent );
		
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		if ( pb.ioVDRefNum == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		plus::string unit = iota::inscribe_decimal( ~pb.ioVDRefNum );
		
		return New_FSTree_SymbolicLink( parent, name, "/sys/mac/unit/" + unit );
	}
	
	template < N::FolderType type >
	static FSTreePtr Folder_Link_Factory( const FSTreePtr&     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		N::FSVolumeRefNum key = GetKeyFromParent( parent );
		
		return seize_ptr( new FSTree_Folder_Link( parent, key, type, name ) );
	}
	
	
	#define PREMAPPED( map )  &premapped_factory, (const void*) map
	
	const FSTree_Premapped::Mapping sys_mac_vol_N_Mappings[] =
	{
		{ "name", &Volume_Name_Factory },
		
		{ "block-size",  &Property_Factory< GetVolumeBlockSize      > },
		{ "blocks",      &Property_Factory< GetVolumeBlockCount     > },
		{ "blocks-free", &Property_Factory< GetVolumeFreeBlockCount > },
		
		{ "capacity",  &Property_Factory< GetVolumeCapacity  > },
		{ "freespace", &Property_Factory< GetVolumeFreeSpace > },
		
		{ "sig", &Property_Factory< GetVolumeSignature > },
		
		{ "drive",  &Drive_Link_Factory  },
		{ "driver", &Driver_Link_Factory },
		
		{ "fsid", &Property_Factory< GetVolumeFSID > },
		
		{ "writes", &Property_Factory< GetVolumeWriteCount > },
		{ "files",  &Property_Factory< GetVolumeFileCount  > },
		{ "dirs",   &Property_Factory< GetVolumeDirCount   > },
		
		{ "dt",    PREMAPPED( sys_mac_vol_list_N_dt_Mappings ) },
		{ "parms", PREMAPPED( sys_mac_vol_N_parms_Mappings   ) },
		
		// volume roots are named "mnt", not the volume name
		{ "mnt",  &Root_Factory },
		
		{ "flush",  &Trigger_Factory< Trigger< Volume_Flush   > > },
		{ "umount", &Trigger_Factory< Trigger< Volume_Unmount > > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "eject",  &Trigger_Factory< Trigger< Volume_Eject   > > },
		
	#endif
		
		{ "sys", &Folder_Link_Factory< N::kSystemFolderType    > },
		{ "tmp", &Folder_Link_Factory< N::kTemporaryFolderType > },
		
		{ NULL, NULL }
		
	};
	
	FSTreePtr New_FSTree_sys_mac_vol( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return new_basic_directory( parent, name, vol_lookup, vol_iterate );
	}
	
	FSTreePtr Get_sys_mac_vol_N( N::FSVolumeRefNum vRefNum )
	{
		FSTreePtr parent = ResolveAbsolutePath( STR_LEN( "/sys/mac/vol/list" ) );
		
		const plus::string name = iota::inscribe_decimal( -vRefNum );
		
		return Premapped_Factory( parent, name, sys_mac_vol_N_Mappings );
	}
	
}

