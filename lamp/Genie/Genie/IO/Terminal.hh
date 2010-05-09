/*	===========
 *	Terminal.hh
 *	===========
 */

#ifndef GENIE_IO_TERMINAL_HH
#define GENIE_IO_TERMINAL_HH

// POSIX
#include "errno.h"
#include "fcntl.h"

// Genie
#include "Genie/IO/Base.hh"
#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	typedef std::size_t TerminalID;
	
	class TerminalHandle : public IOHandle
	{
		private:
			const plus::string               itsTTYName;
			boost::shared_ptr< IOHandle >    itsTTY;
			boost::weak_ptr< ProcessGroup >  itsForegroundProcessGroup;
			bool                             itIsDisconnected;
			
			IOHandle* Next() const  { return itsTTY.get(); }
		
		public:
			TerminalHandle( const plus::string& ttyName )
			:
				IOHandle( O_RDWR ),
				itsTTYName      ( ttyName ),
				itIsDisconnected( false   )
			{
			}
			
			~TerminalHandle();
			
			bool IsTerminal() const  { return true; }
			
			void Attach( const boost::shared_ptr< IOHandle >& target )  { itsTTY = target; }
			
			FSTreePtr GetFile();
			
			const boost::weak_ptr< ProcessGroup >& GetProcessGroup() const  { return itsForegroundProcessGroup; }
			
			void SetProcessGroup( const boost::weak_ptr< ProcessGroup >& pgrp )  { itsForegroundProcessGroup = pgrp; }
			
			void IOCtl( unsigned long request, int* argp );
			
			bool IsDisconnected() const  { return itIsDisconnected; }
			
			void Disconnect();
			
			const plus::string& TTYName() const  { return itsTTYName; }
	};
	
	template <> struct IOHandle_Downcast_Traits< TerminalHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsTerminal; }
		
		static int GetError( IOHandle& )  { return ENOTTY; }
	};
	
}

#endif

