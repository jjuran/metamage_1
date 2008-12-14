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
#include "Genie/FileSystem/FSTree_QueryFile.hh"
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
	
	
	extern const Functional_Traits< VRefNum_KeyName_Traits::Key >::Mapping sys_mac_vol_N_Mappings[];
	
	FSTreePtr sys_mac_vol_Details::GetChildNode( const FSTreePtr&    parent,
		                                         const std::string&  name,
		                                         const Key&          key )
	{
		return Premapped_Factory< Key, sys_mac_vol_N_Mappings >( parent, name, key );
	}
	
	
	struct GetVolumeName
	{
		static const bool needsName = true;
		
		typedef const unsigned char* Result;
		
		Result Get( const XVolumeParam& volume ) const
		{
			return volume.ioNamePtr;
		}
	};
	
	struct GetVolumeBlockCount
	{
		static const bool needsName = false;
		
		typedef UInt32 Result;  // will break on 16TB volumes
		
		Result Get( const XVolumeParam& volume ) const
		{
			return Has_PBXGetVolInfo() ? volume.ioVTotalBytes / volume.ioVAlBlkSiz
			                           : GetTotalBlocks( volume );
		}
	};
	
	struct GetVolumeBlockSize
	{
		static const bool needsName = false;
		
		typedef UInt32 Result;
		
		Result Get( const XVolumeParam& volume ) const
		{
			return volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeBlockCount
	{
		static const bool needsName = false;
		
		typedef UInt32 Result;
		
		Result Get( const XVolumeParam& volume ) const
		{
			return Has_PBXGetVolInfo() ? volume.ioVFreeBytes / volume.ioVAlBlkSiz
			                           : GetFreeBlocks( volume );
		}
	};
	
	struct GetVolumeCapacity
	{
		static const bool needsName = false;
		
		typedef UInt64 Result;
		
		Result Get( const XVolumeParam& volume ) const
		{
			return Has_PBXGetVolInfo() ? volume.ioVTotalBytes
			                           : GetTotalBlocks( volume ) * volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeSpace
	{
		static const bool needsName = false;
		
		typedef UInt64 Result;
		
		Result Get( const XVolumeParam& volume ) const
		{
			return Has_PBXGetVolInfo() ? volume.ioVFreeBytes
			                           : GetFreeBlocks( volume ) * volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeSignature
	{
		static const bool needsName = false;
		
		typedef const char* Result;
		
		Result Get( const XVolumeParam& volume ) const
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
		
		Result Get( const XVolumeParam& volume ) const
		{
			return volume.ioVFSID;
		}
	};
	
	struct GetVolumeWriteCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		Result Get( const XVolumeParam& volume ) const
		{
			return volume.ioVWrCnt;
		}
	};
	
	struct GetVolumeFileCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		Result Get( const XVolumeParam& volume ) const
		{
			return volume.ioVFilCnt;
		}
	};
	
	struct GetVolumeDirCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		Result Get( const XVolumeParam& volume ) const
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
	class sys_mac_vol_N_Query
	{
		private:
			typedef N::FSVolumeRefNum Key;
			
			Key itsKey;
		
		public:
			sys_mac_vol_N_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				XVolumeParam pb;
				
				Str31 name;
				
				if ( Has_PBXGetVolInfo() )
				{
					PBXGetVolInfoSync( pb, itsKey, Accessor::needsName ? name : NULL );
				}
				else
				{
					PBHGetVInfoSync( pb, itsKey, Accessor::needsName ? name : NULL );
				}
				
				std::string output = NN::Convert< std::string >( Accessor().Get( pb ) ) + "\n";
				
				return output;
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
	
	
	template < class Get >
	static FSTreePtr Query_Factory( const FSTreePtr&             parent,
	                                const std::string&           name,
	                                VRefNum_KeyName_Traits::Key  key )
	{
		typedef sys_mac_vol_N_Query< Get > Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return FSTreePtr( new QueryFile( parent, name, Query( key ) ) );
	}
	
	template < class Stamp >
	static FSTreePtr Stamp_Factory( const FSTreePtr&             parent,
	                                const std::string&           name,
	                                VRefNum_KeyName_Traits::Key  key )
	{
		return FSTreePtr( new Stamp( parent, name, key ) );
	}
	
	static FSTreePtr Root_Factory( const FSTreePtr&             parent,
	                               const std::string&           name,
	                               VRefNum_KeyName_Traits::Key  key )
	{
		FSSpec volume = N::FSMakeFSSpec( key, N::fsRtDirID, "\p" );
		
		return FSTreeFromFSSpec( volume );
	}
	
	static FSTreePtr Drive_Link_Factory( const FSTreePtr&             parent,
	                                     const std::string&           name,
	                                     VRefNum_KeyName_Traits::Key  key )
	{
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		std::string drive = NN::Convert< std::string >( pb.ioVDrvInfo );
		
		return FSTreePtr( new FSTree_Virtual_Link( parent, name, "/sys/mac/drive/" + drive ) );
	}
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&             parent,
	                                      const std::string&           name,
	                                      VRefNum_KeyName_Traits::Key  key )
	{
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		std::string unit = NN::Convert< std::string >( ~pb.ioVDRefNum );
		
		return FSTreePtr( new FSTree_Virtual_Link( parent, name, "/sys/mac/unit/" + unit ) );
	}
	
	template < N::FolderType type >
	static FSTreePtr Folder_Link_Factory( const FSTreePtr&             parent,
	                                      const std::string&           name,
	                                      VRefNum_KeyName_Traits::Key  key )
	{
		return FSTreePtr( new FSTree_Folder_Link( parent, key, type, name ) );
	}
	
	const Functional_Traits< VRefNum_KeyName_Traits::Key >::Mapping sys_mac_vol_N_Mappings[] =
	{
		{ "name", &Query_Factory< GetVolumeName > },
		
		{ "block-size",  &Query_Factory< GetVolumeBlockSize      > },
		{ "blocks",      &Query_Factory< GetVolumeBlockCount     > },
		{ "blocks-free", &Query_Factory< GetVolumeFreeBlockCount > },
		
		{ "capacity",  &Query_Factory< GetVolumeCapacity  > },
		{ "freespace", &Query_Factory< GetVolumeFreeSpace > },
		
		{ "sig", &Query_Factory< GetVolumeSignature > },
		
		{ "drive",  &Drive_Link_Factory  },
		{ "driver", &Driver_Link_Factory },
		
		{ "fsid", &Query_Factory< GetVolumeFSID > },
		
		{ "writes", &Query_Factory< GetVolumeWriteCount > },
		{ "files",  &Query_Factory< GetVolumeFileCount  > },
		{ "dirs",   &Query_Factory< GetVolumeDirCount   > },
		
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

