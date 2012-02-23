/*	==========
 *	Trigger.cc
 *	==========
 */

#include "Genie/FS/Trigger.hh"

// Genie
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	static void trigger_touch( const FSTree* node )
	{
		const Trigger_Base* file = static_cast< const Trigger_Base* >( node );
		
		file->Invoke();
	}
	
	static IOPtr trigger_open( const FSTree* node, int flags, mode_t mode )
	{
		const Trigger_Base* file = static_cast< const Trigger_Base* >( node );
		
		return file->Open( flags, mode );
	}
	
	static const data_method_set trigger_data_methods =
	{
		&trigger_open
	};
	
	static node_method_set trigger_methods =
	{
		NULL,
		NULL,
		&trigger_touch,
		NULL,
		NULL,
		NULL,
		&trigger_data_methods
	};
	
	Trigger_Base::Trigger_Base( const FSTreePtr&     parent,
	                            const plus::string&  name,
	                            mode_t               mode )
	:
		FSTree( parent, name, mode, &trigger_methods )
	{
	}
	
	class TriggerHandle : public VirtualFileHandle< StreamHandle >
	{
		public:
			TriggerHandle( const FSTreePtr&  file,
			               OpenFlags         flags )
			:
				VirtualFileHandle< StreamHandle >( file, flags )
			{
			}
			
			unsigned int SysPoll()  { return kPollRead | kPollWrite; }
			
			ssize_t Write( const char* buffer, size_t n );
	};
	
	ssize_t TriggerHandle::Write( const char* buffer, size_t n )
	{
		GetFile()->SetTimes();
		
		return n;
	}
	
	IOPtr Trigger_Base::Open( OpenFlags flags, mode_t mode ) const
	{
		return new TriggerHandle( Self(), flags );
	}
	
}

