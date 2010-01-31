/*
	sys_mac_vol_list_N_dt.cc
	------------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/sys_mac_vol_list_N_dt.hh"

// iota
#include "iota/decimal.hh"
#include "iota/quad.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/ResolvableSymLink.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Nitrogen
{
	
	static FSSpec DTGetAPPL( FSVolumeRefNum  vRefNum,
	                         OSType          signature,
	                         short           index = 0 )
	{
		DTPBRec pb;
		
		PBDTGetPath( vRefNum, pb );
		
		FSSpec result;
		
		pb.ioNamePtr     = result.name;
		pb.ioIndex       = index;
		pb.ioFileCreator = signature;
		
		PBDTGetAPPLSync( pb );
		
		result.vRefNum = vRefNum;
		result.parID   = pb.ioAPPLParID;
		
		return result;
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static N::FSVolumeRefNum GetKeyFromParent( const FSTreePtr& parent )
	{
		const FSTreePtr& grandparent = parent->ParentRef();
		
		return N::FSVolumeRefNum( -iota::parse_unsigned_decimal( grandparent->Name().c_str() ) );
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
	
	
	class dt_appls_QUAD_latest : public FSTree_ResolvableSymLink
	{
		public:
			dt_appls_QUAD_latest( const FSTreePtr&    parent,
			                      const std::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const;
	};
	
	class dt_appls_QUAD_list_N : public FSTree_ResolvableSymLink
	{
		public:
			dt_appls_QUAD_list_N( const FSTreePtr&    parent,
			                      const std::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const;
	};
	
	
	FSTreePtr dt_appls_QUAD_latest::ResolveLink() const
	{
		const FSTreePtr& parent = ParentRef();
		
		const ::OSType creator = iota::decode_quad( parent->Name().c_str() );
		
		const FSTreePtr& great_x2_grandparent = parent->ParentRef()->ParentRef()->ParentRef();
		
		const N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -iota::parse_unsigned_decimal( great_x2_grandparent->Name().c_str() ) );
		
		const FSSpec file = N::DTGetAPPL( vRefNum, N::OSType( creator ) );
		
		const bool onServer = VolumeIsOnServer( vRefNum );
		
		return FSTreeFromFSSpec( file, onServer );
	}
	
	FSTreePtr dt_appls_QUAD_list_N::ResolveLink() const
	{
		const short index = iota::parse_unsigned_decimal( Name().c_str() );
		
		const FSTreePtr& grandparent = ParentRef()->ParentRef();
		
		const ::OSType creator = iota::decode_quad( grandparent->Name().c_str() );
		
		const FSTreePtr& great_x3_grandparent = grandparent->ParentRef()->ParentRef()->ParentRef();
		
		const N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -iota::parse_unsigned_decimal( great_x3_grandparent->Name().c_str() ) );
		
		const FSSpec file = N::DTGetAPPL( vRefNum, N::OSType( creator ), index );
		
		const bool onServer = VolumeIsOnServer( vRefNum );
		
		return FSTreeFromFSSpec( file, onServer );
	}
	
	
	static FSTreePtr appl_QUAD_list_lookup( const FSTreePtr& parent, const std::string& name )
	{
		if ( !canonical_positive_integer::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return seize_ptr( new dt_appls_QUAD_list_N( parent, name ) );
	}
	
	static void appl_QUAD_list_iterate( FSTreeCache& cache )
	{
		// Can't enumerate
	}
	
	static FSTreePtr new_sys_mac_vol_list_N_dt_appls_QUAD_list( const FSTreePtr& parent, const std::string& name )
	{
		return new_basic_directory( parent, name, appl_QUAD_list_lookup, appl_QUAD_list_iterate );
	}
	
	extern const FSTree_Premapped::Mapping sys_mac_vol_list_N_dt_appls_QUAD_Mappings[];
	
	const FSTree_Premapped::Mapping sys_mac_vol_list_N_dt_appls_QUAD_Mappings[] =
	{
		{ "latest",  &Basic_Factory< dt_appls_QUAD_latest > },
		
		{ "list",  &new_sys_mac_vol_list_N_dt_appls_QUAD_list },
		
		{ NULL, NULL }
		
	};
	
	
	static FSTreePtr appl_lookup( const FSTreePtr& parent, const std::string& name )
	{
		if ( name.length() != sizeof 'quad' )
		{
			p7::throw_errno( ENOENT );
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

