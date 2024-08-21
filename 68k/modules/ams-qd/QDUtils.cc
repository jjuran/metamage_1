/*
	QDUtils.cc
	----------
*/

#include "QDUtils.hh"

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// quickdraw
#include "qd/pack_bits.hh"
#include "qd/regions.hh"
#include "qd/region_detail.hh"

// ams-common
#include "callouts.hh"
#include "math.hh"
#include "QDGlobals.hh"


static inline
size_t byte_distance( const void* begin, const void* end )
{
	return (const char*) end - (const char*) begin;
}

template < class T >
static inline
void advance_bytes( T*& pointer, size_t n )
{
	(const char*&) pointer += n;
}

pascal void PackBits_patch( Ptr* src, Ptr* dst, short srcBytes )
{
	UInt8** tmp = (UInt8**) dst;
	
	quickdraw::pack_bits( (const UInt8*&) *src, srcBytes, *tmp );
	
	dst = (Ptr*) tmp;
}

pascal void UnpackBits_patch( Ptr* src, Ptr* dst, short dstBytes )
{
	UInt8** tmp = (UInt8**) dst;
	
	quickdraw::unpack_bits( (const UInt8*&) *src, *tmp, dstBytes );
	
	dst = (Ptr*) tmp;
}

pascal unsigned char BitTst_patch( Ptr addr, long bit )
{
	addr += bit / 8u;
	bit   = bit % 8u;
	
	return (*addr & (1 << 7 - bit)) != 0;
}

pascal void BitSet_patch( Ptr addr, long bit )
{
	addr += bit / 8u;
	bit   = bit % 8u;
	
	const uint8_t mask = 1 << 7 - bit;
	
	*addr |= mask;
}

pascal void BitClr_patch( Ptr addr, long bit )
{
	addr += bit / 8u;
	bit   = bit % 8u;
	
	const uint8_t mask = 1 << 7 - bit;
	
	*addr &= ~mask;
}

pascal short Random_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	next_pseudorandom( &qd.randSeed );
	
	if ( (short) qd.randSeed == (short) 0x8000 )
	{
		return 0;
	}
	
	return qd.randSeed;
}

pascal long BitAnd_patch( long a, long b )
{
	return a & b;
}

pascal long BitNot_patch( long x )
{
	return ~x;
}

pascal unsigned long BitShift_patch( unsigned long x, short n )
{
	return + n > 0 ? x << ( n % 32u)
	       : n < 0 ? x >> (-n % 32u)
	       :         x;
}

static const char hex_table[ 32 ] =
{
	0,  10, 11, 12, 13, 14, 15, 0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  1,  2,  3,  4,  5,  6,  7,
	8,  9,  0   // more zeros follow
};

const char xdigit_mask = 0x1F;

static inline 
unsigned char unhex( char xdigit )
{
	return hex_table[ xdigit & xdigit_mask ];
}

pascal void StuffHex_patch( char* dst, const unsigned char* srcHex )
{
	uint8_t len = *srcHex++;
	
	unsigned char c;
	
	while ( len > 0 )
	{
		c  = unhex( *srcHex++ ) << 4;
		c |= unhex( *srcHex++ );
		
		*dst++ = c;
		
		len -= 2;
	}
}

pascal void MapPt_patch( Point* pt, const Rect* src, const Rect* dst )
{
	const UInt16 srcHeight = src->bottom - src->top;
	const UInt16 dstHeight = dst->bottom - dst->top;
	
	const UInt16 srcWidth = src->right - src->left;
	const UInt16 dstWidth = dst->right - dst->left;
	
	pt->v = muldivu_w( pt->v - src->top,  dstHeight, srcHeight ) + dst->top;
	pt->h = muldivu_w( pt->h - src->left, dstWidth,  srcWidth  ) + dst->left;
}

pascal void MapRect_patch( Rect* r, const Rect* src, const Rect* dst )
{
	MapPt( (Point*) &r->top,    src, dst );
	MapPt( (Point*) &r->bottom, src, dst );
}

static
short* next_row( short* p )
{
	using quickdraw::Region_end;
	
	while ( *p++ != Region_end )
	{
		continue;
	}
	
	return p;
}

