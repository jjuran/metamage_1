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
	
	class Console;
	
	typedef std::size_t ConsoleID;
	
	class ConsoleTTYHandle : public TTYHandle
	{
		private:
			ConsoleID id;
			boost::shared_ptr< Console > console;
		
		public:
			static TypeCode Type()  { return kConsoleTTYType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			ConsoleTTYHandle( ConsoleID id );
			
			~ConsoleTTYHandle();
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			void IOCtl( unsigned long request, int* argp );
			
			ConsoleID ID() const  { return id; }
			
			void SaveLeaderWaitStatus( int status );
	};
	
	
	boost::shared_ptr< IOHandle > NewConsoleDevice();
	
	ConsoleTTYHandle& GetConsoleByID( ConsoleID id );
	
	typedef std::map< ConsoleID, boost::weak_ptr< IOHandle > > ConsoleMap;
	
	const ConsoleMap& GetConsoleMap();
	
}

#endif

