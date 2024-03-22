/*
	bridge.cc
	---------
*/

#include "callout/bridge.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// log-of-war
#include "logofwar/report.hh"

// v68k-alloc
#include "v68k-alloc/memory.hh"

// v68k-auth
#include "auth/auth.hh"

// v68k-syscalls
#include "syscall/bridge.hh"

// v68k-ioutils
#include "ioutils/load.hh"

// v68k-callouts
#include "callout/cursor.hh"
#include "callout/exceptions.hh"
#include "callout/fixed.hh"
#include "callout/notify_cursor.hh"
#include "callout/regions.hh"
#include "callout/screen.hh"
#include "callout/sound.hh"
#include "callout/timeval.hh"
#include "callout/uptime.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k    {
namespace callout {

using v68k::mem_read;
using v68k::mem_write;
using v68k::mem_update;

using v68k::auth::fully_authorized;
using v68k::auth::supervisor_mode_switch_allowed;


enum
{
	nop = 0x4E71,
	rts = 0x4E75,
	nil = 0
};

typedef int32_t (*function_type)( v68k::processor_state& s );

enum code32_t
{
	code32_min = 0,
	code32_max = 0xffffffff,
};

static
void print( code32_t x )
{
	char buffer[ 7 ];
	
	char* p = buffer;
	
	*p++ = '\'';
	*p++ = x >> 24;
	*p++ = x >> 16;
	*p++ = x >>  8;
	*p++ = x >>  0;
	*p++ = '\'';
	*p++ = '\0';
	
	logofwar::print( buffer );
}

static
int32_t unimplemented_callout( v68k::processor_state& s )
{
	abort();
	
	// Not reached
	return nil;
}

static
int32_t no_op_callout( v68k::processor_state& s )
{
	return rts;
}

static
int32_t load_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t path_addr = s.a(0);
	const uint32_t path_size = s.d(0);  // includes trailing NUL
	
	s.a(0) = 0;
	
	if ( !fully_authorized )
	{
		s.d(1) = EPERM;
		
		return rts;
	}
	
	const uint8_t* p = s.translate( path_addr,
	                                path_size,
	                                data_space,
	                                mem_read );
	
	if ( p == NULL )
	{
		s.d(1) = EFAULT;
		
		return rts;
	}
	
	if ( path_size == 0  ||  p[0] == '\0'  ||  p[ path_size - 1 ] != '\0' )
	{
		s.d(1) = EINVAL;
		
		return rts;
	}
	
	using v68k::alloc::page_size;
	using v68k::alloc::page_size_bits;
	using v68k::alloc::allocate_n_pages;
	using v68k::ioutils::load_file;
	
	const char* path = (const char*) p;
	
	void* alloc = load_file( path, &s.d(0) );
	
	const size_t n = (s.d(0) + page_size - 1) >> page_size_bits;  // round up
	
	const uint32_t addr = allocate_n_pages( alloc, n );
	
	if ( addr == 0 )
	{
		free( alloc );
		
		s.d(1) = ENOMEM;
	}
	else
	{
		s.a(0) = addr;
	}
	
	return rts;
}

static
int32_t enter_supervisor_mode_callout( v68k::processor_state& s )
{
	const uint16_t old_SR = s.get_SR();
	const uint16_t new_SR = old_SR | 0x2000;
	
	s.d(0) = old_SR;
	
	if ( old_SR == new_SR )
	{
		s.sr.nzvc = 4;  // Clear N, set Z
		
		return rts;
	}
	
	if ( supervisor_mode_switch_allowed )
	{
		uint32_t return_address;
		
		if ( ! s.get_long( s.a(7), return_address, s.data_space() ) )
		{
			abort();  // FIXME
			return nil;
		}
		
		s.a(7) += 4;
		
		s.set_SR( new_SR );
		
		s.sr.nzvc = 0;  // Clear N, Z
		
		s.pc() = return_address - 2;
		
		return nop;
	}
	else
	{
		s.d(0) = 0xFFFFFFFF;
		s.d(1) = EPERM;
		
		s.sr.nzvc = 8;  // Set N
	}
	
	return rts;
}

static
int32_t module_suspend_callout( v68k::processor_state& s )
{
	s.condition = startup;
	
	return rts;
}

static
int32_t set_trace_mode_callout( v68k::processor_state& s )
{
	s.sr.ttsm = (s.sr.ttsm & 0x3) | (uint8_t( s.pc() ) << 1 & 0xC);
	
	return rts;
}

enum
{
	noErr      = 0,
	memFullErr = -108,
};

static
int32_t alloc_callout( v68k::processor_state& s )
{
	int32_t err = noErr;
	
	const uint32_t size = s.d(0);
	
	uint32_t addr = v68k::alloc::allocate( size );
	
	s.a(0) = addr;
	
	if ( addr == 0 )
	{
		err = memFullErr;
		
		WARNING = "Memory exhaustion: ", size, "-byte allocation failed";
	}
	
	s.d(0) = err;
	
	return rts;
}

