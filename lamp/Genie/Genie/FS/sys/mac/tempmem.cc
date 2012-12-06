/*
	Genie/FS/sys/mac/tempmem.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/tempmem.hh"

// POSIX
#include <sys/stat.h>

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/VirtualFile.hh"
#include "Genie/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	class TempMem_IOHandle : public VirtualFileHandle< IOHandle >
	{
		public:
			TempMem_IOHandle( const FSTreePtr&  file,
			                  int               flags )
			:
				VirtualFileHandle< IOHandle >( file, flags )
			{
			}
			
			memory_mapping_ptr Map( size_t length, int prot, int flags, off_t offset );
	};
	
	memory_mapping_ptr
	//
	TempMem_IOHandle::Map( size_t length, int prot, int flags, off_t offset )
	{
		if ( offset != 0 )
		{
			p7::throw_errno( EINVAL );
		}
		
		n::owned< N::Handle > h = N::TempNewHandle( length );
		
		return new Handle_memory_mapping( h, length, flags );
	}
	
	
	static off_t mac_tempmem_geteof( const FSTree* node )
	{
		return ::TempFreeMem();
	}
	
	static IOPtr mac_tempmem_open( const FSTree* node, int flags, mode_t mode )
	{
		return new TempMem_IOHandle( node, flags );
		
	}
	
	static const data_method_set mac_tempmem_data_methods =
	{
		&mac_tempmem_open,
		&mac_tempmem_geteof
	};
	
	static const node_method_set mac_tempmem_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&mac_tempmem_data_methods
	};
	
	FSTreePtr New_FSTree_sys_mac_tempmem( const FSTree*        parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return new FSTree( parent, name, S_IFREG | 0, &mac_tempmem_methods );
	}
	
}

