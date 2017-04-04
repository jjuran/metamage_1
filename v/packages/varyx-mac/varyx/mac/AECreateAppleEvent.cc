/*
	AECreateAppleEvent.cc
	---------------------
*/

#include "varyx/mac/AECreateAppleEvent.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#ifndef __AEDATAMODEL__
#include <ApplicationServices/ApplicationServices.h>
#endif
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// vlib
#include "vlib/proc_info.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/mb32.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AEDesc.hh"


namespace varyx
{
namespace mac
{

static
Value v_AECreateAppleEvent( const Value& v )
{
	list_iterator args( v );
	
	const Value& arg1 = args.use();
	const Value& arg2 = args.use();
	const Value& arg3 = args.use();
	const Value& arg4 = args.use();
	const Value& arg5 = args.get();
	
	const ::AEEventClass evClass = arg1.to< MB32 >().get();
	const ::AEEventClass evID    = arg2.to< MB32 >().get();
	
	const AEDesc& addressDesc = static_cast< const AEDesc& >( arg3 );
	
	const ::AEAddressDesc address = addressDesc.get();
	
	const ::AEReturnID      rID = arg4.number().clipped();
	const ::AETransactionID xID = arg5.number().clipped();
	
	AEDesc result;
	
	OSErr err;
	
	::AppleEvent* event = result.pointer();
	
	err = ::AECreateAppleEvent( evClass, evID, &address, rID, xID, event );
	
	throw_MacOS_error( err, "AECreateAppleEvent" );
	
	return result;
}

static inline
Value int_with_default( const type_info& typeinfo, SInt32 x )
{
	return Value( Type( typeinfo ), Op_duplicate, Integer( x ) );
}

#define AEEventClass     Type( mb32_vtype )
#define AEEventID        Type( mb32_vtype )
#define AEAddressDesc    Type( AEDesc_vtype )
#define AEReturnID       int_with_default( i16_vtype, ::kAutoGenerateReturnID )
#define AETransactionID  int_with_default( i32_vtype, ::kAnyTransactionID     )

static const Value
AECreateAppleEvent = Value( AEEventClass,
                     Value( AEEventID,
                     Value( AEAddressDesc,
                     Value( AEReturnID,
                            AETransactionID ) ) ) );

const proc_info proc_AECreateAppleEvent =
{
	"AECreateAppleEvent",
	&v_AECreateAppleEvent,
	&AECreateAppleEvent,
};

}
}
