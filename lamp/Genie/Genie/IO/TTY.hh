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
	
	class TTYHandle : public DeviceHandle
	{
		private:
			const std::string                ttyName;
			boost::weak_ptr< ProcessGroup >  itsForegroundProcessGroup;
		
		public:
			TTYHandle( const std::string& ttyName ) : ttyName( ttyName )
			{
			}
			
			static TypeCode Type()  { return kTTYType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			FSTreePtr GetFile() const;
			
			const boost::weak_ptr< ProcessGroup >& GetProcessGroup() const  { return itsForegroundProcessGroup; }
			
			void SetProcessGroup( const boost::weak_ptr< ProcessGroup >& pgrp )  { itsForegroundProcessGroup = pgrp; }
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			const std::string& TTYName() const  { return ttyName; }
			
			virtual void SaveLeaderWaitStatus( int status )  {}
	};
	
}

#endif

