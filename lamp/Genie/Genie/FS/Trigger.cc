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
	
	Trigger_Base::Trigger_Base( const FSTreePtr&     parent,
	                            const plus::string&  name )
	:
		FSTree( parent, name )
	{
	}
	
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
		return seize_ptr( new TriggerHandle( Self(), flags ) );
	}
	
}

