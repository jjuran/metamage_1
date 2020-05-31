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


#pragma exceptions off


#define PROMPT "vdb> "

#define PRINT( s )  write( STDERR_FILENO, s "\n", sizeof s )


bool continue_on_bus_error;

static unsigned n_steps;

static uint32_t step_over;

static const char* causes[] =
{
	"User Break",
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

class NMI_reentry_guard
{
	private:
		static volatile int level;
		
		// non-copyable
		NMI_reentry_guard           ( const NMI_reentry_guard& );
		NMI_reentry_guard& operator=( const NMI_reentry_guard& );
	
	public:
		NMI_reentry_guard()  { ++level; }
		~NMI_reentry_guard()  { --level; }
		
		static bool reentered()  { return level; }
		
};

volatile int NMI_reentry_guard::level;

static void debugger_loop( registers& regs )
{
	const uint16_t buserr_offset =  2 * sizeof (uint32_t);
	const uint16_t trace_offset  =  9 * sizeof (uint32_t);
	const uint16_t sigint_offset = 66 * sizeof (uint32_t);
	
	const uint16_t vector_offset = regs.fv & 0x03FF;
	
	if ( NMI_reentry_guard::reentered()  &&  vector_offset == sigint_offset )
	{
		return;
	}
	
	NMI_reentry_guard guard;
	
	if ( vector_offset != trace_offset )
	{
		n_steps   = 0;
		step_over = 0;
		
		report_exception( vector_offset );
	}
	
	if ( vector_offset == buserr_offset  &&  continue_on_bus_error )
	{
		regs.sr &= 0x3FFF;  // clear Trace bits
		return;
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
		regs.sr |= 0x8000;  // set T1 bit (since neither Trace bit is set)
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
			
			// Trying to read() our next user command failed.  Just bail out.
			
			_exit( 1 );
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
			
			case 'x':
				_exit(0);
			
			default:
				break;
		}
	}
}

static
asm void debugger_break()
{
	MOVEM.L  D0-D7/A0-A7,-(SP)
	
	MOVE     SR,D3        // S bit is set if we're in Supervisor mode
	NOT.W    D3           // S bit is clear if we're in Supervisor mode
	OR.W     64(SP),D3    // S bit is clear if we went from User -> Supervisor
	BTST     #13,D3       // Test the S bit
	BNE.S    no_USP_load  // no mode switch
	
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
	
	TST.W    6(SP)  // test the format/vector word
	BEQ.S    return_to_user
	
	RTE
	
return_to_user:
	MOVE     (SP)+,CCR   // restore the CCR
	MOVE.L   (SP),6(SP)  // copy the PC to the return address
	ADDQ.L   #6,SP       // pop the PC and format/vector word off the stack
	
	RTS
}

asm void user_break()
{
	// return address is on top of stack
	
	CLR.W    -(SP)        // vector 0 shall mean a user break
	MOVE.L   2(SP),-(SP)  // push the PC again
	MOVE     SR,-(SP)     // push the SR
	
	JMP  debugger_break
}

asm int set_trace_handler()
{
	JSR     0xFFFFFFFA ;  // enter_supervisor_mode()
	
	BMI.S   bail ;  // D0 is -1 if branch taken
	
	LEA     debugger_break,A0
	
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