static
bool compact_region_vertically( RgnHandle rgn )
{
	using quickdraw::Region_end;
	
	short* q = (short*) (*rgn + 1);
	
	short* r1 = q;
	short* r2 = next_row( r1 );
	
	while ( *r2 != Region_end )
	{
		if ( *r2 == *r1 )
		{
			*q++ = *r1++;  // copy v coordinate
			
			// pointers to h coordinates
			short* p1 = r1;
			short* p2 = r2 + 1;
			
			while ( *p1 != Region_end  &&  *p2 != Region_end )
			{
				// Copy the lesser h coordinate...
				
				if ( *p1 < *p2 )
				{
					*q++ = *p1++;
				}
				else if ( *p2 < *p1 )
				{
					if ( q == p1 )
					{
						/*
							We're about to clobber row 1 with data from row 2.
							Shift the remaining row 1 data to the right, which
							will overwrite previous data from row 2 that we've
							already copied.
						*/
						
						const size_t n_bytes = byte_distance( p1, r2 );
						
						advance_bytes( p1, byte_distance( r2, p2 ) );
						
						fast_memmove( p1, q, n_bytes );
					}
					
					*q++ = *p2++;
				}
				else
				{
					// ... unless they're equal, in which case skip both
					
					++p1;
					++p2;
				}
			}
			
			while ( *p1 != Region_end )
			{
				*q++ = *p1++;
			}
			
			while ( *p2 != Region_end )
			{
				*q++ = *p2++;
			}
			
			*q++ = *p2++;  // copy end-of-row sentinel
			
			if ( *p2 == Region_end )
			{
				goto done;
			}
			
			r1 = p2;
			
			r2 = next_row( r1 );
		}
		else
		{
			while ( r1 < r2 )
			{
				*q++ = *r1++;
			}
			
			r2 = next_row( r1 );
		}
	}
	
	while ( r1 < r2 )
	{
		*q++ = *r1++;
	}
	
done:
	
	*q++ = Region_end;  // end-of-region sentinel
	
	const Size size = (Byte*) q - (Byte*) *rgn;
	
	if ( size == rgn[0]->rgnSize )
	{
		return false;
	}
	
	rgn[0]->rgnSize = size;
	
	return true;
}

static
void compact_region_horizontally( RgnHandle rgn )
{
	using quickdraw::Region_end;
	
	short* p = (short*) (*rgn + 1);
	short* q = p;
	
	while ( *p != Region_end )
	{
		const short v = *p++;
		
		*q++ = v;
		
		const short* r = q;  // start of row of h coordinates (following v)
		
		short last_h = -0x8000;
		
		while ( *p != Region_end )
		{
			const short h = *p++;
			
			if ( h == last_h )
			{
				--q;
				
				last_h = -0x8000;
			}
			else
			{
				last_h = *q++ = h;
			}
		}
		
		if ( q == r )
		{
			--q;
			++p;
		}
		else
		{
			*q++ = *p++;  // copy end-of-row sentinel
		}
	}
	
	*q++ = *p++;  // copy end-of-region sentinel
	
	rgn[0]->rgnSize = (Byte*) q - (Byte*) *rgn;
}

static
void compact_region( RgnHandle rgn )
{
	using quickdraw::set_region_bbox;
	
	while ( compact_region_vertically  ( rgn ) )  continue;
	
	compact_region_horizontally( rgn );
	
	set_region_bbox( &rgn[0]->rgnBBox.top, (short*) (*rgn + 1) );
	
	SetHandleSize( (Handle) rgn, rgn[0]->rgnSize );
}

pascal void MapRgn_patch( RgnHandle rgn, const Rect* src, const Rect* dst )
{
	using quickdraw::Region_end;
	
	const UInt16 srcHeight = src->bottom - src->top;
	const UInt16 dstHeight = dst->bottom - dst->top;
	
	const UInt16 srcWidth = src->right - src->left;
	const UInt16 dstWidth = dst->right - dst->left;
	
	short* p = (short*) (*rgn + 1);
	
	while ( *p != Region_end )
	{
		const short v = *p;
		
		*p++ = muldivu_w( v - src->top, dstHeight, srcHeight ) + dst->top;
		
		while ( *p != Region_end )
		{
			const short h = *p;
			
			*p++ = muldivu_w( h - src->left, dstWidth, srcWidth ) + dst->left;
		}
		
		++p;  // skip end-of-row sentinel
	}
	
	compact_region( rgn );
}

pascal long DeltaPoint_patch( Point a, Point b )
{
	Point delta;
	
	delta.v = a.v - b.v;
	delta.h = a.h - b.h;
	
	return *(long*) &delta;
}

/*
	The logic and tables for slope<->angle conversion are loosely based
	on Executor source code (see src/toolutil.c) with some corrections.
	
	https://github.com/ctm/executor/blob/master/src/toolutil.c#L458
	
	Thanks to ctm and ARDI for releasing the Executor source code!
*/

