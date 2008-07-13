/*	=============
 *	ConsoleTTY.hh
 *	=============
 */

#ifndef GENIE_IO_CONSOLETTY_HH
#define GENIE_IO_CONSOLETTY_HH

// Genie
#include "Genie/IO/DynamicGroup.hh"
#include "Genie/IO/TTY.hh"


namespace Genie
{
	
	typedef DynamicElementID ConsoleID;
	
	class ConsoleTTYHandle : public TTYHandle
	{
		private:
			ConsoleID                      itsID;
			boost::shared_ptr< IOHandle >  itsWindow;
			std::string                    itsCurrentInput;
			int                            itsWindowSalvagePolicy;
			
			IOHandle* Next() const;
		
		public:
			static const char* PathPrefix()  { return "/dev/con/"; }
			
			ConsoleTTYHandle( TerminalID id, const std::string& name );
			
			~ConsoleTTYHandle();
			
			FSTreePtr GetFile() const  { return itsWindow->GetFile(); }
			
			bool IsDisconnected() const;
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			void IOCtl( unsigned long request, int* argp );
			
			ConsoleID ID() const  { return itsID; }
	};
	
}

#endif

