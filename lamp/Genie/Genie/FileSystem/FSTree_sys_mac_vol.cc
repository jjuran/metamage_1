/*	=====================
 *	FSTree_sys_mac_vol.cc
 *	=====================
 */

#include "Genie/FileSystem/FSTree_sys_mac_vol.hh"

// Nitrogen
#include "Nitrogen/Folders.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/Drives.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_Stamp_Action.hh"
#include "Genie/FileSystem/FSTree_Virtual_Link.hh"


namespace Nitrogen
{
	
	static const Gestalt_Selector gestaltFSAttr = Gestalt_Selector( ::gestaltFSAttr );
	
	template <> struct GestaltDefault< gestaltFSAttr > : GestaltAttrDefaults {};
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		return N::FSVolumeRefNum( -std::atoi( parent->Name().c_str() ) );
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
	static inline UInt16 GetTotalBlocks( const XVolumeParam& volume );
	static inline UInt16 GetFreeBlocks ( const XVolumeParam& volume );
	
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
	
	
	std::string VRefNum_KeyName_Traits::NameFromKey( const Key& key )
	{
		return Int_KeyName_Traits::NameFromKey( -key );
	}
	
	VRefNum_KeyName_Traits::Key VRefNum_KeyName_Traits::KeyFromName( const std::string& name )
	{
		int n = Int_KeyName_Traits::KeyFromName( name );
		
		if ( n <= 0  ||  SInt16( n ) != n )
		{
			p7::throw_errno( ENOENT );
		}
		
		N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -n );
		
		return vRefNum;
	}
	
	bool VRefNum_KeyName_Traits::KeyIsValid( const Key& key )
	{
		try
		{
			(void) N::FSMakeFSSpec( key, N::fsRtDirID, NULL );
		}
		catch ( const N::NSVErr& err )
		{
		#ifdef __MWERKS__
			
			if ( err.Get() != nsvErr )
			{
				throw;
			}
			
		#endif
			
			return false;
		}
		
		return true;
	}
	
	
	extern const Functional_Traits< void >::Mapping sys_mac_vol_N_Mappings[];
	
	FSTreePtr sys_mac_vol_Details::GetChildNode( const FSTreePtr&    parent,
		                                         const std::string&  name,
		                                         const Key&          key )
	{
		return Premapped_Factory< sys_mac_vol_N_Mappings >( parent, name );
	}
	
	
	struct GetVolumeName
	{
		static const bool needsName = true;
		
		typedef const unsigned char* Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioNamePtr;
		}
	};
	
	struct GetVolumeBlockCount
	{
		static const bool needsName = false;
		
		typedef UInt32 Result;  // will break on 16TB volumes
		
		static Result Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVTotalBytes / volume.ioVAlBlkSiz
			                           : GetTotalBlocks( volume );
		}
	};
	
	struct GetVolumeBlockSize
	{
		static const bool needsName = false;
		
		typedef UInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeBlockCount
	{
		static const bool needsName = false;
		
		typedef UInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVFreeBytes / volume.ioVAlBlkSiz
			                           : GetFreeBlocks( volume );
		}
	};
	
	struct GetVolumeCapacity
	{
		static const bool needsName = false;
		
		typedef UInt64 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVTotalBytes
			                           : GetTotalBlocks( volume ) * volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeSpace
	{
		static const bool needsName = false;
		
		typedef UInt64 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVFreeBytes
			                           : GetFreeBlocks( volume ) * volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeSignature
	{
		static const bool needsName = false;
		
		typedef const char* Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			static char sigWord[] = "ab";
			
			sigWord[ 0 ] = volume.ioVSigWord >> 8;
			sigWord[ 1 ] = volume.ioVSigWord & 0xff;
			
			return sigWord;
		}
	};
	
	struct GetVolumeFSID
	{
		static const bool needsName = false;
		
		typedef SInt16 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioVFSID;
		}
	};
	
	struct GetVolumeWriteCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioVWrCnt;
		}
	};
	
	struct GetVolumeFileCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioVFilCnt;
		}
	};
	
	struct GetVolumeDirCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
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
	
	inline void PBHGetVInfoSync( XVolumeParam& pb, N::FSVolumeRefNum vRefNum, StringPtr name = NULL )
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
	
	template < class Accessor >
	struct sys_mac_vol_N_Property
	{
		typedef N::FSVolumeRefNum Key;
		
		static std::string Read( const FSTree* that )
		{
			Key key = GetKey( that );
			
			XVolumeParam pb;
			
			Str31 name;
			
			if ( Has_PBXGetVolInfo() )
			{
				PBXGetVolInfoSync( pb, key, Accessor::needsName ? name : NULL );
			}
			else
			{
				PBHGetVInfoSync( pb, key, Accessor::needsName ? name : NULL );
			}
			
			return NN::Convert< std::string >( Accessor::Get( pb ) );
		}
	};
	
	class FSTree_Folder_Link : public FSTree
	{
		private:
			typedef N::FSVolumeRefNum Key;
			
			Key            itsKey;
			N::FolderType  itsType;
		
		public:
			FSTree_Folder_Link( const FSTreePtr&    parent,
			                    const Key&          key,
			                    N::FolderType       type,
			                    const std::string&  name ) : FSTree( parent, name ),
			                                                 itsKey ( key  ),
			                                                 itsType( type )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const
			{
				return FSTreeFromFSSpec( NN::Convert< FSSpec >( N::FindFolder( itsKey, itsType, false ) ) );
			}
	};
	
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_vol_N_Property< Accessor > Property;
		
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read ) );
	}
	
	template < class Stamp >
	static FSTreePtr Stamp_Factory( const FSTreePtr&    parent,
	                                const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		return FSTreePtr( new Stamp( parent, name, key ) );
	}
	
	static FSTreePtr Root_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		FSSpec volume = N::FSMakeFSSpec( key, N::fsRtDirID, "\p" );
		
		return FSTreeFromFSSpec( volume );
	}
	
	static FSTreePtr Drive_Link_Factory( const FSTreePtr&    parent,
	                                     const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		std::string drive = NN::Convert< std::string >( pb.ioVDrvInfo );
		
		return FSTreePtr( new FSTree_Virtual_Link( parent, name, "/sys/mac/drive/" + drive ) );
	}
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&    parent,
	                                      const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		std::string unit = NN::Convert< std::string >( ~pb.ioVDRefNum );
		
		return FSTreePtr( new FSTree_Virtual_Link( parent, name, "/sys/mac/unit/" + unit ) );
	}
	
	template < N::FolderType type >
	static FSTreePtr Folder_Link_Factory( const FSTreePtr&    parent,
	                                      const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		return FSTreePtr( new FSTree_Folder_Link( parent, key, type, name ) );
	}
	
	const Functional_Traits< void >::Mapping sys_mac_vol_N_Mappings[] =
	{
		{ "name", &Property_Factory< GetVolumeName > },
		
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
		
		// volume roots are named "mnt", not the volume name
		{ "mnt",  &Root_Factory },
		
		{ "flush",  &Stamp_Factory< FSTree_Stamp_Action< Volume_Flush   > > },
		{ "umount", &Stamp_Factory< FSTree_Stamp_Action< Volume_Unmount > > },
		
	#if !TARGET_API_MAC_CARBON
		
		{ "eject",  &Stamp_Factory< FSTree_Stamp_Action< Volume_Eject   > > },
		
	#endif
		
		{ "sys", &Folder_Link_Factory< N::kSystemFolderType    > },
		{ "tmp", &Folder_Link_Factory< N::kTemporaryFolderType > },
		
		{ NULL, NULL }
		
	};
	
}

