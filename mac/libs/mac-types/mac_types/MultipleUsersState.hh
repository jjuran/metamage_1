/*
	MultipleUsersState.hh
	---------------------
*/

#ifndef MACTYPES_MULTIPLEUSERSSTATE_HH
#define MACTYPES_MULTIPLEUSERSSTATE_HH

// mac-types
#include "mac_types/FSSpec.hh"


namespace mac   {
namespace types {
	
#pragma options align=mac68k
	
	struct MultipleUsersState
	{
		typedef unsigned char Boolean;
		typedef unsigned char Str31[ 32 ];
		
		short giVersion;
		
		short giReserved0;
		short giReserved1;
		short giReserved2;
		short giReserved3;
		
		FSSpec giReserved4;
		
		// end version 1
		
		short giDocsVRefNum;
		long  giDocsDirID;
		
		short giForceSaves;
		short giForceOpens;
		
		Str31 giWorkgroupName;
		Str31 giUserName;
		Str31 giFrontAppName;
		
		short giReserved5;
		
		short giIsOn;
		
		// end version 2
		
		short giUserLoggedInType;
		
		char giUserEncryptPwd[ 16 ];
		
		short giUserEnvironment;
		
		long giReserved6;
		long giReserved7;
		
		Boolean giDisableScrnShots;
		
		// end version 3, 4
		
		Boolean giSupportsAsyncFSCalls;
		
		short giPrefsVRefNum;
		long  giPrefsDirID;
		
		unsigned long giUserLogInTime;
		
		Boolean giUsingPrintQuotas;
		Boolean giUsingDiskQuotas;
		
		// end version 5
		
		Boolean giInSystemAccess;
		Boolean giUserFolderEnabled;
		
		short giReserved8;
		long  giReserved9;
		
		Boolean giInLoginScreen;
	};
	
#pragma options align=reset
	
}
}

#endif
