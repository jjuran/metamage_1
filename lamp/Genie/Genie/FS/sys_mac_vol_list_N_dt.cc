/*
	sys_mac_vol_list_N_dt.cc
	------------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/sys_mac_vol_list_N_dt.hh"

// iota
#include "iota/quad.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/ResolvableSymLink.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		const FSTreePtr& grandparent = parent->ParentRef();
		
		return N::FSVolumeRefNum( -std::atoi( grandparent->Name().c_str() ) );
	}
	
	
	static N::FSDirSpec DTGetInfo_Dir( N::FSVolumeRefNum vRefNum )
	{
		DTPBRec pb;
		
		N::PBDTGetPath( vRefNum, pb );
		
		N::ThrowOSStatus( ::PBDTGetInfoSync( &pb ) );
		
		const N::FSVolumeRefNum new_vRefNum = N::FSVolumeRefNum( pb.ioVRefNum );
		const N::FSDirID        new_dirID   = N::FSDirID       ( pb.ioDirID   );
		
		return NN::Make< N::FSDirSpec >( new_vRefNum, new_dirID );
	}
	
	class FSTree_Desktop_Dir_Link : public FSTree_ResolvableSymLink
	{
		private:
			N::FSVolumeRefNum itsVRefNum;
		
		public:
			FSTree_Desktop_Dir_Link( const FSTreePtr&    parent,
			                         const std::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name ),
				itsVRefNum( GetKeyFromParent( parent ) )
			{
			}
			
			FSTreePtr ResolveLink() const
			{
				const N::FSDirSpec dir = DTGetInfo_Dir( itsVRefNum );
				
				const bool onServer = VolumeIsOnServer( dir.vRefNum );
				
				return FSTreeFromFSDirSpec( dir, onServer );
			}
	};
	
	
	static FSSpec DTGetAPPL( N::FSVolumeRefNum vRefNum, ::OSType creator )
	{
		DTPBRec pb;
		
		N::PBDTGetPath( vRefNum, pb );
		
		FSSpec result;
		
		pb.ioNamePtr     = result.name;
		pb.ioIndex       = 0;
		pb.ioFileCreator = creator;
		
		N::ThrowOSStatus( ::PBDTGetAPPLSync( &pb ) );
		
		result.vRefNum = vRefNum;
		result.parID   = pb.ioAPPLParID;
		
		return result;
	}
	
	class dt_appls_QUAD_latest : public FSTree_ResolvableSymLink
	{
		public:
			dt_appls_QUAD_latest( const FSTreePtr&    parent,
			                      const std::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const
			{
				const FSTreePtr& parent = ParentRef();
				
				const ::OSType creator = iota::decode_quad( parent->Name().c_str() );
				
				const FSTreePtr& great_x2_grandparent = parent->ParentRef()->ParentRef()->ParentRef();
				
				const N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -atoi( great_x2_grandparent->Name().c_str() ) );
				
				const FSSpec file = DTGetAPPL( vRefNum, creator );
				
				const bool onServer = VolumeIsOnServer( vRefNum );
				
				return FSTreeFromFSSpec( file, onServer );
			}
	};
	
	
	extern const FSTree_Premapped::Mapping sys_mac_vol_list_N_dt_appls_QUAD_Mappings[];
	
	const FSTree_Premapped::Mapping sys_mac_vol_list_N_dt_appls_QUAD_Mappings[] =
	{
		{ "latest",  &Basic_Factory< dt_appls_QUAD_latest > },
		
		{ NULL, NULL }
		
	};
	
	static FSTreePtr appl_lookup( const FSTreePtr& parent, const std::string& name )
	{
		if ( name.length() != sizeof 'quad' )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return Premapped_Factory< sys_mac_vol_list_N_dt_appls_QUAD_Mappings >( parent, name );
	}
	
	static void appl_iterate( FSTreeCache& cache )
	{
		// Can't enumerate
	}
	
	static FSTreePtr new_sys_mac_vol_list_N_dt_appls( const FSTreePtr& parent, const std::string& name )
	{
		return new_basic_directory( parent, name, appl_lookup, appl_iterate );
	}
	
	
	const FSTree_Premapped::Mapping sys_mac_vol_list_N_dt_Mappings[] =
	{
		{ "dir",  &Basic_Factory< FSTree_Desktop_Dir_Link > },
		
		{ "appls",  &new_sys_mac_vol_list_N_dt_appls },
		
		{ NULL, NULL }
		
	};
	
}

