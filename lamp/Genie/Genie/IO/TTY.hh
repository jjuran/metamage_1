/*	======
 *	TTY.hh
 *	======
 */

#ifndef GENIE_IO_TTY_HH
#define GENIE_IO_TTY_HH

// Standard C++
#include <string>

// Genie
#include "Genie/IO/Device.hh"
#include "Genie/IO/Terminal.hh"


namespace Genie
{
	
	typedef std::size_t TerminalID;
	
	class TTYHandle : public DeviceHandle
	{
		public:
			virtual ~TTYHandle();
	};
	
}

#endif

