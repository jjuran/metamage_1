/*	===========
 *	Terminal.hh
 *	===========
 */

#ifndef GENIE_TERMINAL_HH
#define GENIE_TERMINAL_HH

// Standard C++
#include <utility>

// Genie
#include "Genie/IORef.hh"


namespace Genie
{
	
	class CharacterDevice : public IOStream
	{
		public:
			virtual ~CharacterDevice()  {}
			virtual bool IsATTY() const  { return false; }
			virtual const char* DeviceName() const = 0;
			
			virtual unsigned int SysPoll() const  { return kPollRead | kPollWrite; }
			
			virtual int SysRead (       char* data, std::size_t byteCount );
			
			virtual int SysWrite( const char* data, std::size_t byteCount )
			{
				return byteCount;
			}
	};
	
	typedef std::pair< IORef, IORef > NewPseudoTerminal_Result;
	
	NewPseudoTerminal_Result NewPseudoTerminal();
	
	class Console;
	
	IORef NewTerminal();
	
}

#endif

