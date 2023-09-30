/*
	exceptions.hh
	-------------
*/

#ifndef CALLOUT_EXCEPTIONS_HH
#define CALLOUT_EXCEPTIONS_HH

// v68k
#include "v68k/state.hh"


namespace v68k    {
namespace callout {

int32_t unimplemented_trap_callout ( v68k::processor_state& s );
int32_t bus_error_callout          ( v68k::processor_state& s );
int32_t address_error_callout      ( v68k::processor_state& s );
int32_t illegal_instruction_callout( v68k::processor_state& s );
int32_t division_by_zero_callout   ( v68k::processor_state& s );
int32_t chk_trap_callout           ( v68k::processor_state& s );
int32_t trapv_trap_callout         ( v68k::processor_state& s );
int32_t privilege_violation_callout( v68k::processor_state& s );
int32_t trace_exception_callout    ( v68k::processor_state& s );
int32_t line_A_emulator_callout    ( v68k::processor_state& s );
int32_t line_F_emulator_callout    ( v68k::processor_state& s );
int32_t format_error_callout       ( v68k::processor_state& s );
int32_t sigint_interrupt_callout   ( v68k::processor_state& s );

}  // namespace callout
}  // namespace v68k


#endif
