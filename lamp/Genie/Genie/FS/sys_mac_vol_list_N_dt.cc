/*
	sys_mac_vol_list_N_dt.cc
	------------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/sys_mac_vol_list_N_dt.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
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
	
	
	const FSTree_Premapped::Mapping sys_mac_vol_list_N_dt_Mappings[] =
	{
		{ "dir",  &Basic_Factory< FSTree_Desktop_Dir_Link > },
		
		{ NULL, NULL }
		
	};
	
}

