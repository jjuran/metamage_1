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
				FSTree( parent, name )
			{
			}
			
		#if TARGET_API_MAC_CARBON
			
			bool Exists() const
			{
				return global_rom_size != 0;
			}
			
		#endif
			
			mode_t FileTypeMode() const  { return S_IFREG; }
			
			mode_t FilePermMode() const  { return TARGET_API_MAC_CARBON ? 0 : S_IRUSR; }
			
			off_t GetEOF() const  { return global_rom_size; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	boost::shared_ptr< IOHandle > FSTree_sys_mac_rom::Open( OpenFlags flags ) const
	{
	#if TARGET_API_MAC_CARBON
		
		throw p7::errno_t( EPERM );
		
	#else
		
		return seize_ptr( new MemoryFileHandle( Self(),
		                                        flags,
		                                        LMGetROMBase(),
		                                        GetEOF() ) );
		
	#endif
	}
	
	FSTreePtr New_FSTree_sys_mac_rom( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return seize_ptr( new FSTree_sys_mac_rom( parent, name ) );
	}
	
}

