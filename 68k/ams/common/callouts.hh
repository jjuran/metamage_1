/*
	callouts.hh
	-----------
*/

#ifndef CALLOUTS_HH
#define CALLOUTS_HH

#define DEFINE_CALLOUT( result, name, args, addr )  \
inline asm result name args  { JSR  (0xFFFF0000 | addr) }

DEFINE_CALLOUT( void, module_suspend, (), 0xFFF8 )

DEFINE_CALLOUT( void, lock_screen,   (), 0xFFEC )
DEFINE_CALLOUT( void, unlock_screen, (), 0xFFEA )

DEFINE_CALLOUT( unsigned long, get_Ticks, (), 0xFFDE )

DEFINE_CALLOUT( struct timeval*,
                timeval_from_microseconds,
                ( struct timeval*      dst : __A0,
                  unsigned long long*  src : __A1 ),
                0xFFDA )

DEFINE_CALLOUT( struct timeval*,
                timeval_from_nanoseconds,
                ( struct timeval*      dst : __A0,
                  unsigned long long*  src : __A1 ),
                0xFFD8 )

DEFINE_CALLOUT( void,
                fast_memset,
                ( void* : __A0, char : __D1, unsigned long : __D0 ),
                0xFFD6 )

DEFINE_CALLOUT( void,
                fast_memnot,
                ( void* : __A0, unsigned long : __D0 ),
                0xFFD4 )

DEFINE_CALLOUT( void,
                fast_rshift,
                ( void*        dst : __A1,
                  void const*  src : __A0,
                  unsigned     n   : __D0,    // src n_bytes; dst n is src n + 1
                  int          x   : __D1 ),  // right shift bit count
                0xFFD2 )

DEFINE_CALLOUT( void,
                fast_lshift,
                ( void*        dst : __A1,
                  void const*  src : __A0,
                  unsigned     n   : __D0,    // n_bytes
                  int          x   : __D2,    // left shift bit count
                  int          z   : __D1 = 0 ),
                0xFFD2 )

DEFINE_CALLOUT( void*,
                fast_mempcpy,
                ( void*        dst : __A0,
                  void const*  src : __A1,
                  unsigned     n   : __D0 ),
                0xFFD0 )

DEFINE_CALLOUT( int,
                fast_memcmp,
                ( void const*  a : __A0,
                  void const*  b : __A1,
                  unsigned     n : __D0 ),
                0xFFC6 )

DEFINE_CALLOUT( void,
                plot_cursor,
                ( const struct Cursor*  crsr : __A1,
                  void*                 dest : __A0,
                  unsigned              ht_s : __D0,
                  unsigned              n_vs : __D1,
                  unsigned              rowB : __D2 ),
                0xFFBE )

DEFINE_CALLOUT( void,
                notify_cursor_moved,
                ( long loc : __D0 ),
                0xFFBC )

DEFINE_CALLOUT( void,
                notify_cursor_vis,
                ( unsigned char vis : __D0 ),
                0xFFBA )

DEFINE_CALLOUT( void,
                notify_cursor_set,
                ( const struct Cursor* crsr : __A0 ),
                0xFFB8 )

DEFINE_CALLOUT( void,
                blit_bytes,
                ( const void*  src  : __A0,
                  void*        dst  : __A1,
                  unsigned     dsrc : __D0,
                  unsigned     ddst : __D1,
                  unsigned     lenn : __D2 ),
                0xFFB6 )

DEFINE_CALLOUT( void,
                draw_bytes,
                ( const void*  pattern : __A0,
                  unsigned     pat_v_h : __D0,
                  void*        dst_buf : __A1,
                  unsigned     dst_h_w : __D1,
                  unsigned     rb_mode : __D2 ),
                0xFFB4 )

DEFINE_CALLOUT( long,
                send_sound_command,
                ( short        domain : __D1,
                  const void*  buffer : __A0,
                  long         length : __D0 ),
                0xFFAE )

DEFINE_CALLOUT( long,
                next_pseudorandom,
                ( long randSeed : __D0 ),
                0xFFA6 )

DEFINE_CALLOUT( long,
                fast_fix_mul,
                ( long a : __D0,
                  long b : __D1 ),
                0xFF9E )

