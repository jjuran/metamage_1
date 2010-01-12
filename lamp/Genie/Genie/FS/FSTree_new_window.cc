/*	====================
 *	FSTree_new_window.cc
 *	====================
 */

#include "Genie/FS/FSTree_new_window.hh"

// Genie
#include "Genie/IO/Directory.hh"
#include "Genie/FS/FSTree_sys_window.hh"


namespace Genie
{
	
	class OpenWindowHandle : public DirHandle
	{
		public:
			OpenWindowHandle( const FSTreePtr& tree );
			
			~OpenWindowHandle();
	};
	
	OpenWindowHandle::OpenWindowHandle( const FSTreePtr& tree ) : DirHandle( tree )
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

