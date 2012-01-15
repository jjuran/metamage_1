/*	==========
 *	Trigger.cc
 *	==========
 */

#include "Genie/FS/Trigger.hh"

// Genie
#include "Genie/IO/Stream.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	Trigger_Base::Trigger_Base( const FSTreePtr&     parent,
	                            const plus::string&  name,
	                            mode_t               mode )
	:
		FSTree( parent, name, mode )
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
	
	IOPtr Trigger_Base::Open( OpenFlags flags ) const
	{
		return new TriggerHandle( Self(), flags );
	}
	
}

