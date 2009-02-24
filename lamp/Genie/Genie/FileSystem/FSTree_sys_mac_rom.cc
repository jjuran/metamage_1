/*	=====================
 *	FSTree_sys_mac_rom.cc
 *	=====================
 */

#include "Genie/FileSystem/FSTree_sys_mac_rom.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac.hh"
#include "Genie/IO/MemoryFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	UInt32 GetROMSize()
	{
		static UInt32 romSize = N::Gestalt( N::GestaltSelector( gestaltROMSize ) );
		
		return romSize;
	}
	
	
	boost::shared_ptr< IOHandle > FSTree_sys_mac_rom::Open( OpenFlags flags ) const
	{
	#if TARGET_API_MAC_CARBON
		
		p7::throw_errno( EPERM );
		
		// Not reached
		return boost::shared_ptr< IOHandle >();
		
	#else
		
		return boost::shared_ptr< IOHandle >( new MemoryFileHandle( Self(),
		                                                            flags,
		                                                            LMGetROMBase(),
		                                                            GetEOF() ) );
		
	#endif
	}
	
}

