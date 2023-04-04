/*
	IntlUtils.cc
	------------
*/

#include "IntlUtils.hh"

// Mac OS
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"


#pragma exceptions off


#define PSTR_LEN( s ) "\p" s, sizeof s


static ConstStr255Param day_names[] =
{
	"\p" "",
	"\p" "Sunday",
	"\p" "Monday",
	"\p" "Tuesday",
	"\p" "Wednesday",
	"\p" "Thursday",
	"\p" "Friday",
	"\p" "Saturday",
};

static ConstStr255Param month_names[] =
{
	"\p" "",
	"\p" "January",
	"\p" "February",
	"\p" "March",
	"\p" "April",
	"\p" "May",
	"\p" "June",
	"\p" "July",
	"\p" "August",
	"\p" "September",
	"\p" "October",
	"\p" "November",
	"\p" "December",
};

static inline
unsigned char* fast_pstrcpy( unsigned char* p, const unsigned char* s )
{
	return (unsigned char*) fast_mempcpy( p, s + 1, s[ 0 ] );
}

static
pascal void IUDateString_call( long dateTime, DateForm form, Str255 result )
{
	DateTimeRec rec;
	
	SecondsToDate( dateTime, &rec );
	
	int year = rec.year;
	
	unsigned char* p = result;
	
	switch ( form )
	{
		case shortDate:
			*p++ = 10;
			*p++ = year / 1000 + '0';  year %= 1000;
			*p++ = year / 100  + '0';  year %= 100;
			*p++ = year / 10   + '0';  year %= 10;
			*p++ = year        + '0';
			*p++ = '-';
			*p++ = rec.month / 10  + '0';
			*p++ = rec.month % 10  + '0';
			*p++ = '-';
			*p++ = rec.day / 10    + '0';
			*p++ = rec.day % 10    + '0';
			break;
		
		case longDate:
			++p;  // skip length byte
			
			p = fast_pstrcpy( p, day_names[ rec.dayOfWeek ] );
			*p++ = ',';
			*p++ = ' ';
			
			p = fast_pstrcpy( p, month_names[ rec.month ] );
			*p++ = ' ';
			
			*p++ = rec.day / 10 + '0';
			*p++ = rec.day % 10 + '0';
			*p++ = ',';
			*p++ = ' ';
			
			*p++ = year / 1000 + '0';  year %= 1000;
			*p++ = year / 100  + '0';  year %= 100;
			*p++ = year / 10   + '0';  year %= 10;
			*p++ = year        + '0';
			
			result[ 0 ] = p - (result + 1);
			break;
		
		case abbrevDate:
			fast_memcpy( result, PSTR_LEN( "<<abbrevDate>>" ) );
			break;
		
		default:
			fast_memcpy( result, PSTR_LEN( "<<invalid IUDateString form>>" ) );
			break;
	}
}

static
pascal short IUMagString_call( char* a, char* b, short an, short bn )
{
	typedef unsigned char uint8_t;
	
	return RelString_sans_case( (uint8_t*) a, an, (uint8_t*) b, bn );
}

static
pascal short IUMagIDString_call( char* a, char* b, short an, short bn )
{
	typedef unsigned char uint8_t;
	
	return CmpString_sans_case( (uint8_t*) a, an, (uint8_t*) b, bn );
}

static
void unimplemented_call( short selector : __D0 )
{
	FATAL = "unimplemented Pack6 call ", selector;
	
	asm { ILLEGAL }
}

asm void Pack6_patch( short selector )
{
	MOVEA.L  (SP)+,A0
	MOVE.W   (SP)+,D0
	MOVE.L   A0,-(SP)
	
	TST.W    D0
	BEQ.S    dispatch_IUDateString
	
	CMPI.W   #0x000A,D0
	BEQ      dispatch_IUMagString
	
	CMPI.W   #0x000C,D0
	BEQ      dispatch_IUMagIDString
	
	JMP      unimplemented_call
	
dispatch_IUDateString:
	JMP      IUDateString_call
	
dispatch_IUMagString:
	JMP      IUMagString_call
	
dispatch_IUMagIDString:
	JMP      IUMagIDString_call
}
