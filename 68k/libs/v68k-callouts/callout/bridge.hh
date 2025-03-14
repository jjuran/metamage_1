/*
	bridge.hh
	---------
*/

#ifndef CALLOUT_BRIDGE_HH
#define CALLOUT_BRIDGE_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

enum
{
	// $FFFE - $FFF8
	no_op,
	load,
	enter_supervisor_mode,
	module_suspend,
	
	// $FFF6 - $FFF0
	trace_reserved,
	trace_all,
	trace_flow,
	trace_off,
	
	// $FFEE - $FFE8
	obsolete_ScrnBitMap_trap,
	lock_screen,
	unlock_screen,
	reserved1,
	
	// $FFE6 - $FFE0
	alloc,
	dealloc,
	reserved2,
	reserved3,
	
	// $FFDE - $FFD8
	get_Ticks,
	get_microseconds,  // returns in registers, for _Microseconds
	timeval_from_microseconds,
	timeval_from_nanoseconds,
	
	// $FFD6 - $FFD0
	fast_memset,
	fast_memnot,
	fast_shift,
	fast_mempcpy,
	
	// $FFCE - $FFC8
	mem_test,
	reserved7,
	reserved8,
	reserved9,
	
	// $FFC6 - $FFC0
	fast_memcmp,
	reserved10,
	reserved11,
	reserved12,
	
	// $FFBE - $FFB8
	plot_cursor,
	notify_cursor_moved,
	notify_cursor_vis,
	notify_cursor_set,
	
	// $FFB6 - $FFB0
	blit_bytes,
	draw_bytes,
	reserved14,
	reserved15,
	
	// $FFAE - $FFA8
	send_sound_command,
	reserved16,
	reserved17,
	reserved18,
	
	// $FFA6 - $FFA0
	next_pseudorandom,
	reserved19,
	reserved20,
	reserved21,
	
	// $FF9E - $FF98
	fast_fix_mul,
	fast_fix_ratio,
	fast_fix_round,
	fast_fix_div,
	
	// $FF96 - $FF90
	sect_rect_region,
	sect_regions,
	xor_regions,
	polygon_region,
	
	// $FF8E - $FF88
	finish_region,
	reserved22,
	reserved23,
	reserved24,
	
	// $FF86 - $FF80
	transcode_8x_1bpp_to_8bpp,
	reserved25,
	reserved26,
	reserved27,
	
	system_call,
	microseconds,  // returns on stack, for MacRelix tool exec parameter block
	
	bus_error,
	address_error,
	illegal_instruction,
	division_by_zero,
	chk_trap,
	trapv_trap,
	privilege_violation,
	trace_exception,
	line_A_emulator,
	line_F_emulator,
	format_error,
	
	sigint_interrupt,
	
	unimplemented_trap,
	FreeMem_trap,
	BlockMove_trap,
	Gestalt_trap,
	unimplemented,
	n
};

inline
uint32_t callout_address( int index )
{
	return uint32_t( (index + 1) * -2 );
}

int32_t bridge( v68k::processor_state& emu );

}  // namespace callout
}  // namespace v68k


#endif
