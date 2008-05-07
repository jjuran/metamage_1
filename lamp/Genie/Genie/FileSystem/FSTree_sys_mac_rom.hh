/*	=====================
 *	FSTree_sys_mac_rom.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACROM_HH
#define GENIE_FILESYSTEM_FSTREESYSMACROM_HH

// Mac OS Universal Interfaces
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	UInt32 GetROMSize();
	
	class FSTree_sys_mac_rom : public FSTree
	{
		public:
			FSTree_sys_mac_rom()  {}
			
			std::string Name() const  { return "rom"; }
			
			FSTreePtr Parent() const;
			
			mode_t FileTypeMode() const  { return S_IFREG; }
			
			mode_t FilePermMode() const  { return TARGET_API_MAC_CARBON ? 0 : S_IRUSR; }
			
			off_t GetEOF() const  { return GetROMSize(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
}

#endif

