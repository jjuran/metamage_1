/*
	Genie/FS/new/port.cc
	--------------------
*/

#include "Genie/FS/new/port.hh"

// Genie
#include "Genie/FS/sys/port.hh"
#include "Genie/IO/VirtualDirectory.hh"


namespace Genie
{
	
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
		RemoveWindow( GetFile().get() );
	}
	
	boost::shared_ptr< IOHandle > FSTree_new_window::ChangeToDirectory() const
	{
		FSTreePtr dir = NewWindow();
		
		return seize_ptr( new OpenWindowHandle( dir ) );
	}
	
}

