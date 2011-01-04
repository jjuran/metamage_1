/*
	Mac/AppleEvents/Types/AECoercionHandler.cc
	------------------------------------------
*/

#include "Mac/AppleEvents/Types/AECoercionHandler.hh"


namespace Mac
{
	
	AECoercionHandler::AECoercionHandler() : fromType(),
	                                         toType(),
	                                         handler(),
	                                         handlerRefCon(),
	                                         fromTypeIsDesc(),
	                                         isSysHandler()
	{}
	
	bool operator==( const AECoercionHandler& a, const AECoercionHandler& b )
	{
		return a.handler        == b.handler
		    && a.handlerRefCon  == b.handlerRefCon
		    && a.fromType       == b.fromType
		    && a.toType         == b.toType
		    && a.fromTypeIsDesc == b.fromTypeIsDesc
		    && a.isSysHandler   == b.isSysHandler;
	}
	
	nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall )
	{
		ThrowOSStatus( ::AEInstallCoercionHandler( toInstall.fromType, 
		                                           toInstall.toType, 
		                                           toInstall.handler, 
		                                           toInstall.handlerRefCon, 
		                                           toInstall.fromTypeIsDesc,
		                                           toInstall.isSysHandler ) );
		
		return nucleus::owned< AECoercionHandler >::seize( toInstall );
	}
	
	AECoercionHandler AEGetCoercionHandler( DescType  fromType,
	                                        DescType  toType,
	                                        bool      isSysHandler )
	{
		::AECoercionHandlerUPP  handler;
		long                    handlerRefCon;
		::Boolean               fromTypeIsDesc;
		
		ThrowOSStatus( ::AEGetCoercionHandler( fromType,
		                                       toType,
		                                       &handler,
		                                       &handlerRefCon,
		                                       &fromTypeIsDesc,
		                                       isSysHandler ) );
		
		return AECoercionHandler( fromType,
		                          toType,
		                          handler,
		                          handlerRefCon,
		                          fromTypeIsDesc,
		                          isSysHandler );
	}
	
}