DEFINE_CALLOUT( long,
                fast_fix_ratio,
                ( short numer : __D0,
                  short denom : __D1 ),
                0xFF9C )

DEFINE_CALLOUT( short,
                fast_fix_round,
                ( long x : __D0 ),
                0xFF9A )

DEFINE_CALLOUT( long,
                fast_fix_div,
                ( long numer : __D0,
                  long denom : __D1 ),
                0xFF98 )

DEFINE_CALLOUT( void,
                sect_rect_region,
                ( struct Rect      const*  rect : __D0,
                  struct MacRegion const*  src  : __A0,
                  struct MacRegion*        dst  : __A1 ),
                0xFF96 )

#undef DEFINE_CALLOUT

inline
void fast_lshift( void* dst : __A1, unsigned n : __D0, int x : __D2 )
{
	return fast_lshift( dst, dst, n, x, 0 );
}

inline
int fast_memequ( const void* a, const void* b, unsigned long n )
{
	return fast_memcmp( a, b, n ) == 0;
}

static inline
asm bool mem_test( const void* addr : __A0, int size : __D0 )
{
	MOVEQ.L  #0,D1
	MOVEQ.L  #0,D2
	
	JSR      0xFFFFFFCE
}

inline asm
void fast_memcpy( void* : __A1, const void* : __A0, unsigned long : __D0 )
{
	0xA22E  // _BlockMoveData
}

inline asm
void fast_memmove( void* : __A1, const void* : __A0, unsigned long : __D0 )
{
	0xA22E  // _BlockMoveData
}

inline
void plot_cursor( const struct Cursor*  crsr,
                  void*                 dest,
                  short                 shift,
                  short                 h_trim,
                  short                 v_skip,
                  short                 n,
                  short                 rowBytes )
{
	plot_cursor( crsr, dest, shift | h_trim << 16, v_skip | n << 16, rowBytes );
}

inline
void blit_bytes( const void* src, unsigned short src_stride,
                 void*       dst, unsigned short dst_stride,
                 unsigned short len,
                 unsigned short n,
                 unsigned short mode = 0 )
{
	blit_bytes( src, dst, src_stride, dst_stride | mode << 16, len << 16 | n );
}

inline
void draw_bytes( const void* pat, unsigned short h_index, unsigned short v_index,
                 void*       dst, unsigned short dst_len, unsigned short dst_n,
                                  unsigned short stride,  unsigned short mode )
{
	draw_bytes( pat, v_index << 16 | h_index,
	            dst, dst_len << 16 | dst_n,
	                 stride  << 16 | mode );
}

inline
long next_pseudorandom( long* randSeed )
{
	return *randSeed = next_pseudorandom( *randSeed );
}

unsigned long CmpString_sans_case( const unsigned char*  a : __A0,
                                   unsigned char         n : __D1,
                                   const unsigned char*  b : __A1,
                                   unsigned char         m : __D2 );

inline
unsigned long EqualString_sans_case( const unsigned char*  a : __A0,
                                     unsigned char         n : __D1,
                                     const unsigned char*  b : __A1,
                                     unsigned char         m : __D2 )
{
	return ! CmpString_sans_case( a, n, b, m );
}

inline
unsigned long EqualString_sans_case( const unsigned char*  a : __A0,
                                     unsigned char         n : __D1,
                                     const unsigned char*  b : __A1 )
{
	unsigned char m = *b++;
	
	return EqualString_sans_case( a, n, b, m );
}

inline
unsigned long EqualString_sans_case( const unsigned char*  a : __A0,
                                     const unsigned char*  b : __A1 )
{
	unsigned char n = *a++;
	unsigned char m = *b++;
	
	return EqualString_sans_case( a, n, b, m );
}

long RelString_sans_case( const unsigned char*  a : __A0,
                          unsigned char         n : __D1,
                          const unsigned char*  b : __A1,
                          unsigned char         m : __D2 );

inline
long RelString_sans_case( const unsigned char*  a : __A0,
                          const unsigned char*  b : __A1 )
{
	unsigned char n = *a++;
	unsigned char m = *b++;
	
	return RelString_sans_case( a, n, b, m );
}

#endif
