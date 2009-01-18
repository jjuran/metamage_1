/*	==========
 *	DeskBus.cp
 *	==========
 */

#include "ClassicToolbox/DeskBus.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( ADBManager )
	
	
	static void RegisterADBManagerErrors();
	
	
#pragma force_active on
	
	class ADBManagerErrorsRegistration
	{
		public:
			ADBManagerErrorsRegistration()  { RegisterADBManagerErrors(); }
	};
	
	static ADBManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
	void ADBOp( ::Ptr refCon, ADBCompletionUPP completion, ::Ptr buffer, short commandNum )
	{
		ThrowOSStatus( ::ADBOp( refCon, completion, buffer, commandNum ) );
	}
	
	
	ADBAddress GetIndADB( ADBDataBlock& data, short index )
	{
		::ADBAddress address = ::GetIndADB( &data, index );
		
		if ( address == -1 )
		{
			throw GetIndADB_Failed();
		}
		
		return ADBAddress( address );
	}
	
	ADBDataBlock GetADBInfo( ADBAddress adbAttr )
	{
		ADBDataBlock result;
		
		ThrowOSStatus( ::GetADBInfo( &result, adbAttr ) );
		
		return result;
	}
	
	void SetADBInfo( const ADBSetInfoBlock& info, ADBAddress adbAttr )
	{
		ThrowOSStatus( ::SetADBInfo( &info, adbAttr ) );
	}
	
	void RegisterADBManagerErrors()
	{
		//RegisterOSStatus< errADBOp >();  // not defined in MacErrors.h or DeskBus.h
		RegisterOSStatus< qErr >();
	}
	
}

