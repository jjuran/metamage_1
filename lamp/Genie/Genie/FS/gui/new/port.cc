/*
	gui/new/port.cc
	---------------
*/

#include "Genie/FS/gui/new/port.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/gui/port.hh"
#include "Genie/IO/VirtualDirectory.hh"


namespace Genie
{
	
	FSTree_new_port::FSTree_new_port( const FSTreePtr&     parent,
	                                  const plus::string&  name )
	:
		FSTree( parent, name, S_IFDIR | 0100 )
	{
	}
	
	class OpenWindowHandle : public VirtualDirHandle
	{
		public:
			OpenWindowHandle( const FSTreePtr& tree );
			
			~OpenWindowHandle();
	};
	
	OpenWindowHandle::OpenWindowHandle( const FSTreePtr& tree ) : VirtualDirHandle( tree )
	{
	}
	
	OpenWindowHandle::~OpenWindowHandle()
	{
		remove_port( GetFile().get() );
	}
	
	IOPtr FSTree_new_port::ChangeToDirectory() const
	{
		FSTreePtr dir = new_port();
		
		return seize_ptr( new OpenWindowHandle( dir ) );
	}
	
}

