/*
	Genie/FS/sys/mac/rom.cc
	-----------------------
*/

#include "Genie/FS/sys/mac/rom.hh"

// POSIX
#include "sys/stat.h"

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/IO/MemoryFile.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static UInt32 get_ROM_size()
	{
		SInt32 rom_size;
		
		const OSErr err = ::Gestalt( gestaltROMSize, &rom_size );
		
		return err == noErr ? rom_size : 0;
	}
	
	
	static const UInt32 global_rom_size = get_ROM_size();
	
	
	class FSTree_sys_mac_rom : public FSTree
	{
		public:
			FSTree_sys_mac_rom( const FSTreePtr&     parent,
			                    const plus::string&  name )
			:
				FSTree( parent,
				        name,
				        S_IFREG * (!TARGET_API_MAC_CARBON  ||  global_rom_size)
				      | S_IRUSR *  !TARGET_API_MAC_CARBON )
			{
			}
			
			off_t GetEOF() const  { return global_rom_size; }
			
			IOPtr Open( OpenFlags flags, mode_t mode ) const;
	};
	
	
	IOPtr FSTree_sys_mac_rom::Open( OpenFlags flags, mode_t mode ) const
	{
	#if TARGET_API_MAC_CARBON
		
		throw p7::errno_t( EPERM );
		
	#else
		
		return new MemoryFileHandle( Self(),
		                             flags,
		                             LMGetROMBase(),
		                             GetEOF() );
		
	#endif
	}
	
	FSTreePtr New_FSTree_sys_mac_rom( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return new FSTree_sys_mac_rom( parent, name );
	}
	
}

