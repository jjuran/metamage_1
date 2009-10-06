/*	=====================
 *	FSTree_sys_mac_vol.cc
 *	=====================
 */

#include "Genie/FS/FSTree_sys_mac_vol.hh"

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/Folders.h"
#include "Nitrogen/Gestalt.h"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"

// poseven
#include "poseven/Errno.hh"

// Genie
#include "Genie/FS/Drives.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/FSTree_Virtual_Link.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/Trigger.hh"
#include "Genie/FS/sys_mac_vol_parms.hh"
#include "Genie/Utilities/AsyncIO.hh"


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
	
	
	struct VRefNum_KeyName_Traits
	{
		typedef N::FSVolumeRefNum Key;
		
		static std::string NameFromKey( const Key& key );
		
		static Key KeyFromName( const std::string& name );
		
		static bool KeyIsValid( const Key& key );
	};
	
	
	struct sys_mac_vol_Details : public VRefNum_KeyName_Traits
	{
		typedef N::Volume_Container Sequence;
		
		static Sequence ItemSequence()  { return N::Volumes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_vol_Details > FSTree_sys_mac_vol;
	
	
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
	
	
	extern const FSTree_Premapped::Mapping sys_mac_vol_N_Mappings[];
	
	FSTreePtr sys_mac_vol_Details::GetChildNode( const FSTreePtr&    parent,
		                                         const std::string&  name,
		                                         const Key&          key )
	{
		return Premapped_Factory< sys_mac_vol_N_Mappings >( parent, name );
	}
	
	
	struct Volume_Accessor_Defaults
	{
		static const bool needsName = false;
		
		static const bool alwaysStringified = false;
		
		static const bool neverZero = false;
	};
	
	struct GetVolumeName : Volume_Accessor_Defaults
	{
		static const bool needsName = true;
		
		static const bool alwaysStringified = true;
		
		typedef const unsigned char* Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioNamePtr;
		}
	};
	
	struct GetVolumeBlockCount : Volume_Accessor_Defaults
	{
		typedef UInt32 Result;  // will break on 16TB volumes
		
		static Result Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVTotalBytes / volume.ioVAlBlkSiz
			                           : GetTotalBlocks( volume );
		}
	};
	
	struct GetVolumeBlockSize : Volume_Accessor_Defaults
	{
		typedef UInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeBlockCount : Volume_Accessor_Defaults
	{
		typedef UInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVFreeBytes / volume.ioVAlBlkSiz
			                           : GetFreeBlocks( volume );
		}
	};
	
	struct GetVolumeCapacity : Volume_Accessor_Defaults
	{
		typedef UInt64 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVTotalBytes
			                           : GetTotalBlocks( volume ) * volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeSpace : Volume_Accessor_Defaults
	{
		typedef UInt64 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return Has_PBXGetVolInfo() ? volume.ioVFreeBytes
			                           : GetFreeBlocks( volume ) * volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeSignature : Volume_Accessor_Defaults
	{
		static const bool alwaysStringified = true;
		
		typedef const char* Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			static char sigWord[] = "ab";
			
			sigWord[ 0 ] = volume.ioVSigWord >> 8;
			sigWord[ 1 ] = volume.ioVSigWord & 0xff;
			
			return sigWord;
		}
	};
	
	struct GetVolumeFSID : Volume_Accessor_Defaults
	{
		typedef SInt16 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioVFSID;
		}
	};
	
	struct GetVolumeWriteCount : Volume_Accessor_Defaults
	{
		static const bool neverZero = true;
		
		typedef SInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioVWrCnt;
		}
	};
	
	struct GetVolumeFileCount : Volume_Accessor_Defaults
	{
		static const bool neverZero = true;
		
		typedef SInt32 Result;
		
		static Result Get( const XVolumeParam& volume )
		{
			return volume.ioVFilCnt;
		}
	};
	
	struct GetVolumeDirCount : Volume_Accessor_Defaults
	{
		static const bool neverZero = true;
		
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
		
		static typename Accessor::Result Get( const FSTree* that )
		{
			XVolumeParam pb;
			
			Str31 name;
			
			GetVolInfo( pb, that, Accessor::needsName ? name : NULL );
			
			const typename Accessor::Result data = Accessor::Get( pb );
			
			return data;
		}
		
		static std::string Read( const FSTree* that, bool binary )
		{
			const typename Accessor::Result data = Get( that );
			
			const bool raw = !Accessor::alwaysStringified  &&  binary;
			
			std::string result = raw ? std::string( (char*) &data, sizeof data )
			                   :       NN::Convert< std::string >( data );
			
			return result;
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
				const bool onServer = VolumeIsOnServer( itsKey );
				
				return FSTreeFromFSDirSpec( N::FindFolder( itsKey, itsType, false ), onServer );
			}
	};
	
	
	template < class Accessor >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		typedef sys_mac_vol_N_Property< Accessor > Property;
		
		const bool fixed = !Accessor::alwaysStringified;
		
		const size_t size = fixed ? sizeof (typename Accessor::Result) : 0;
		
		FSTreePtr result = New_FSTree_Property( parent,
		                                        name,
		                                        size,
		                                        &Property::Read );
		
		if ( Accessor::neverZero  &&  Property::Get( result.get() ) == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		return result;
	}
	
	static FSTreePtr Volume_Name_Factory( const FSTreePtr&    parent,
	                                      const std::string&  name )
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
	static FSTreePtr Trigger_Factory( const FSTreePtr&    parent,
	                                  const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		return FSTreePtr( new Trigger( parent, name, key ) );
	}
	
	static FSTreePtr Root_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		FSSpec volume = MacIO::FSMakeFSSpec< FNF_Throws >( key, N::fsRtDirID, "\p" );
		
		return FSTreeFromFSSpec( volume, VolumeIsOnServer( key ) );
	}
	
	static FSTreePtr Drive_Link_Factory( const FSTreePtr&    parent,
	                                     const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		if ( pb.ioVDrvInfo == 1 )
		{
			p7::throw_errno( ENOENT );
		}
		
		std::string drive = iota::inscribe_decimal( pb.ioVDrvInfo );
		
		return New_FSTree_Virtual_Link( parent, name, "/sys/mac/drive/" + drive );
	}
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&    parent,
	                                      const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		if ( pb.ioVDRefNum == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		std::string unit = iota::inscribe_decimal( ~pb.ioVDRefNum );
		
		return New_FSTree_Virtual_Link( parent, name, "/sys/mac/unit/" + unit );
	}
	
	template < N::FolderType type >
	static FSTreePtr Folder_Link_Factory( const FSTreePtr&    parent,
	                                      const std::string&  name )
	{
		VRefNum_KeyName_Traits::Key key = GetKeyFromParent( parent );
		
		return FSTreePtr( new FSTree_Folder_Link( parent, key, type, name ) );
	}
	
	const FSTree_Premapped::Mapping sys_mac_vol_N_Mappings[] =
	{
		{ "name", &Volume_Name_Factory },
		
		{ "block-size",  &Property_Factory< GetVolumeBlockSize      > },
		{ "blocks",      &Property_Factory< GetVolumeBlockCount     > },
		{ "blocks-free", &Property_Factory< GetVolumeFreeBlockCount > },
		
		{ "capacity",  &Property_Factory< GetVolumeCapacity  > },
		{ "freespace", &Property_Factory< GetVolumeFreeSpace > },
		
		{ "sig", &Property_Factory< GetVolumeSignature > },
		
		{ "drive",  &Drive_Link_Factory,  true },
		{ "driver", &Driver_Link_Factory, true },
		
		{ "fsid", &Property_Factory< GetVolumeFSID > },
		
		{ "writes", &Property_Factory< GetVolumeWriteCount >, true },
		{ "files",  &Property_Factory< GetVolumeFileCount  >, true },
		{ "dirs",   &Property_Factory< GetVolumeDirCount   >, true },
		
		{ "parms", &Premapped_Factory< sys_mac_vol_N_parms_Mappings > },
		
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
	
	FSTreePtr New_FSTree_sys_mac_vol( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_sys_mac_vol( parent, name ) );
	}
	
	FSTreePtr Get_sys_mac_vol_N( N::FSVolumeRefNum vRefNum )
	{
		return sys_mac_vol_Details::GetChildNode( ResolveAbsolutePath( STR_LEN( "/sys/mac/vol" ) ),
		                                          VRefNum_KeyName_Traits::NameFromKey( vRefNum ),
		                                          vRefNum );
	}
	
}

