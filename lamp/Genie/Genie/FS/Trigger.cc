/*	==========
 *	Trigger.cc
 *	==========
 */

#include "Genie/FS/Trigger.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/touch.hh"

// Genie
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	static void trigger_touch( const FSTree* node )
	{
		if ( trigger_function f = *(trigger_function*) node->extra() )
		{
			f( node );
			
			return;
		}
	}
	
	static IOPtr trigger_open( const FSTree* node, int flags, mode_t mode );
	
	static const data_method_set trigger_data_methods =
	{
		&trigger_open
	};
	
	static const node_method_set trigger_methods =
	{
		NULL,
		NULL,
		&trigger_touch,
		NULL,
		NULL,
		NULL,
		&trigger_data_methods
	};
	
	class TriggerHandle : public VirtualFileHandle< StreamHandle >
	{
		public:
			TriggerHandle( const FSTreePtr&  file,
			               int               flags )
			:
				VirtualFileHandle< StreamHandle >( file, flags )
			{
			}
			
			unsigned int SysPoll()  { return kPollRead | kPollWrite; }
			
			ssize_t Write( const char* buffer, size_t n );
	};
	
	ssize_t TriggerHandle::Write( const char* buffer, size_t n )
	{
		touch( GetFile().get() );
		
		return n;
	}
	
	static IOPtr trigger_open( const FSTree* node, int flags, mode_t mode )
	{
		return new TriggerHandle( node, flags );
	}
	
	
	FSTreePtr trigger_factory( const FSTree*        parent,
	                           const plus::string&  name,
	                           const void*          args )
	{
		FSTree* result = new FSTree( parent,
		                             name,
		                             S_IFCHR | 0200,
		                             &trigger_methods,
		                             sizeof (trigger_extra) );
		
		trigger_extra& extra = *(trigger_extra*) result->extra();
		
		extra = *(trigger_extra*) args;
		
		return result;
	}
	
}

