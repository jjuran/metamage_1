/*
	v68k-hello.cc
	-------------
*/

// Standard C
#include <errno.h>
#include <stdio.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// v68k
#include "v68k/endian.hh"
#include "v68k/emulator.hh"


#pragma exceptions off


using v68k::big_word;
using v68k::big_longword;


static void dump( const v68k::emulator& emu )
{
	const v68k::registers& regs = emu.regs;
	
	printf( "\n" );
	
	for ( int i = 0;  i < 8;  ++i )
	{
		printf( "D%d: %.8x   A%d: %.8x\n", i, regs.d[i], i, regs.a[i] );
	}
	
	printf( "\n" );
	
	printf( "Alt SP:  %.8x\n", regs.alt_sp  );
	printf( "Alt SSP: %.8x\n", regs.alt_ssp );
	
	printf( "\n" );
	
	printf( "PC: %.8x\n", regs.pc );
	printf( "SR: %.4x\n", emu.get_SR() );
	
	printf( "\n" );
}


const uint32_t code_max_size = 4096;

const uint32_t os_address   = 1024;
const uint32_t initial_USP  = 3072;
const uint32_t initial_SSP  = 4096;
const uint32_t code_address = 4096;

const uint32_t mem_size = 8192;

const uint32_t user_pb_addr   = 3072 +  0;  // 20 bytes
const uint32_t system_pb_addr = 3072 + 20;  // 20 bytes

static const uint16_t os[] =
{
	// Jump over handlers
	
	0x601A,  // BRA.S  *+28
	
	// Illegal Instruction,
	// Privilege Violation
	
	0x484F,  // BKPT  #7
	
	// Trap 15
	
	0x4E72,  // STOP #FFFF  ; finish
	0xFFFF,
	
	// Trap 0
	
	0x41EF,  // LEA  (2,A7),A0
	0x0002,
	
	0x5590,  // SUBQ.L  #2,(A0)
	
	0x2050,  // MOVEA.L  (A0),A0
	
	0x30BC,  // MOVE.W  #0x484A,(A0)
	0x484A,
	
	0x4E73,  // RTE
	
	// Line A Emulator
	
	0x54AF,  // ADDQ.L  #2,(2,A7)
	0x0002,
	
	0x4E73,  // RTE
	
	// OS resumes here
	
	0x027C,  // ANDI #DFFF,SR  ; clear S
	0xDFFF,
	
	0x4FF8,  // LEA  (3072).W,A7
	initial_USP,
	
	0x42B8,  // CLR.L  user_pb_addr + 2 * sizeof (uint32_t)  ; user_pb->errno_var
	user_pb_addr + 2 * sizeof (uint32_t),
	
	0x21FC,  // MOVE.L  #user_pb_addr,(system_pb_addr).W  ; pb->current_user
	0x0000,
	user_pb_addr,
	system_pb_addr,
	
	0x4878,  // PEA  (system_pb_addr).W  ; pb
	system_pb_addr,
	
	0x4878,  // PEA  (0).W  ; envp
	0x0000,
	
	0x4878,  // PEA  (0).W  ; argv
	0x0000,
	
	0x4878,  // PEA  (0).W  ; argc
	0x0000,
	
	0x4EB8,  // JSR  code_address
	code_address,
	
	0x4e4F   // TRAP  #15
};

const uint32_t bkpt_7_addr = os_address +  2;
const uint32_t finish_addr = os_address +  4;
const uint32_t trap_0_addr = os_address +  8;
const uint32_t line_A_addr = os_address + 22;

static void load_vectors( uint8_t* mem )
{
	uint32_t* vectors = (uint32_t*) mem;
	
	memset( vectors, 0xFF, 1024 );
	
	vectors[0] = big_longword( initial_SSP );  // isp
	vectors[1] = big_longword( os_address  );  // pc
	
	vectors[4] = big_longword( bkpt_7_addr );  // Illegal Instruction
	vectors[8] = big_longword( bkpt_7_addr );  // Privilege Violation
	
	vectors[10] = big_longword( line_A_addr );  // Line A Emulator
	
	vectors[32] = big_longword( trap_0_addr );  // Trap  0
	vectors[47] = big_longword( finish_addr );  // Trap 15
}

static void load_code( uint16_t*        dest,
                       const uint16_t*  begin,
                       const uint16_t*  end )
{
	while ( begin < end )
	{
		*dest++ = big_word( *begin++ );
	}
}

static inline void load_n_words( uint8_t*         mem,
                                 uint32_t         offset,
                                 const uint16_t*  begin,
                                 size_t           n_words )
{
	uint16_t* dest = (uint16_t*) (mem + offset);
	
	load_code( dest, begin, begin + n_words );
}

