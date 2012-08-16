/*
	Mac/Toolbox/Types/OSStatus.hh
	-----------------------------
*/

#ifndef MAC_TOOLBOX_TYPES_OSSTATUS_HH
#define MAC_TOOLBOX_TYPES_OSSTATUS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace Mac
{
	
	class OSStatus
	{
		private:
			::OSStatus status;
			
			// Not implemented:
			OSStatus(          bool      );
			OSStatus(          char      );
			OSStatus(   signed char      );
			OSStatus(   signed long long );
			OSStatus( unsigned char      );
			OSStatus( unsigned short     );
			OSStatus( unsigned int       );
			OSStatus( unsigned long      );
			OSStatus( unsigned long long );
		
		public:
			typedef ::OSStatus error_number;
			
			OSStatus()                                            : status( noErr )    {}
			OSStatus( ::OSStatus s )                              : status( s )        {}
			
			OSStatus( ::OSErr s )                                 : status( s )        {}
			OSStatus( signed int s )                              : status( s )        {}
			
			static OSStatus Make( ::OSStatus s )                  { return OSStatus( s ); }
			::OSStatus Get() const                                { return status; }
			operator ::OSStatus() const                           { return status; }
	};
	
}

#endif

