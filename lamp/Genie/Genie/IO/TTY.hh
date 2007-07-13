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


namespace Genie
{
	
	class TTYHandle : public DeviceHandle
	{
		private:
			const std::string  ttyName;
			int                itsProcessGroupID;
		
		public:
			TTYHandle( const std::string& ttyName ) : ttyName( ttyName ),
			                                          itsProcessGroupID()
			{
			}
			
			static TypeCode Type()  { return kTTYType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			FSTreePtr GetFile() const;
			
			int GetProcessGroup() const  { return itsProcessGroupID; }
			
			void SetProcessGroup( int pgrp )  { itsProcessGroupID = pgrp; }
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			const std::string& TTYName() const  { return ttyName; }
	};
	
}

#endif