static
int32_t dealloc_callout( v68k::processor_state& s )
{
	const uint32_t addr = s.a(0);
	
	v68k::alloc::deallocate( addr );
	
	s.d(0) = noErr;
	
	return rts;
}

static
int32_t fast_memset_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t mem = s.a(0);
	
	const uint32_t n = s.d(0);
	const uint32_t x = s.d(1);
	
	if ( n == 0 )
	{
		return rts;
	}
	
	uint8_t* p = s.translate( mem, n, data_space, mem_write );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	memset( p, x, n );
	
	s.translate( mem, n, data_space, mem_update );
	
	return rts;
}

static
int32_t fast_memnot_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t mem = s.a(0);
	
	const uint32_t n = s.d(0);
	
	if ( n == 0 )
	{
		return rts;
	}
	
	uint8_t* p = s.translate( mem, n, data_space, mem_write );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* q = p + n;
	
	while ( p < q )
	{
		*p++ ^= 0xFF;
	}
	
	s.translate( mem, n, data_space, mem_update );
	
	return rts;
}

static
int32_t fast_shift_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t a = s.a(0);
	const uint32_t b = s.a(1);
	
	const uint32_t n = s.d(0);
	const uint32_t x = s.d(1);
	const uint32_t y = s.d(2);
	
	if ( (x ? x : y) >= 8 )
	{
		abort();
		return nil;  // FIXME
	}
	
	const uint8_t right_shift = x ? x : 8 - y;
	const uint8_t left_shift  = x ? 8 - x : y;
	
	const uint8_t* src = s.translate( a, n, data_space, mem_read );
	
	if ( src == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	const uint32_t n_dst_bytes = n + (x != 0);
	
	uint8_t* dst = s.translate( b, n_dst_bytes, data_space, mem_write );
	
	if ( dst == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint32_t n_src_bytes = n;
	
	if ( x )
	{
		while ( n_src_bytes-- > 0 )
		{
			const uint8_t byte = *src++;
			
			*dst++ |= byte >> right_shift;
			*dst    = byte << left_shift;
		}
	}
	else if ( n_src_bytes )
	{
		while ( --n_src_bytes > 0 )
		{
			*dst    = *src++ << left_shift;
			*dst++ |= *src   >> right_shift;
		}
		
		*dst = *src << left_shift;
	}
	
	s.translate( b, n_dst_bytes, data_space, mem_update );
	
	return rts;
}

static
int32_t fast_mempcpy_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t dst = s.a(0);
	const uint32_t src = s.a(1);
	
	const uint32_t n = s.d(0);
	
	const uint8_t* p = s.translate( src, n, data_space, mem_read );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* q = s.translate( dst, n, data_space, mem_write );
	
	if ( q == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	memmove( q, p, n );
	
	s.translate( dst, n, data_space, mem_update );
	
	s.a(0) += n;
	s.a(1) += n;
	
	return rts;
}

static
int32_t mem_test_callout( v68k::processor_state& s )
{
	uint32_t addr = s.a(0);
	uint32_t size = s.d(0);
	
	function_code_t fc = s.data_space();
	
	bool ok = s.mem.translate( addr, size, fc, mem_read ) != NULL;
	
	s.d(0) = ok;
	
	return rts;
}

static
int32_t fast_memcmp_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t one = s.a(0);
	const uint32_t two = s.a(1);
	const uint32_t len = s.d(0);
	
	const uint8_t* p = s.mem.translate( one, len, data_space, mem_read );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	const uint8_t* q = s.mem.translate( two, len, data_space, mem_read );
	
	if ( q == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	s.d(0) = memcmp( p, q, len );
	
	return rts;
}

enum
{
	srcCopy,
	srcOr,
	srcXor,
	srcBic,
	
	notSrcCopy,
	notSrcOr,
	notSrcXor,
	notSrcBic,
};

typedef unsigned char Byte;

static inline
void blit_srcCopy( const Byte* src, Byte* dst, size_t len )
{
	/*
		We have to use memmove(), not memcpy(), because we
		don't know if userspace passed us overlapping ranges.
	*/
	
	memmove( dst, src, len );
}

#define DEFINE_BLIT_MODE( name, op )  \
static inline void  \
blit_##name( const Byte* p, Byte* q, int n )  { while ( n-- )  *q++ op *p++; }

DEFINE_BLIT_MODE( srcOr,      |=   )
DEFINE_BLIT_MODE( srcXor,     ^=   )
DEFINE_BLIT_MODE( srcBic,     &= ~ )

DEFINE_BLIT_MODE( notSrcCopy,  = ~ )
DEFINE_BLIT_MODE( notSrcOr,   |= ~ )
DEFINE_BLIT_MODE( notSrcXor,  ^= ~ )
DEFINE_BLIT_MODE( notSrcBic,  &=   )

#undef DEFINE_BLIT_MODE

static
int32_t blit_bytes_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint16_t n = s.d(2);
	
	const uint32_t len = s.d(2) >> 16;
	
	if ( n == 0  ||  len == 0 )
	{
		return rts;
	}
	
	const uint16_t src_stride = s.d(0);
	const uint16_t dst_stride = s.d(1);
	
	uint32_t src = s.a(0);
	uint32_t dst = s.a(1);
	
	/*
		Limiting n, len, and stride to 16 bits ensures
		that the multiplications won't overflow 32 bits.
	*/
	
	const uint32_t src_span = (n - 1) * src_stride + len;
	const uint32_t dst_span = (n - 1) * dst_stride + len;
	
	const uint8_t* p = s.translate( src, src_span, data_space, mem_read );
	
	if ( p == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* q = s.translate( dst, dst_span, data_space, mem_write );
	
	if ( q == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	const uint16_t mode = s.d(1) >> 16;
	
#define BLIT_MODE_CASE( name )  \
case name:  while ( n-- )       \
{ blit_##name( p, q, len );  p += src_stride;  q += dst_stride; }  break
	
	switch ( mode )
	{
		BLIT_MODE_CASE( srcCopy    );
		BLIT_MODE_CASE( srcOr      );
		BLIT_MODE_CASE( srcXor     );
		BLIT_MODE_CASE( srcBic     );
		BLIT_MODE_CASE( notSrcCopy );
		BLIT_MODE_CASE( notSrcOr   );
		BLIT_MODE_CASE( notSrcXor  );
		BLIT_MODE_CASE( notSrcBic  );
		
		default:
			break;
	}
	
#undef BLIT_MODE_CASE
	
	s.translate( dst, dst_span, data_space, mem_update );
	
	return rts;
}

static
uint8_t* patcpy_mode( uint8_t* dst, uint8_t pat, size_t n, short mode )
{
	switch ( mode )
	{
		case srcCopy:  for ( int i = 0; i < n; ++i )  *dst++  =  pat;  break;
		case srcOr:    for ( int i = 0; i < n; ++i )  *dst++ |=  pat;  break;
		case srcXor:   for ( int i = 0; i < n; ++i )  *dst++ ^=  pat;  break;
		case srcBic:   for ( int i = 0; i < n; ++i )  *dst++ &= ~pat;  break;
	}
	
	return dst;
}

static
int32_t draw_bytes_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	uint16_t src_n = 8;  // sizeof (Pattern)
	uint16_t dst_n = s.d(1);
	
	const uint32_t dst_len = s.d(1) >> 16;
	
	if ( dst_n == 0  ||  dst_len == 0 )
	{
		return rts;
	}
	
	const uint32_t dst_stride = s.d(2) >> 16;
	
	uint32_t src = s.a(0);
	uint32_t dst = s.a(1);
	
	/*
		Limiting n, len, and stride to 16 bits ensures
		that the multiplications won't overflow 32 bits.
	*/
	
	const uint32_t src_span =  src_n;
	const uint32_t dst_span = (dst_n - 1) * dst_stride + dst_len;
	
	const uint8_t* pattern = s.translate( src, src_span, data_space, mem_read );
	
	if ( pattern == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
	uint8_t* q = s.translate( dst, dst_span, data_space, mem_write );
	
	if ( q == NULL )
	{
		abort();
		return nil;  // FIXME
	}
	
//	uint16_t h_index =  s.d(0)        & 0x07;  // not yet implemented
	uint16_t v_index = (s.d(0) >> 16) & 0x07;
	
	const uint16_t mode = s.d(2);
	
	while ( dst_n-- )
	{
		patcpy_mode( q, pattern[ v_index ], dst_len, mode );
		
		q += dst_stride;
		
		++v_index;
		
		v_index &= 0x7;
	}
	
	s.translate( dst, dst_span, data_space, mem_update );
	
	return rts;
}

static
int32_t next_pseudorandom_callout( v68k::processor_state& s )
{
	uint32_t randSeed = s.d(0);
	
	randSeed = randSeed * 16807ull % 0x7FFFFFFF;
	
	s.d(0) = randSeed;
	
	return rts;
}

static
int32_t system_call_callout( v68k::processor_state& s )
{
	op_result result = bridge_call( s );
	
	if ( result >= 0 )
	{
		return rts;
	}
	else if ( result == Illegal_instruction )
	{
		FATAL = "Unimplemented system call: ", s.d(0);
	}
	
	return nil;
}

static
int32_t FreeMem_callout( v68k::processor_state& s )
{
	s.d(0) = v68k::alloc::count_free_bytes();
	
	return rts;
}

static
int32_t BlockMove_callout( v68k::processor_state& s )
{
	const v68k::function_code_t data_space = s.data_space();
	
	const uint32_t src = s.a(0);
	const uint32_t dst = s.a(1);
	
	const int32_t n = s.d(0);
	
	if ( n > 0 )
	{
		const uint8_t* p = s.translate( src, n, data_space, mem_read );
		
		if ( p == NULL )
		{
			abort();
			return nil;  // FIXME
		}
		
		uint8_t* q = s.translate( dst, n, data_space, mem_write );
		
		if ( q == NULL )
		{
			abort();
			return nil;  // FIXME
		}
		
		memmove( q, p, n );
		
		s.translate( dst, n, data_space, mem_update );
	}
	else if ( n < 0 )
	{
		ERROR = "Negative BlockMove byte count: ", n;
	}
	
	s.d(0) = noErr;
	
	return rts;
}

#define OSTYPE(a, b, c, d)  ((a) << 24 | (b) << 16 | (c) << 8 | (d))

static
int32_t Gestalt_callout( v68k::processor_state& s )
{
	const int32_t gestaltUndefSelectorErr = -5551;
	
	const uint32_t selector = s.d(0);
	
	uint32_t value = 0;
	int32_t result = 0;
	
	switch ( selector )
	{
		case OSTYPE('v', '6', '8', 'k'):  // 'v68k'
		case OSTYPE('m', 'a', 'c', 'h'):  // 'mach'
		//	value = 0;
			break;
		
		case OSTYPE('p', 'r', 'o', 'c'):  // 'proc'
			value = (s.model >> 4) + 1;
			break;
		
		default:
			result = gestaltUndefSelectorErr;
			
			NOTICE = "Undefined Gestalt selector ", code32_t( selector );
			break;
	}
	
	s.sr.nzvc = result ? 0x8 : 0x4;  // either N or Z
	
	s.d(0) = result;
	s.a(0) = value;
	
	return rts;
}


static const function_type the_callouts[] =
{
	&no_op_callout,
	&load_callout,
	&enter_supervisor_mode_callout,
	&module_suspend_callout,
	
	&set_trace_mode_callout,
	&set_trace_mode_callout,
	&set_trace_mode_callout,
	&set_trace_mode_callout,
	
	NULL,
	&lock_screen_callout,
	&unlock_screen_callout,
	NULL,
	
	&alloc_callout,
	&dealloc_callout,
	NULL,
	NULL,
	
	&get_Ticks_callout,
	&get_microseconds_callout,
	&timeval_from_microseconds_callout,
	&timeval_from_nanoseconds_callout,
	
	&fast_memset_callout,
	&fast_memnot_callout,
	&fast_shift_callout,
	&fast_mempcpy_callout,
	
	&mem_test_callout,
	NULL,
	NULL,
	NULL,
	
	&fast_memcmp_callout,
	NULL,
	NULL,
	NULL,
	
	&plot_cursor_callout,
	&notify_cursor_moved_callout,
	&notify_cursor_vis_callout,
	&notify_cursor_set_callout,
	
	&blit_bytes_callout,
	&draw_bytes_callout,
	NULL,
	NULL,
	
	&send_sound_command_callout,
	NULL,
	NULL,
	NULL,
	
	&next_pseudorandom_callout,
	NULL,
	NULL,
	NULL,
	
	&fast_fix_mul_callout,
	&fast_fix_ratio_callout,
	&fast_fix_round_callout,
	&fast_fix_div_callout,
	
	&sect_rect_region_callout,
	NULL,
	NULL,
	NULL,
	
	&system_call_callout,
	&microseconds_callout,
	
	&bus_error_callout,
	&address_error_callout,
	&illegal_instruction_callout,
	&division_by_zero_callout,
	&chk_trap_callout,
	&trapv_trap_callout,
	&privilege_violation_callout,
	&trace_exception_callout,
	&line_A_emulator_callout,
	&line_F_emulator_callout,
	&format_error_callout,
	
	&sigint_interrupt_callout,
	
	&unimplemented_trap_callout,
	&FreeMem_callout,
	&BlockMove_callout,
	&Gestalt_callout,
	&unimplemented_callout
};


int32_t bridge( v68k::processor_state& s )
{
	const int32_t pc = s.pc();
	
	const uint32_t call_number = pc / -2 - 1;
	
	const size_t n_callouts = sizeof the_callouts / sizeof the_callouts[0];
	
	if ( call_number < n_callouts )
	{
		const function_type f = the_callouts[ call_number ];
		
		if ( f != NULL )
		{
			return f( s );
		}
	}
	
	return nil;
}

}  // namespace callout
}  // namespace v68k
