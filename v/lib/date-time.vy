module date-time

export
let days =
[
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
]

export
let months =
[
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
]

let month_days =
[
	31, 28, 31,
	30, 31, 30,
	31, 31, 30,
	31, 30, 31,
]

let utc = "UTC"

export
def time_pieces (secs)
{
	let q = 365 * 4 + 1
	
	# 365 * n           is Jan 1
	# 365 * n + 31      is Feb 1
	# 365 * n + 31 + 28 is Feb 29
	
	let feb29 = 365 * 2 + 31 + 28
	
	var mins = secs div 60
	    secs = secs mod 60
	
	var hours = mins div 60
	    mins  = mins mod 60
	
	var days  = hours div 24
	    hours = hours mod 24
	
	let wday = (days + 4) mod 7
	
	var quads = days div q
	    days  = days mod q
	
	var years = quads * 4
	
	let leap_diff = days - feb29
	
	days -= +(leap_diff > 0)
	
	years += days div 365
	days   = days mod 365
	
	var month = 1
	
	for m in month_days do
	{
		if days < m then {break}
		
		days -= m
		++month
	}
	
	days += 1 + +(leap_diff == 0)
	
	return 1970 + years, month, days, hours, mins, secs, wday
}

def tens (x)
{
	return ("0" * (x < 10)) x
}

export
def gmtime (secs)
{
	let (y, m, d, H, M, S, w) = time_pieces secs
	
	let fields =
	(
		sec:  S,
		min:  M,
		hour: H,
		mday: d,
		mon:  m - 1,
		year: y,
		wday: w,
		zone: utc,
	)
	
	return record fields
}

export
def stamp (secs)
{
	let (y, m, d, H, M, S), ... = time_pieces secs
	
	let date = "-".join (     y, tens m, tens d)
	let time = ":".join (tens H, tens M, tens S)
	let zone = utc

	return date " " time " " zone
}
