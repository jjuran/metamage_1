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
#include "Genie/FileSystem/FSTree_sys_mac.hh"


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
	
	
	FSTreePtr sys_mac_vol_Details::Parent() const
	{
		return GetSingleton< FSTree_sys_mac >();
	}
	
	FSTreePtr sys_mac_vol_Details::GetChildNode( const Key& key )
	{
		return MakeFSTree( new FSTree_sys_mac_vol_N( key ) );
	}
	
	
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
			const char*    itsName;
		
		public:
			FSTree_Folder_Link( const Key&     key,
			                    N::FolderType  type,
			                    const char*    name ) : itsKey ( key  ),
			                                            itsType( type ),
			                                            itsName( name )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return itsName; }
			
			FSTreePtr Parent() const  { return MakeFSTree( new FSTree_sys_mac_vol_N( itsKey ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const
			{
				return FSTreeFromFSSpec( NN::Convert< FSSpec >( N::FindFolder( itsKey, itsType, false ) ) );
			}
	};
	
	
	static FSTreePtr Name_Factory( const FSTreePtr&             parent,
	                               const std::string&           name,
	                               VRefNum_KeyName_Traits::Key  key )
	{
		typedef sys_mac_vol_N_name_Query Query;
		
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
	}
	
	void FSTree_sys_mac_vol_N::Init()
	{
		FSSpec volume = N::FSMakeFSSpec( itsKey, N::fsRtDirID, "\p" );
		
		Map( FSTreeFromFSSpec( volume ) );  // volume roots are named "mnt", not the volume name
		
		Map( Name_Factory( shared_from_this(), "name", itsKey ) );
		
		Map( FSTreePtr( new FSTree_Folder_Link( itsKey, N::kSystemFolderType,    "sys" ) ) );
		Map( FSTreePtr( new FSTree_Folder_Link( itsKey, N::kTemporaryFolderType, "tmp" ) ) );
	}
	
	
	FSTreePtr Get_sys_mac_vol_N( Nitrogen::FSVolumeRefNum vRefNum )
	{
		return MakeFSTree( new FSTree_sys_mac_vol_N( vRefNum ) );
	}
	
}

