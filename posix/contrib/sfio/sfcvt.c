#include	"sfhdr.h"

/*	Convert a floating point value to ASCII
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

static char	*Inf = "Inf", *Zero = "0";
#define INTPART		(SF_IDIGITS/2)
#define INFINITE	((_Sfi = 3), Inf)
#define ZERO		((_Sfi = 1), Zero)

#if __STD_C
char* _sfcvt(Void_t* dv, int n_digit, int* decpt, int* sign, int format)
#else
char* _sfcvt(dv,n_digit,decpt,sign,format)
Void_t*	dv;		/* value to convert		*/
int	n_digit;	/* number of digits wanted	*/
int*	decpt;		/* to return decimal point	*/
int*	sign;		/* to return sign		*/
int	format;		/* conversion format		*/
#endif
{
	reg char	*sp;
	reg long	n, v;
	reg char	*ep, *buf, *endsp;
	static char	*Buf;

	/* set up local buffer */
	if(!Buf && !(Buf = (char*)malloc(SF_MAXDIGITS)))
		return INFINITE;

	*sign = *decpt = 0;

#if !_ast_fltmax_double
	if(format&SFFMT_LDOUBLE)
	{	Sfdouble_t	dval = *((Sfdouble_t*)dv);

		if(dval == 0.)
			return ZERO;
		else if((*sign = (dval < 0.)) )	/* assignment = */
			dval = -dval;

		n = 0;
		if(dval >= (Sfdouble_t)SF_MAXLONG)
		{	/* scale to a small enough number to fit an int */
			v = SF_MAXEXP10-1;
			do
			{	if(dval < _Sfpos10[v])
					v -= 1;
				else
				{
					dval *= _Sfneg10[v];
					if((n += (1<<v)) >= SF_IDIGITS)
						return INFINITE;
				}
			} while(dval >= (Sfdouble_t)SF_MAXLONG);
		}
		*decpt = (int)n;

		buf = sp = Buf+INTPART;
		if((v = (int)dval) != 0)
		{	/* translate the integer part */
			dval -= (Sfdouble_t)v;

			sfucvt(v,sp,n,ep,long,ulong);

			n = buf-sp;
			if((*decpt += (int)n) >= SF_IDIGITS)
				return INFINITE;
			buf = sp;
			sp = Buf+INTPART;
		}
		else	n = 0;

		/* remaining number of digits to compute; add 1 for later rounding */
		n = (((format&SFFMT_EFORMAT) || *decpt <= 0) ? 1 : *decpt+1) - n;
		if(n_digit > 0)
			n += n_digit;

		if((ep = (sp+n)) > (endsp = Buf+(SF_MAXDIGITS-2)))
			ep = endsp; 
		if(sp > ep)
			sp = ep;
		else
		{
			if((format&SFFMT_EFORMAT) && *decpt == 0 && dval > 0.)
			{	reg Sfdouble_t	d;
				while((d = dval*10.) < 1.)
				{	dval = d;
					*decpt -= 1;
				}
			}

			while(sp < ep)
			{	/* generate fractional digits */
				if(dval <= 0.)
				{	/* fill with 0's */
					do { *sp++ = '0'; } while(sp < ep);
					goto done;
				}
				else if((n = (dval *= 10.)) < 10)
				{	*sp++ = '0' + n;
					dval -= n;
				}
				else /* n == 10 */
				{	do { *sp++ = '9'; } while(sp < ep);
				}
			}
		}
	} else
#endif
	{	double	dval = *((double*)dv);

		if(dval == 0.)
			return ZERO;
		else if((*sign = (dval < 0.)) )	/* assignment = */
			dval = -dval;

		n = 0;
		if(dval >= (double)SF_MAXLONG)
		{	/* scale to a small enough number to fit an int */
			v = SF_MAXEXP10-1;
			do
			{	if(dval < _Sfpos10[v])
					v -= 1;
				else
				{	dval *= _Sfneg10[v];
					if((n += (1<<v)) >= SF_IDIGITS)
						return INFINITE;
				}
			} while(dval >= (double)SF_MAXLONG);
		}
		*decpt = (int)n;

		buf = sp = Buf+INTPART;
		if((v = (int)dval) != 0)
		{	/* translate the integer part */
			dval -= (double)v;

			sfucvt(v,sp,n,ep,long,ulong);

			n = buf-sp;
			if((*decpt += (int)n) >= SF_IDIGITS)
				return INFINITE;
			buf = sp;
			sp = Buf+INTPART;
		}
		else	n = 0;

		/* remaining number of digits to compute; add 1 for later rounding */
		n = (((format&SFFMT_EFORMAT) || *decpt <= 0) ? 1 : *decpt+1) - n;
		if(n_digit > 0)
			n += n_digit;

		if((ep = (sp+n)) > (endsp = Buf+(SF_MAXDIGITS-2)))
			ep = endsp; 
		if(sp > ep)
			sp = ep;
		else
		{
			if((format&SFFMT_EFORMAT) && *decpt == 0 && dval > 0.)
			{	reg double	d;
				while((int)(d = dval*10.) == 0)
				{	dval = d;
					*decpt -= 1;
				}
			}

			while(sp < ep)
			{	/* generate fractional digits */
				if(dval <= 0.)
				{	/* fill with 0's */
					do { *sp++ = '0'; } while(sp < ep);
					goto done;
				}
				else if((n = (dval *= 10.)) < 10)
				{	*sp++ = '0' + n;
					dval -= n;
				}
				else /* n == 10 */
				{	do { *sp++ = '9'; } while(sp < ep);
				}
			}
		}
	}

	if(ep <= buf)
		ep = buf+1;
	else if(ep < endsp)
	{	/* round the last digit */
		*--sp += 5;
		while(*sp > '9')
		{	*sp = '0';
			if(sp > buf)
				*--sp += 1;
			else
			{	/* next power of 10 */
				*sp = '1';
				*decpt += 1;
				if(!(format&SFFMT_EFORMAT))
				{	/* add one more 0 for %f precision */
					ep[-1] = '0';
					ep += 1;
				}
			}
		}
	}

done:
	*--ep = '\0';
	_Sfi = ep-buf;
	return buf;
}
