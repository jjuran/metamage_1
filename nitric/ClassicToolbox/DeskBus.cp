/*	==========
 *	DeskBus.cp
 *	==========
 */

#include "ClassicToolbox/DeskBus.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace Nitrogen
{
	
	ADBManagerErrorsRegistrationDependency::ADBManagerErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterADBManagerErrors();
	
	
	class ADBManagerErrorsRegistration
	{
		public:
			ADBManagerErrorsRegistration()  { RegisterADBManagerErrors(); }
	};
	
	static ADBManagerErrorsRegistration theRegistration;
	
	
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

