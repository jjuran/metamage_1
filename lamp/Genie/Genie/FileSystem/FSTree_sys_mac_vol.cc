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
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/ResolvePathname.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
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
		
		Result operator()( const HVolumeParam& volume ) const
		{
			return volume.ioNamePtr;
		}
	};
	
	struct GetVolumeBlockCount
	{
		static const bool needsName = false;
		
		typedef UInt16 Result;
		
		Result operator()( const HVolumeParam& volume ) const
		{
			return volume.ioVNmAlBlks;
		}
	};
	
	struct GetVolumeBlockSize
	{
		static const bool needsName = false;
		
		typedef UInt32 Result;
		
		Result operator()( const HVolumeParam& volume ) const
		{
			return volume.ioVAlBlkSiz;
		}
	};
	
	struct GetVolumeFreeBlockCount
	{
		static const bool needsName = false;
		
		typedef UInt16 Result;
		
		Result operator()( const HVolumeParam& volume ) const
		{
			return volume.ioVFrBlk;
		}
	};
	
	struct GetVolumeSignature
	{
		static const bool needsName = false;
		
		typedef const char* Result;
		
		Result operator()( const HVolumeParam& volume ) const
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
		
		Result operator()( const HVolumeParam& volume ) const
		{
			return volume.ioVFSID;
		}
	};
	
	struct GetVolumeWriteCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		Result operator()( const HVolumeParam& volume ) const
		{
			return volume.ioVWrCnt;
		}
	};
	
	struct GetVolumeFileCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		Result operator()( const HVolumeParam& volume ) const
		{
			return volume.ioVFilCnt;
		}
	};
	
	struct GetVolumeDirCount
	{
		static const bool needsName = false;
		
		typedef SInt32 Result;
		
		Result operator()( const HVolumeParam& volume ) const
		{
			return volume.ioVDirCnt;
		}
	};
	
	
	static void PBHGetVInfoSync( HVolumeParam& pb, N::FSVolumeRefNum vRefNum, StringPtr name = NULL )
	{
		pb.ioNamePtr  = name;
		pb.ioVRefNum  = vRefNum;
		pb.ioVolIndex = 0;
		
		N::ThrowOSStatus( ::PBHGetVInfoSync( (HParamBlockRec*) &pb ) );
	}
	
	template < class Get >
	class sys_mac_vol_N_Query
	{
		private:
			typedef N::FSVolumeRefNum Key;
			
			Key itsKey;
		
		public:
			sys_mac_vol_N_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string operator()() const
			{
				HVolumeParam pb;
				
				Str31 name;
				
				PBHGetVInfoSync( pb, itsKey, Get::needsName ? name : NULL );
				
				std::string output = NN::Convert< std::string >( Get()( pb ) ) + "\n";
				
				return output;
			}
	};
	
	class FSTree_Virtual_Link : public FSTree
	{
		private:
			std::string itsTarget;
		
		public:
			FSTree_Virtual_Link( const FSTreePtr&    parent,
			                     const std::string&  name,
			                     const std::string&  target ) : FSTree( parent, name ),
			                                                    itsTarget( target )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return itsTarget; }
			
			FSTreePtr ResolveLink() const
			{
				return ResolvePathname( itsTarget );
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
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
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
		
		return MakeFSTree( new FSTree_Virtual_Link( parent, name, "/sys/mac/drive/" + drive ) );
	}
	
	static FSTreePtr Driver_Link_Factory( const FSTreePtr&             parent,
	                                      const std::string&           name,
	                                      VRefNum_KeyName_Traits::Key  key )
	{
		HVolumeParam pb;
		
		PBHGetVInfoSync( pb, key );
		
		std::string unit = NN::Convert< std::string >( ~pb.ioVDRefNum );
		
		return MakeFSTree( new FSTree_Virtual_Link( parent, name, "/sys/mac/unit/" + unit ) );
	}
	
	template < N::FolderType type >
	static FSTreePtr Folder_Link_Factory( const FSTreePtr&             parent,
	                                      const std::string&           name,
	                                      VRefNum_KeyName_Traits::Key  key )
	{
		return MakeFSTree( new FSTree_Folder_Link( parent, key, type, name ) );
	}
	
	const Functional_Traits< VRefNum_KeyName_Traits::Key >::Mapping sys_mac_vol_N_Mappings[] =
	{
		{ "name", &Query_Factory< GetVolumeName > },
		
		{ "used_blocks", &Query_Factory< GetVolumeBlockCount     > },
		{ "block_size",  &Query_Factory< GetVolumeBlockSize      > },
		{ "free_blocks", &Query_Factory< GetVolumeFreeBlockCount > },
		
		{ "sig", &Query_Factory< GetVolumeSignature > },
		
		{ "drive",  &Drive_Link_Factory  },
		{ "driver", &Driver_Link_Factory },
		
		{ "fsid", &Query_Factory< GetVolumeFSID > },
		
		{ "writes", &Query_Factory< GetVolumeWriteCount > },
		{ "files",  &Query_Factory< GetVolumeFileCount  > },
		{ "dirs",   &Query_Factory< GetVolumeDirCount   > },
		
		// volume roots are named "mnt", not the volume name
		{ "mnt",  &Root_Factory },
		
		{ "sys", &Folder_Link_Factory< N::kSystemFolderType    > },
		{ "tmp", &Folder_Link_Factory< N::kTemporaryFolderType > },
		
		{ NULL, NULL }
		
	};
	
}

