/*
	trace.cc
	--------
*/

#include "trace.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <errno.h>

// gear
#include "gear/parse_decimal.hh"

// vdb
#include "print_registers.hh"


#define PROMPT "vdb> "

#define TROFF_WARNING "Warning:  Tracing is off.  Enter 't' to reenable."

#define PRINT( s )  write( STDERR_FILENO, s "\n", sizeof s )


static unsigned n_steps;

static uint32_t step_over;

static const char* causes[] =
{
	NULL,  // Reset SP (this is a stack pointer)
	NULL,  // Reset PC (would be overridden by actual Reset exception)
	"Bus Error",
	"Address Error",
	"Illegal Instruction",
	"Division By Zero",
	"CHK Exception",
	"TRAPV Exception",
	"Privilege Violation",
	NULL,  // Trace Exception (implied)
	NULL,  // Line 1010 (reserved for trap dispatcher)
	"Line 1111",
	NULL,  // unassigned, reserved
	"Coprocessor Protocol Violation",
	"Format Error",
};

static const char* interrupts[] =
{
	NULL,  // Reserved (there is no signal 0)
	NULL,  // SIGHUP
	"SIGINT",
};

static
void report_exception( uint16_t vector_offset )
{
	const char* cause = NULL;
	
	const uint8_t vector_number = vector_offset >> 2;
	
	if ( vector_number >= 64 )
	{
		const uint8_t index = vector_number - 64;
		
		if ( index < sizeof interrupts / sizeof *interrupts )
		{
			cause = interrupts[ index ];
		}
		
		if ( cause == NULL )
		{
			cause = "unspecified interrupt";
		}
	}
	
	if ( vector_number < sizeof causes / sizeof *causes )
	{
		cause = causes[ vector_number ];
	}
	
	if ( cause == NULL )
	{
		cause = "unknown exception";
	}
	
	write( STDERR_FILENO, "\n", 1 );
	write( STDERR_FILENO, cause, strlen( cause ) );
	write( STDERR_FILENO, "\n\n", 2 );
}

static void debugger_loop( registers& regs )
{
	const uint16_t trace_offset = 9 * sizeof (uint32_t);
	
	const uint16_t vector_offset = regs.fv & 0x03FF;
	
	if ( vector_offset != trace_offset )
	{
		report_exception( vector_offset );
	}
	
	if ( n_steps > 0 )
	{
		--n_steps;
		
		return;
	}
	
	if ( step_over )
	{
		if ( regs.pc < step_over  ||  regs.pc > step_over + 22 )
		{
			return;
		}
		
		step_over = 0;
	}
	
	if ( (regs.sr & 0xC000) == 0 )
	{
		PRINT( TROFF_WARNING );
	}
	
	char buffer[ 16 ];
	
	for ( ;; )
	{
		write( STDERR_FILENO, PROMPT, sizeof PROMPT - 1 );
		
		ssize_t n_read = read( STDIN_FILENO, buffer, sizeof buffer );
		
		int local_errno = 0;
		
		asm
		{
			MOVE.L   D1,local_errno
		}
		
		if ( n_read <= 0 )
		{
			if ( n_read < 0  &&  local_errno == EINTR )
			{
				PRINT( "" );
				continue;
			}
			
			// FIXME
			break;
		}
		
		buffer[ n_read - 1 ] = '\0';
		
		const char* p = buffer;
		
		switch ( *p )
		{
			case 'c':
			case 'g':
				regs.sr &= 0x3FFF;  // clear Trace bits
				return;
			
			case 's':
				n_steps = 0;
				
				{
					while ( *++p != '\0' )  if ( *p != ' ' )  break;
					
					const char* q = p;
					
					const unsigned steps = gear::parse_unsigned_decimal( &q );
					
					if ( q != p  &&  steps > 0 )
					{
						n_steps = steps - 1;
					}
				}
				
				return;
			
			case 'o':
				step_over = regs.pc;
				return;
			
			case 'r':
				print_registers( regs );
				break;
			
			case 't':
				regs.sr |= 0x8000;
				break;
			
			case 'x':
				_exit(0);
			
			default:
				break;
		}
	}
}

static asm void trace_handler()
{
	MOVEM.L  D0-D7/A0-A7,-(SP)
	
	MOVE.W   #0x2000,D3
	AND.W    64(SP),D3
	BNE.S    no_USP_load
	
	MOVE     USP,A0
	MOVE.L   A0,60(SP)  // copy user SP to registers
	
no_USP_load:
	
	MOVE.L  SP,-(SP)
	
	JSR  debugger_loop
	
	ADDQ.L   #4,SP
	
	TST.W    D3
	BNE.S    no_USP_store
	
	MOVEA.L  60(SP),A0  // update USP
	MOVE     A0,USP
	
no_USP_store:
	
	MOVEM.L  (SP)+,D0-D7/A0-A6  // update registers (not A7)
	
	ADDQ.L   #4,SP
	
	RTE
}

asm int set_trace_handler()
{
	JSR     0xFFFFFFFA ;  // enter_supervisor_mode()
	
	BMI.S   bail ;  // D0 is -1 if branch taken
	
	LEA     trace_handler,A0
	
	MOVE.L  A0,0x0008  // Bus Error
	MOVE.L  A0,0x000C  // Address Error
	MOVE.L  A0,0x0010  // Illegal Instruction
	MOVE.L  A0,0x0014  // Division By Zero
	MOVE.L  A0,0x0018  // CHK
	MOVE.L  A0,0x001C  // TRAPV
	MOVE.L  A0,0x0020  // Privilege Violation
	MOVE.L  A0,0x0024  // Trace
//	MOVE.L  A0,0x0028  // Line 1010
	MOVE.L  A0,0x002C  // Line 1111
	MOVE.L  A0,0x0030  // unassigned, reserved
	MOVE.L  A0,0x0034  // Coprocessor Protocol Violation
	MOVE.L  A0,0x0038  // Format Error
	
	MOVE.L  A0,0x0108  // SIGINT interrupt
	
	MOVE    D0,SR
	MOVEQ   #0,D0
	
bail:
	RTS
}
