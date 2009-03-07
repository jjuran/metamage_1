/*	=========================
 *	FSTree_sys_mac_gestalt.cc
 *	=========================
 */

#include "Genie/FileSystem/FSTree_sys_mac_gestalt.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/OSStatus.h"

// BitsAndBytes
#include "HexCodes.hh"
#include "HexStrings.hh"


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
	namespace Bits = BitsAndBytes;
	
	
	std::string Gestalt_KeyName_Traits::NameFromKey( const Key& key )
	{
		std::string result = OSType_KeyName_Traits::NameFromKey( key );
		
		if ( !sys_mac_gestalt_Details::KeyIsValid( key ) )
		{
			result = "." + result;
		}
		
		return result;
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
		return FSTreePtr( new FSTree_Premapped( parent, name ) );
	}
	
}

