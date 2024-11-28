/*
	load.cc
	-------
*/

#include "os/load.hh"

// Standard C
#include <string.h>

// v68k
#include "v68k/endian.hh"

// v68k-user
#include "v68k-user/load.hh"

// v68k-mac
#include "v68k-mac/trap_dispatcher.hh"

// v68k-callouts
#include "callout/bridge.hh"

// v68k-syscalls
#include "syscall/handler.hh"

// v68k-exec
#include "exec/load.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k {
namespace os   {

using v68k::callout::system_call;
using v68k::callout::microseconds;

using v68k::exec::code_address;
using v68k::exec::params_addr;

using v68k::exec::argc_addr;
using v68k::exec::argv_addr;

enum
{
	os_address  = 7168,
	initial_SSP = 3072,
	stack_limit = 10240,
	initial_USP = 30720,
	
	user_pb_addr   = params_addr +  0,  // 20 bytes
	system_pb_addr = params_addr + 20,  // 20 bytes
	
	user_pb_stack_bottom_addr = user_pb_addr + 0,
	user_pb_stack_limit_addr  = user_pb_addr + 4,
};

static
const uint16_t loader_code[] =
{
	0x027C,  // ANDI #0xD8FF,SR  ; clear S and the interrupt mask
	0xD8FF,
	
	0x4FF8,  // LEA  (3072).W,A7
	initial_USP,
	
	0x4BF8,  // LEA  (12288).W,A5
	0x3000,
	
	0x21CF,  // MOVE.L   A7,(user_pb_stack_bottom_addr).W
	user_pb_stack_bottom_addr,
	
	0x21FC,  // MOVE.L  #stack_limit,(user_pb_stack_limit_addr).W
	0x0000,
	stack_limit,
	user_pb_stack_limit_addr,
	
	0x42B8,  // CLR.L  user_pb_addr + 2 * sizeof (u32)  ; user_pb->errno_var
	user_pb_addr + 2 * sizeof (uint32_t),
	
	0x21FC,  // MOVE.L  #user_pb_addr,(system_pb_addr).W  ; pb->current_user
	0x0000,
	user_pb_addr,
	system_pb_addr,
	
	0x21FC,  // MOVE.L   #callout_address(system_call),<dispatcher field>
	0xFFFF,
	(uint16_t) callout_address(system_call),
	system_pb_addr + 3 * sizeof (uint32_t),
	
	0x21FC,  // MOVE.L   #callout_address(microseconds),<microseconds field>
	0xFFFF,
	(uint16_t) callout_address(microseconds),
	system_pb_addr + 4 * sizeof (uint32_t),
	
	0x4878,  // PEA  (system_pb_addr).W  ; pb
	system_pb_addr,
	
	0x4878,  // PEA  (0).W  ; envp
	0x0000,
	
	0x2F38,  // MOVE.L  (argv_addr).W,-(A7)
	argv_addr,
	
	0x2F38,  // MOVE.L  (argc_addr).W,-(A7)
	argc_addr,
	
	0x4EB8,  // JSR  code_address
	code_address,
	
	0x2F00,  // MOVE.L  D0,-(A7)
	0x42A7,  // CLR.L   -(A7)  ; zero return address
	0x7001,  // MOVEQ  #1,D0
	0x484A,  // BKPT   #2
	
	// Not reached
	0x4E75   // RTS
};

static
const uint16_t no_op_exception[] =
{
	0x4E73,  // RTE
};

#define HANDLER( handler )  handler, sizeof handler

void load( uint8_t* mem, uint32_t mem_size )
{
	v68k::user::os_load_spec os = { mem, mem_size, os_address };
	
	uint32_t* vectors = (uint32_t*) mem;
	
	memset( vectors, 0xFF, 1024 );
	
	vectors[0] = big_longword( initial_SSP );  // isp
	
	using v68k::user::install_exception_handler;
	using v68k::mac::trap_dispatcher;
	
	install_exception_handler( os,  1, HANDLER( loader_code ) );
	install_exception_handler( os, 10, HANDLER( trap_dispatcher ) );
	install_exception_handler( os, 32 + 2, HANDLER( syscall_patch_handler ) );
	
	install_exception_handler( os, 64, HANDLER( no_op_exception ) );
	install_exception_handler( os, 88, HANDLER( no_op_exception ) );
	
	using namespace v68k::callout;
	
	vectors[ 2] = big_longword( callout_address( bus_error           ) );
	vectors[ 3] = big_longword( callout_address( address_error       ) );
	vectors[ 4] = big_longword( callout_address( illegal_instruction ) );
	vectors[ 5] = big_longword( callout_address( division_by_zero    ) );
	vectors[ 6] = big_longword( callout_address( chk_trap            ) );
	vectors[ 7] = big_longword( callout_address( trapv_trap          ) );
	vectors[ 8] = big_longword( callout_address( privilege_violation ) );
	vectors[ 9] = big_longword( callout_address( trace_exception     ) );
	vectors[11] = big_longword( callout_address( line_F_emulator     ) );
	vectors[14] = big_longword( callout_address( format_error        ) );
	
	vectors[66] = big_longword( callout_address( sigint_interrupt    ) );
}

}  // namespace os
}  // namespace v68k
