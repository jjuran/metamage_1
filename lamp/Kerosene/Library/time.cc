/*	=======
 *	time.cc
 *	=======
 */

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// Standard C
#include "errno.h"
#include "time.h"


#pragma exceptions off


/*
	
	jan	31	31	31	31
	feb	28	29	59	60
	mar	31	31	90	91
	apr	30	30	120	121
	may	31	31	151	152
	jun	30	30	181	182
	jul	31	31	212	213
	aug	31	31	243	244
	sep	30	30	273	274
	oct	31	31	304	305
	nov	30	30	334	335
	dec	31	31	365	366
	
*/

struct tm* gmtime_r( const time_t* time_p, struct tm* result )
{
	if ( time_p == NULL  ||  result == NULL )
	{
		errno = EFAULT;
		
		return NULL;
	}
	
	if ( *time_p < 0 )
	{
		errno = EINVAL;
		
		return NULL;
	}
	
	int days_since_epoch       = *time_p / (60 * 60 * 24);
	int seconds_after_midnight = *time_p % (60 * 60 * 24);
	
	result->tm_sec  = seconds_after_midnight        % 60;
	result->tm_min  = seconds_after_midnight /   60 % 60;
	result->tm_hour = seconds_after_midnight / 3600;
	
	days_since_epoch += 2 * 365 + 1;  // epoch is now 1968
	
	// Working in quad years is valid until 2099
	const int days_per_quad = 4 * 365 + 1;
	
	int quads      = days_since_epoch / days_per_quad;
	int extra_days = days_since_epoch % days_per_quad;
	
	int extra_years = ( extra_days - 1 ) / 365;
	
	int years = quads * 4 + extra_years;
	
	years += 68;  // epoch is now 1900
	
	result->tm_year = years;
	
	if ( extra_years )
	{
		extra_days -= 1;
		extra_days %= 365;
	}
	
	result->tm_yday = extra_days;
	
	result->tm_wday = (years + years / 4 + extra_days + 1) % 7;
	
	const bool leap_year = extra_years == 0;
	
	if ( extra_days < (31 + 28 + leap_year) )
	{
		bool jan = extra_days < 31;
		
		result->tm_mday = 1 + extra_days - (jan ? 0 : 31);
		result->tm_mon  =                   jan ? 0 :  1;
	}
	else
	{
		extra_days -= 31 + 28 + leap_year;
		
		bool late = extra_days >= 153;  // after July
		
		extra_days %= 153;
		
		int month_offset = extra_days * 2 / 61;
		
		result->tm_mon = 2 + 5 * late + month_offset;
		
		result->tm_mday = 1 + extra_days - month_offset * 61 / 2 - month_offset % 2;
	}
	
	result->tm_isdst = 0;
	
	return result;
}

struct tm* gmtime( const time_t* time_p )
{
	static struct tm static_tm;
	
	return gmtime_r( time_p, &static_tm );
}

static inline
long get_dls_gmtdelta_field()
{
	MachineLocation location;
	
	ReadLocation( &location );
	
	return location.u.gmtDelta;
}

struct tm* localtime_r( const time_t* time_p, struct tm* result )
{
	if ( time_p == NULL )
	{
		return NULL;
	}
	
	const long raw_field = get_dls_gmtdelta_field();
	
	// Mask off DLS byte, and sign extend if negative
	const long delta = (raw_field & 0x00FFFFFF) | (raw_field & 0x00800000) * 0xFF << 1;
	
	time_t adjusted_time = *time_p + delta;
	
	if ( gmtime_r( &adjusted_time, result ) == NULL )
	{
		return NULL;
	}
	
	result->tm_isdst = (raw_field & 0x80000000) != 0;
	
	return result;
}

struct tm* localtime( const time_t* time_p )
{
	static struct tm static_tm;
	
	return localtime_r( time_p, &static_tm );
}
