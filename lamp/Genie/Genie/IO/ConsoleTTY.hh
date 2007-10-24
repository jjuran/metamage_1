/*	=============
 *	ConsoleTTY.hh
 *	=============
 */

#ifndef GENIE_IO_CONSOLETTY_HH
#define GENIE_IO_CONSOLETTY_HH

// Standard C++
#include <map>

// Genie
#include "Genie/IO/TTY.hh"


namespace Genie
{
	
	class ConsoleWindow;
	
	typedef TerminalID ConsoleID;
	
	class ConsoleTTYHandle : public TTYHandle
	{
		private:
			ConsoleID id;
			boost::shared_ptr< ConsoleWindow > console;
			std::string itsCurrentInput;
			int itsWindowSalvagePolicy;
			int itsLeaderWaitStatus;
		
		public:
			ConsoleTTYHandle( ConsoleID id );
			
			~ConsoleTTYHandle();
			
			IOHandle* Next() const;
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			void IOCtl( unsigned long request, int* argp );
			
			ConsoleID ID() const  { return id; }
			
			void Open();
			
			void SaveLeaderWaitStatus( int status )  { itsLeaderWaitStatus = status; }
	};
	
	
	boost::shared_ptr< IOHandle > NewConsoleDevice();
	
	TTYHandle& GetConsoleByID( ConsoleID id );
	
	typedef std::map< ConsoleID, boost::weak_ptr< IOHandle > > ConsoleMap;
	
	const ConsoleMap& GetConsoleMap();
	
}

#endif

