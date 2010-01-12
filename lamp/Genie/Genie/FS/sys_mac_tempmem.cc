/*	==================
 *	sys_mac_tempmem.cc
 *	==================
 */

#include "Genie/FS/sys_mac_tempmem.hh"

// POSIX
#include <sys/stat.h>

// Nitrogen
#include "Nitrogen/MacMemory.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/IO/VirtualFile.hh"
#include "Genie/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	class TempMem_IOHandle : public VirtualFileHandle< IOHandle >
	{
		public:
			TempMem_IOHandle( const FSTreePtr&  file,
			                  OpenFlags         flags )
			:
				VirtualFileHandle< IOHandle >( file, flags )
			{
			}
			
			memory_mapping::shared_ptr Map( size_t length, off_t offset );
	};
	
	memory_mapping::shared_ptr TempMem_IOHandle::Map( size_t length, off_t offset )
	{
		if ( offset != 0 )
		{
			p7::throw_errno( EINVAL );
		}
		
		NN::Owned< N::Handle > h = N::TempNewHandle( length );
		
		return seize_ptr( new Handle_memory_mapping( h ) );
	}
	
	
	class FSTree_sys_mac_tempmem : public FSTree
	{
		public:
			FSTree_sys_mac_tempmem( const FSTreePtr&    parent,
			                        const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FileTypeMode() const  { return S_IFREG; }
			
			mode_t FilePermMode() const  { return 0; }
			
			off_t GetEOF() const  { return ::TempFreeMem(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	boost::shared_ptr< IOHandle > FSTree_sys_mac_tempmem::Open( OpenFlags flags ) const
	{
		return boost::shared_ptr< IOHandle >( new TempMem_IOHandle( Self(),
		                                                            flags ) );
		
	}
	
	FSTreePtr New_FSTree_sys_mac_tempmem( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_sys_mac_tempmem( parent, name ) );
	}
	
}

