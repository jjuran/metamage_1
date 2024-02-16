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
                  char         x   : __D1 ),  // right shift bit count
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
                  short        dsrc : __D0,
                  short        ddst : __D1,
                  unsigned     lenn : __D2 ),
                0xFFB6 )

DEFINE_CALLOUT( long,
                send_sound_command,
                ( short        domain : __D1,
                  const void*  buffer : __A0,
                  long         length : __D0 ),
                0xFFAE )

#undef DEFINE_CALLOUT

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
                 unsigned short n )
{
	blit_bytes( src, dst, src_stride, dst_stride, len << 16 | n );
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
