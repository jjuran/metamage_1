/*
	BinaryDecimal.cc
	----------------
*/

#include "BinaryDecimal.hh"

// Mac OS
#ifndef __FP__
#include <fp.h>
#endif
#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif

// Standard C
#include <string.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"


#pragma exceptions off


static
StringPtr NumToString_call( long x : __D0, StringPtr p : __A0 )
{
	const char* s = gear::inscribe_decimal( x );
	
	const int len = strlen( s );
	
	StringPtr q = p;
	
	*p++ = len;
	
	fast_memcpy( p, s, len );
	
	return q;
}

static
long StringToNum_call( ConstStr255Param string : __A0 )
{
	return gear::parse_decimal( CSTR( string ) );
}

static inline
int min( int a, int b )
{
	return b < a ? b : a;
}

static inline
int max( int a, int b )
{
	return a > b ? a : b;
}

/*
	FDec2Str() can be tested by the dec2str program:
	
		cd ../ams-68k-bin
		
		xv68k -St -mlib/ams-{core,io,fs,rsrc,pack} dec2str "$@"
*/

static
void FDec2Str_call( StringPtr dst, const decimal* dec, const decform* form )
{
	const Byte* q = dec->sig.text;
	
	int n_sig = dec->sig.length;
	
	Byte* p = dst + 1;
	
	if ( dec->sgn )
	{
		*p++ = '-';
	}
	else if ( form->style == FLOATDECIMAL )
	{
		*p++ = ' ';
	}
	
	if ( *q == 'I' )
	{
		*p++ = 'I';
		*p++ = 'N';
		*p++ = 'F';
	}
	else if ( *q == 'N' )
	{
		int dec_nan = 0;
		
		if ( n_sig >= 5 )
		{
			q += 3;  // skip "N(0"
			
			for ( int i = 0;  i < 2;  ++i )
			{
				Byte c = *q++;
				
				dec_nan *= 16;
				dec_nan += c - (c < 0x40 ? '0' : 'A' - 10);
			}
		}
		
		fast_memcpy( p, "NAN(000)", 8 );
		
		gear::fill_unsigned< 10, unsigned >( dec_nan, (char*) p + 4, 3 );
		
		p += 8;
	}
	else
	{
		while ( n_sig  &&  *q == '0' )
		{
			++q;
			--n_sig;
		}
		
		int x = n_sig ? dec->exp : 0;
		
		int n_digits = max( form->digits, 0 );
		
		if ( form->style != FLOATDECIMAL )
		{
			int n_major = n_sig ? max( n_sig + x, 0 ) : 0;
			
			int n_minor = max( -x, n_digits );
			
			int has_dot = x < 0  ||  n_digits;  // decimal point
			int n_punct = ! n_major + has_dot;
			
			int n_chars = (!! dec->sgn) + n_major + n_punct + n_minor;
			
			if ( n_chars > DECSTROUTLEN )
			{
				p = dst + 1;
				
				*p++ = '?';
			}
			else
			{
				if ( n_major )
				{
					int n_leading = min( n_major, n_sig );
					
					p = (Byte*) fast_mempcpy( p, q, n_leading );
					
					q += n_leading;
					
					n_major -= n_leading;
					n_sig   -= n_leading;
					
					fast_memset( p, '0', n_major );
					
					p += n_major;
				}
				else
				{
					*p++ = '0';
				}
				
				if ( has_dot )
				{
					*p++ = '.';
				}
				
				int n_under = max( -x - n_sig, 0 );
				
				n_minor -= n_under;
				
				fast_memset( p, '0', n_under );
				
				p += n_under;
				
				int n_trailing = min( n_minor, n_sig );
				
				p = (Byte*) fast_mempcpy( p, q, n_trailing );
				
				n_minor -= n_trailing;
				
				fast_memset( p, '0', n_minor );
				
				p += n_minor;
			}
		}
		else
		{
			if ( n_sig )
			{
				*p++ = *q++;
				
				--n_sig;
				
				x += n_sig;
			}
			else
			{
				*p++ = '0';
			}
			
			Byte xsign = '+';
			
			if ( x < 0 )
			{
				x = -x;
				
				xsign = '-';
			}
			
			int xsize = gear::decimal_magnitude( x );
			
			int n_minor = max( n_sig, n_digits - 1 );
			
			int n_chars = 1 + (n_minor > 0) + n_minor + 2 + xsize;
			
			if ( n_chars > DECSTROUTLEN - 1 )
			{
				p = dst;
				
				*p++ = 1;
				*p++ = '?';
				
				return;
			}
			
			if ( n_minor > 0 )
			{
				*p++ = '.';
				
				int n_trailing = min( n_minor, n_sig );
				
				p = (Byte*) fast_mempcpy( p, q, n_trailing );
				
				n_minor -= n_trailing;
				
				fast_memset( p, '0', n_minor );
				
				p += n_minor;
			}
			
			*p++ = 'e';
			*p++ = xsign;
			
			gear::fill_unsigned< 10, unsigned >( x, (char*) p, xsize );
			
			p += xsize;
		}
	}
	
	dst[ 0 ] = p - (dst + 1);
}

static
void unimplemented_call( short selector : __D1 )
{
	FATAL = "unimplemented Pack7 call ", selector;
	
	asm { ILLEGAL }
}

asm void Pack7_patch( short method )
{
	MOVEA.L  (SP)+,A1
	MOVE.W   (SP)+,D1
	MOVE.L   A1,-(SP)
	
	TST.W    D1
	BEQ      dispatch_NumToString
	
	CMPI.W   #0x0001,D1
	BEQ      dispatch_StringToNum
	
	CMPI.W   #0x0003,D1
	BEQ      dispatch_FDec2Str
	
	JMP      unimplemented_call
	
dispatch_NumToString:
	JMP      NumToString_call
	
dispatch_StringToNum:
	JMP      StringToNum_call
	
dispatch_FDec2Str:
	JMP      FDec2Str_call
}
