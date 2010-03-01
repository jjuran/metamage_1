/*	================
 *	FSpOpen_Async.hh
 *	================
 */

#ifndef MACIO_FSPOPENASYNC_HH
#define MACIO_FSPOPENASYNC_HH

// Nitrogen
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/Async.hh"
#include "MacIO/Routines.hh"
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	using Nitrogen::FSIOPermssn;
	using Nitrogen::FSFileRefNum;
	
	
	template < class Traits, class Callback >
	nucleus::owned< FSFileRefNum >
	//
	FSpOpen( const FSSpec&      spec,
	         FSIOPermssn        permissions,
	         Callback           callback,
	         ::IOCompletionUPP  completion = NULL )
	{
		HParamBlockRec pb;
		
		HIOParam& io = pb.ioParam;
		
		io.ioNamePtr  = const_cast< StringPtr >( spec.name );
		io.ioVRefNum  = spec.vRefNum;
		io.ioPermssn  = permissions;
		
		pb.fileParam.ioDirID = spec.parID;
		
		PBAsync< Traits, Throw_All >( pb,
		                              callback,
		                              completion );
		
		return nucleus::owned< FSFileRefNum >::seize( FSFileRefNum( io.ioRefNum ) );
	}
	
	
	template < class Callback >
	nucleus::owned< FSFileRefNum >
	//
	inline FSpOpenDF( const FSSpec&      spec,
	                  FSIOPermssn        permissions,
	                  Callback           callback,
	                  ::IOCompletionUPP  completion = NULL )
	{
		return FSpOpen< HOpenDF_Traits >( spec,
		                                  permissions,
		                                  callback,
		                                  completion );
	}
	
	template < class Callback >
	nucleus::owned< FSFileRefNum >
	//
	inline FSpOpenRF( const FSSpec&      spec,
	                  FSIOPermssn        permissions,
	                  Callback           callback,
	                  ::IOCompletionUPP  completion = NULL )
	{
		return FSpOpen< HOpenRF_Traits >( spec,
		                                  permissions,
		                                  callback,
		                                  completion );
	}
	
}

#endif

