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
	
	class CharacterDevice
	{
		public:
			virtual ~CharacterDevice()  {}
			virtual bool IsATTY() const  { return false; }
			virtual const char* DeviceName() const = 0;
			
			virtual int Read (       char* data, std::size_t byteCount );
			
			virtual int Write( const char* data, std::size_t byteCount )
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

