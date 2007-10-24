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
#include "Genie/ProcessGroup.hh"


namespace Genie
{
	
	typedef std::size_t TerminalID;
	
	class TTYHandle : public DeviceHandle
	{
		private:
			const std::string                ttyName;
			boost::weak_ptr< ProcessGroup >  itsForegroundProcessGroup;
			bool                             itIsDisconnected;
		
		public:
			TTYHandle( const std::string& ttyName ) : ttyName( ttyName ),
			                                          itIsDisconnected( false )
			{
			}
			
			virtual ~TTYHandle();
			
			bool IsTerminal() const  { return true; }
			
			FSTreePtr GetFile() const;
			
			const boost::weak_ptr< ProcessGroup >& GetProcessGroup() const  { return itsForegroundProcessGroup; }
			
			void SetProcessGroup( const boost::weak_ptr< ProcessGroup >& pgrp )  { itsForegroundProcessGroup = pgrp; }
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			bool IsDisconnected() const  { return itIsDisconnected; }
			
			void Disconnect()  { itIsDisconnected = true; }
			
			const std::string& TTYName() const  { return ttyName; }
			
			virtual void SaveLeaderWaitStatus( int status )  {}
	};
	
	template <> struct IOHandle_Downcast_Traits< TTYHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsTerminal; }
	};
	
}

#endif