static const Fixed slope_table[] =
{
	0x80000001,
	0x00394a30, 0x001ca2d7, 0x001314bd, 0x000e4cf5, 0x000b6e17,
	0x000983ad, 0x000824f3, 0x00071d88, 0x00065051, 0x0005abd9,
	0x00052501, 0x0004b462, 0x000454db, 0x000402c2, 0x0003bb68,
	0x00037cc7, 0x00034556, 0x000313e3, 0x0002e77a, 0x0002bf5b,
	0x00029ae7, 0x0002799f, 0x00025b19, 0x00023efc, 0x000224fe,
	0x00020ce1, 0x0001f66e, 0x0001e177, 0x0001cdd6, 0x0001bb68,
	0x0001aa0e, 0x000199af, 0x00018a35, 0x00017b89, 0x00016d9b,
	0x0001605b, 0x000153b9, 0x000147aa, 0x00013c22, 0x00013117,
	0x0001267f, 0x00011c51, 0x00011287, 0x00010919, 0x00010000,
	
	0x0000f737, 0x0000eeb9, 0x0000e681, 0x0000de8a, 0x0000d6cf,
	0x0000cf4e, 0x0000c802, 0x0000c0e9, 0x0000b9ff, 0x0000b341,
	0x0000acad, 0x0000a640, 0x00009ff7, 0x000099d2, 0x000093cd,
	0x00008de7, 0x0000881e, 0x00008270, 0x00007cdc, 0x00007760,
	0x000071fb, 0x00006caa, 0x0000676e, 0x00006245, 0x00005d2d,
	0x00005826, 0x0000532e, 0x00004e44, 0x00004968, 0x00004498,
	0x00003fd4, 0x00003b1a, 0x0000366a, 0x000031c3, 0x00002d24,
	0x0000288c, 0x000023fa, 0x00001f6f, 0x00001ae8, 0x00001666,
	0x000011e7, 0x00000d6b, 0x000008f1, 0x00000478, 0x00000000,
};

pascal long SlopeFromAngle_patch( short angle )
{
	bool reflected = false;
	
	if ( angle < 0 )
	{
		angle += 360;
		
		if ( angle < 0 )
		{
			angle += 32760;
			angle %= 360;
		}
	}
	else if ( angle > 360 )
	{
		angle %= 360;
	}
	
	if ( angle > 180 )
	{
		angle -= 180;
	}
	
	if ( angle <= 90 )
	{
		angle = 180 - angle;
		reflected = true;
	}
	
	Fixed slope = slope_table[ angle - 90 ];
	
	if ( reflected )
	{
		slope = -slope;
	}
	
	return slope;
}

static const Fixed slope_inverse_table[] =
{
	0x00394c25, 0x001ca4cc, 0x001316b2, 0x000e4eea, 0x000b700c,
	0x000985a2, 0x000826e8, 0x00071f7d, 0x00065246, 0x0005adce,
	0x000526f6, 0x0004b657, 0x000456d0, 0x000404b7, 0x0003bd5d,
	0x00037ebc, 0x0003474b, 0x000315d8, 0x0002e96f, 0x0002c150,
	0x00029cdc, 0x00027b94, 0x00025d0e, 0x000240f1, 0x000226f3,
	0x00020ed6, 0x0001f863, 0x0001e36c, 0x0001cfcb, 0x0001bd5d,
	0x0001ac03, 0x00019ba4, 0x00018c2a, 0x00017d7e, 0x00016f90,
	0x00016250, 0x000155ae, 0x0001499f, 0x00013e17, 0x0001330c,
	0x00012874, 0x00011e46, 0x0001147c, 0x00010b0e, 0x000101f5,
	
	0x0000f92c, 0x0000f0ae, 0x0000e876, 0x0000e07f, 0x0000d8c4,
	0x0000d143, 0x0000c9f7, 0x0000c2de, 0x0000bbf4, 0x0000b536,
	0x0000aea2, 0x0000a835, 0x0000a1ec, 0x00009bc7, 0x000095c2,
	0x00008fdc, 0x00008a13, 0x00008465, 0x00007ed1, 0x00007955,
	0x000073f0, 0x00006e9f, 0x00006963, 0x0000643a, 0x00005f22,
	0x00005a1b, 0x00005523, 0x00005039, 0x00004b5d, 0x0000468d,
	0x000041c9, 0x00003d0f, 0x0000385f, 0x000033b8, 0x00002f19,
	0x00002a81, 0x000025ef, 0x00002164, 0x00001cdd, 0x0000185b,
	0x000013dc, 0x00000f60, 0x00000ae6, 0x0000066d, 0x000001f5,
	0x00000000,
};

pascal short AngleFromSlope_patch( long slope )
{
	bool reflected = false;
	
	if ( slope < 0 )
	{
		slope = -slope;
		reflected = true;
	}
	
	const Fixed* p = slope_inverse_table;
	
	while ( *p > slope )
	{
		++p;
	}
	
	short angle = byte_distance( slope_inverse_table, p ) / 4u;
	
	if ( reflected  &&  angle != 90 )
	{
		angle = -angle;
	}
	
	return angle + 90;
}

pascal long PinRect_patch( const Rect* rect, Point pt )
{
	if ( pt.h < rect->left )
	{
		pt.h = rect->left;
	}
	
	if ( pt.v < rect->top )
	{
		pt.v = rect->top;
	}
	
	if ( pt.h > rect->right )
	{
		pt.h = rect->right - 1;
	}
	
	if ( pt.v > rect->bottom )
	{
		pt.v = rect->bottom - 1;
	}
	
	return (long&) pt;
}
