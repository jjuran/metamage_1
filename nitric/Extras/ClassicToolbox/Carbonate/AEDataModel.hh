// Carbonate/AEDataModel.hh

#ifndef CARBONUNITS_AEDATAMODEL_HH
#define CARBONUNITS_AEDATAMODEL_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

/*
	These functions are only declared if ACCESSOR_CALLS_ARE_FUNCTIONS, so we declare them here in
	the opposite case.
*/

#if !ACCESSOR_CALLS_ARE_FUNCTIONS


pascal OSErr AEGetDescData( const AEDesc*  desc,
                            void*          dataPtr,
                            Size           maximumSize );

pascal Size AEGetDescDataSize( const AEDesc* desc );

pascal OSErr AEReplaceDescData( DescType     typeCode,
                                const void*  dataPtr,
                                Size         dataSize,
                                AEDesc*      desc );


#endif

#endif

