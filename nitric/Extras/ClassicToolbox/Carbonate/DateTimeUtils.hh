/*
	Carbonate/DateTimeUtils.hh
	--------------------------
	
	Joshua Juran
*/

#ifndef CARBONATE_DATETIMEUTILS_HH
#define CARBONATE_DATETIMEUTILS_HH

#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif

#undef DateString
#undef TimeString
#undef LongDateString
#undef LongTimeString

#endif

