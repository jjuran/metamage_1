/*	=====================
 *	FSTree_sys_mac_vol.cc
 *	=====================
 */

#include "Genie/FileSystem/FSTree_sys_mac_vol.hh"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	std::string VRefNum_KeyName_Traits::NameFromKey( const Key& key )
	{
		return NN::Convert< std::string >( -key );
	}
	
	VRefNum_KeyName_Traits::Key VRefNum_KeyName_Traits::KeyFromName( const std::string& name )
	{
		int n = std::atoi( name.c_str() );
		
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
	
	
	FSTree_sys_mac_vol_N::FSTree_sys_mac_vol_N( const Key& key ) : itsKey( key )
	{
		FSSpec volume = N::FSMakeFSSpec( key, N::fsRtDirID, "\p" );
		
		Map( FSTreeFromFSSpec( volume ) );  // volume roots are named "mnt", not the volume name
	}
	
	
	FSTreePtr Get_sys_mac_vol_N( Nitrogen::FSVolumeRefNum vRefNum )
	{
		return FSTreePtr( new FSTree_sys_mac_vol_N( vRefNum ) );
	}
	
}

