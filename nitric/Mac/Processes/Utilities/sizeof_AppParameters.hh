/*
	Mac/Processes/Utilities/sizeof_AppParameters.hh
	------------------------------------------------
*/

#ifndef MAC_PROCESSES_UTILITIES_SIZEOFAPPPARAMETERS_HH
#define MAC_PROCESSES_UTILITIES_SIZEOFAPPPARAMETERS_HH

// Standard C/C++
#include <cstddef>

// Mac OS
#ifndef __PROCESSES__
#include <Processes.h>
#endif


namespace Mac
{
	
	inline std::size_t sizeof_AppParameters( const AppParameters& appParameters )
	{
		return sizeof (AppParameters) + appParameters.messageLength;
	}
	
}

#endif

