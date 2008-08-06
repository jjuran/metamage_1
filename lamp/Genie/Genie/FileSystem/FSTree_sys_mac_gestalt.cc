/*	=========================
 *	FSTree_sys_mac_gestalt.cc
 *	=========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_gestalt.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	GestaltTable_Container GestaltTable()
	{
		UInt32 tabl = Nitrogen::Gestalt( Nitrogen::GestaltSelector( 'tabl' ) );
		
		Gestalt_Handle h = reinterpret_cast< Gestalt_Handle >( tabl );
		
		if ( h == NULL  ||  *h == NULL )
		{
			ThrowOSStatus( nilHandleErr );
		}
		
		return GestaltTable_Container( h );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	std::string OSType_KeyName_Traits::NameFromKey( const Key& key )
	{
		return Nucleus::Convert< std::string >( key );
	}
	
	OSType_KeyName_Traits::Key OSType_KeyName_Traits::KeyFromName( const std::string& name )
	{
		return Nucleus::Convert< Nitrogen::OSType >( name );
	}
	
	
	bool sys_mac_gestalt_Details::KeyIsValid( const Key& key )
	{
		try
		{
			N::Gestalt( N::GestaltSelector( key ) );
		}
		catch ( ... )
		{
			return false;
		}
		
		return true;
	}
	
	FSTreePtr sys_mac_gestalt_Details::GetChildNode( const FSTreePtr&    parent,
		                                             const std::string&  name,
		                                             const Key&          key )
	{
		return MakeFSTree( new FSTree_Functional< Gestalt_KeyName_Traits::Key >( parent, name, key ) );
	}
	
}

