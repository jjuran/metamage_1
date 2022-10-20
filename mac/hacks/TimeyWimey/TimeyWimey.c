/*
	TimeyWimey.c
	------------
	
	TimeyWimey INIT for Mac OS
	
	Copyright 2022, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This program does exactly what it appears to do:  If the clock is set
	to any year before 1984, it updates the clock to 1995-01-22, 12:34pm.
	
	(This behavior was specified by a client; the work remains my own.)
	
*/

// Mac OS
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

enum
{
	minutes = 60,
	hours   = 60 * minutes,
	days    = 24 * hours,
	
	k1904_01_01 = 0,
	k1984_01_01 = k1904_01_01 + (80 * 365 + 80/4) * days,
	k1995_01_01 = k1984_01_01 + (11 * 365 +    3) * days,
	k1995_22_01 = k1995_01_01 +        21         * days,
	
	k1995_22_01_12_34pm = k1995_22_01 + 12 * hours + 34 * minutes,
};

UInt32 Time : 0x020c;

int main()
{
	if ( Time < k1984_01_01 )
	{
		SetDateTime( k1995_22_01_12_34pm );
	}
	
	return 0;
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
