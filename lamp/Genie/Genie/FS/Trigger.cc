/*	==========
 *	Trigger.cc
 *	==========
 */

#include "Genie/FS/Trigger.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/IO/Stream.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	mode_t Trigger_Base::FileTypeMode() const
	{
		return S_IFCHR;
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
	
	boost::shared_ptr< IOHandle > Trigger_Base::Open( OpenFlags flags ) const
	{
		return boost::shared_ptr< IOHandle >( new TriggerHandle( Self(), flags ) );
	}
	
}

