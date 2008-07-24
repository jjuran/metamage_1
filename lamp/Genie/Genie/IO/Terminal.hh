/*	===========
 *	Terminal.hh
 *	===========
 */

#ifndef GENIE_IO_TERMINAL_HH
#define GENIE_IO_TERMINAL_HH

// Standard C++
#include <string>

// POSIX
#include "errno.h"

// Genie
#include "Genie/IO/Base.hh"
#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	typedef std::size_t TerminalID;
	
	class TerminalHandle : public IOHandle
	{
		private:
			const std::string                itsTTYName;
			boost::weak_ptr< ProcessGroup >  itsForegroundProcessGroup;
			bool                             itIsDisconnected;
		
		public:
			TerminalHandle( const std::string& ttyName ) : itsTTYName         ( ttyName ),
			                                               itIsDisconnected   ( false   )
			{
			}
			
			virtual ~TerminalHandle();
			
			bool IsTerminal() const  { return true; }
			
			FSTreePtr GetFile() const;
			
			const boost::weak_ptr< ProcessGroup >& GetProcessGroup() const  { return itsForegroundProcessGroup; }
			
			void SetProcessGroup( const boost::weak_ptr< ProcessGroup >& pgrp )  { itsForegroundProcessGroup = pgrp; }
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			bool IsDisconnected() const  { return itIsDisconnected; }
			
			void Disconnect()  { itIsDisconnected = true; }
			
			const std::string& TTYName() const  { return itsTTYName; }
	};
	
	template <> struct IOHandle_Downcast_Traits< TerminalHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsTerminal; }
		
		static int GetError( IOHandle& )  { return ENOTTY; }
	};
	
}

#endif