static bool get_stacked_args( const v68k::emulator& emu, uint32_t* out, int n )
{
	uint32_t sp = emu.regs.a[7];
	
	while ( n > 0 )
	{
		sp += 4;
		
		if ( !emu.mem.get_long( sp, *out, emu.data_space() ) )
		{
			return false;
		}
		
		++out;
		--n;
	}
	
	return true;
}

static void set_errno( v68k::emulator& emu )
{
	emu.regs.d[1] = errno;
	
	const uint32_t errno_ptr_addr = user_pb_addr + 2 * sizeof (uint32_t);
	
	uint32_t errno_ptr;
	
	if ( emu.mem.get_long( errno_ptr_addr, errno_ptr, emu.data_space() )  &&  errno_ptr != 0 )
	{
		emu.mem.put_long( errno_ptr, errno, emu.data_space() );
	}
}

static inline bool set_result( v68k::emulator& emu, int result )
{
	emu.regs.d[0] = result;
	
	if ( result < 0 )
	{
		set_errno( emu );
	}
	
	return true;
}

static bool emu_read( v68k::emulator& emu )
{
	uint32_t args[3];  // fd, buffer, length
	
	if ( !get_stacked_args( emu, args, 3 ) )
	{
		return emu.bus_error();
	}
	
	const int fd = int32_t( args[0] );
	
	const uint32_t buffer = args[1];
	
	const size_t length = args[2];
	
	uint8_t* p = emu.mem.translate( buffer, length, emu.data_space(), v68k::mem_write );
	
	int result;
	
	if ( p == NULL )
	{
		result = -1;
		
		errno = EFAULT;
	}
	else
	{
		result = read( fd, p, length );
	}
	
	return set_result( emu, result );
}

static bool emu_write( v68k::emulator& emu )
{
	uint32_t args[3];  // fd, buffer, length
	
	if ( !get_stacked_args( emu, args, 3 ) )
	{
		return emu.bus_error();
	}
	
	const int fd = int32_t( args[0] );
	
	const uint32_t buffer = args[1];
	
	const size_t length = args[2];
	
	const uint8_t* p = emu.mem.translate( buffer, length, emu.data_space(), v68k::mem_read );
	
	int result;
	
	if ( p == NULL )
	{
		result = -1;
		
		errno = EFAULT;
	}
	else
	{
		result = write( fd, p, length );
	}
	
	return set_result( emu, result );
}

static bool bridge_call( v68k::emulator& emu )
{
	const uint16_t call_number = emu.regs.d[0];
	
	switch ( call_number )
	{
		case 3:  return emu_read ( emu );
		case 4:  return emu_write( emu );
		
		default:
			return false;
	}
}

static int execute_68k( const char* path )
{
	uint8_t mem[ mem_size ];
	
	load_vectors( mem );
	load_n_words( mem, os_address, os, sizeof os / 2 );
	
	if ( path != NULL )
	{
		int fd = open( path, O_RDONLY );
		
		if ( fd >= 0 )
		{
			int n_read = read( fd, mem + code_address, code_max_size );
			
			fprintf( stderr, "Loaded %d bytes from %s\n", n_read, path );
			
			close( fd );
		}
	}
	
	v68k::emulator emu( v68k::mc68000, mem, sizeof mem );
	
	emu.reset();
	
step_loop:
	
	while ( emu.step() )
	{
		continue;
	}
	
	if ( emu.condition == v68k::bkpt_2 )
	{
		if ( bridge_call( emu ) )
		{
			emu.acknowledge_breakpoint( 0x4E75 );  // RTS
		}
		
		goto step_loop;
	}
	
	if ( emu.condition == v68k::finished )
	{
		return 0;
	}
	
	putchar( '\n' );
	
	const char* condition;
	
	switch ( emu.condition )
	{
		using namespace v68k;
		
		case halted:
			condition = "halted";
			break;
		
		case stopped:
			condition = "stopped";
			break;
		
		case bkpt_0:
		case bkpt_1:
		case bkpt_2:
		case bkpt_3:
		case bkpt_4:
		case bkpt_5:
		case bkpt_6:
		case bkpt_7:
			condition = NULL;
			
			printf( "Breakpoint %d\n", emu.condition - bkpt_0 );
			break;
		
		default:
			condition = "???";
			break;
	}
	
	if ( condition )
	{
		printf( "Processor %s\n", condition );
	}
	
	dump( emu );
	
	return 1;
}

int main( int argc, char** argv )
{
	if ( const char* path = argv[1] )
	{
		return execute_68k( path );
	}
	
	return 0;
}

