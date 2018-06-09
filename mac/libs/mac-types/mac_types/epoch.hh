/*
	epoch.hh
	--------
*/

#ifndef MACTYPES_EPOCH_HH
#define MACTYPES_EPOCH_HH


namespace mac   {
namespace types {

static inline
unsigned epoch_delta()
{
	/*
		Returns the number of seconds between Mac and C epochs (global time).
		
		Mac time - C time = delta
		Mac time - delta = C time
		C time + delta = Mac time
	*/
	
	enum { mac_year = 1904, c_year = 1970 };
	
	const unsigned years = c_year - mac_year;  // 66
	const unsigned quads = years / 4;  // 16
	const unsigned extra = years % 4;  // 2
	
	const unsigned days_per_quad = 4 * 365 + 1;  // One Feb 29 per four years
	const unsigned extra_days = extra * 365 + 1;  // First year is a leap year
	const unsigned delta_days = days_per_quad * quads + extra_days;
	
	const unsigned seconds_per_day = 60 * 60 * 24;
	const unsigned delta_seconds = seconds_per_day * delta_days;
	
	return delta_seconds;  // 2082844800
}

}
}

#endif
