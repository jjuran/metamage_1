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
	
	
	struct GetVolumeSignature
	{
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
		typedef SInt16 Result;
		
		SInt16 operator()( const HVolumeParam& volume ) const
		{
			return volume.ioVFSID;
		}
	};
	
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
				HParamBlockRec paramBlock;
				
				HVolumeParam& pb = paramBlock.volumeParam;
				
				pb.ioNamePtr  = NULL;
				pb.ioVRefNum  = itsKey;
				pb.ioVolIndex = 0;
				
				N::ThrowOSStatus( ::PBHGetVInfoSync( &paramBlock ) );
				
				std::string output = NN::Convert< std::string >( Get()( pb ) ) + "\n";
				
				return output;
			}
	};
	
	class sys_mac_vol_N_name_Query
	{
		private:
			typedef N::FSVolumeRefNum Key;
			
			Key itsKey;
		
		public:
			sys_mac_vol_N_name_Query( const Key& key ) : itsKey( key )
			{
			}
			
			std::string operator()() const
			{
				FSSpec volume = N::FSMakeFSSpec( itsKey, N::fsRtDirID, "\p" );
				
				std::string output = NN::Convert< std::string >( volume.name ) + "\n";
				
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
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
	}
	
	static FSTreePtr Root_Factory( const FSTreePtr&             parent,
	                               const std::string&           name,
	                               VRefNum_KeyName_Traits::Key  key )
	{
		FSSpec volume = N::FSMakeFSSpec( key, N::fsRtDirID, "\p" );
		
		return FSTreeFromFSSpec( volume );
	}
	
	static FSTreePtr Name_Factory( const FSTreePtr&             parent,
	                               const std::string&           name,
	                               VRefNum_KeyName_Traits::Key  key )
	{
		typedef sys_mac_vol_N_name_Query Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
	}
	
	template < N::FolderType type >
	static FSTreePtr Link_Factory( const FSTreePtr&             parent,
	                               const std::string&           name,
	                               VRefNum_KeyName_Traits::Key  key )
	{
		return MakeFSTree( new FSTree_Folder_Link( parent, key, type, name ) );
	}
	
	const Functional_Traits< VRefNum_KeyName_Traits::Key >::Mapping sys_mac_vol_N_Mappings[] =
	{
		{ "sig", &Query_Factory< GetVolumeSignature > },
		
		{ "fsid", &Query_Factory< GetVolumeFSID > },
		
		// volume roots are named "mnt", not the volume name
		{ "mnt",  &Root_Factory },
		{ "name", &Name_Factory },
		
		{ "sys", &Link_Factory< N::kSystemFolderType    > },
		{ "tmp", &Link_Factory< N::kTemporaryFolderType > },
		
		{ NULL, NULL }
		
	};
	
}

