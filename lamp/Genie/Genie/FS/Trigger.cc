/*	==========
 *	Trigger.cc
 *	==========
 */

#include "Genie/FS/Trigger.hh"

// POSIX
#include <sys/stat.h>


namespace Genie
{
	
	mode_t Trigger_Base::FileTypeMode() const
	{
		return S_IFCHR;
	}
	
}

