/*
	trap_dispatcher.cc
	------------------
*/

#include "v68k-mac/trap_dispatcher.hh"


namespace v68k {
namespace mac {

const uint16_t trap_dispatcher[ trap_dispatcher_word_count ] =
{
	
// Line A exception handler:
	
	0x2F08,  // MOVE.L   A0,-(A7)           ; save A0
	
	0x4E68,  // MOVE     USP,A0
	0x5988,  // SUBQ.L   #4,A0              ; extend user stack
	0x4E60,  // MOVE     A0,USP
	
	0x20AF,  // MOVEA.L  (6,A7),(A0)        ; push caller PC onto user stack
	0x0006,
	
	0x41FA,  // LEA      dispatcher,A0
	0x000A,
	0x2F48,  // MOVE.L   A0,(6,A7)          ; set return address to dispatcher
	0x0006,
	
	0x205F,  // MOVEA.L  (A7)+,A0           ; restore A0
	0x4E73,  // RTE
	
// User mode trap dispatcher:
	
	0x48E7,  // MOVEM.L  D0-D2/A0,-(A7)     ; save regs + space for trap addr
	0xE080,
	
	0x206F,  // MOVEA.L  (16,A7),A0         ; load caller PC
	0x0010,
	0x3218,  // MOVE.W   (A0)+,D1           ; move trap word to D1 and skip it
	0x2F48,  // MOVE.L   A0,(16,A7)         ; update caller PC
	0x0010,
	
	0x3401,  // MOVE.W   D1,D2              ; copy trap word
	
	0x0C41,  // CMPI.W   #0xA800,D1         ; OS or Toolbox trap?
	0xA800,
	0x6D1E,  // BLT.S    os_trap
	
	0x307C,  // MOVEA.W  #3072,A0           ; point A0 to Toolbox trap table
	0x0C00,
	
	0x0242,  // ANDI.W   #0x03FF,D2         ; index into Toolbox trap table
	0x03FF,
	
	0xE54A,  // LSL.W    #2,D2
	
	0x2F70,  // MOVE.L   (A0,D2.W),(12,A7)  ; overwrite A1 slot with trap addr
	0x2000,
	0x000C,
	
	0x0C41,  // CMPI.W   #0xAC00,D1         ; auto-pop?
	0xAC00,
	
	0x4CDF,  // MOVEM.L  (A7)+,D1-D2/A0     ; pop stack without touching CCR
	0x0106,
	
	0x6D02,  // BLT.S    nopop
	
	0x2E5F,  // MOVE.L   (A7)+,(A7)         ; remove caller's return address
	
// nopop:
	0x4E75,  // RTS                         ; 'return' to Toolbox trap routine
	
// os_trap:
	0x2F09,  // MOVE.L   A1,-(A7)           ; save A1 for OS traps
	
	0x307C,  // MOVEA.W  #1024,A0           ; point A0 to OS trap table
	0x0400,
	
	0x0242,  // ANDI.W   #0x01FF,D2         ; index into OS trap table
	0x01FF,
	
	0xE54A,  // LSL.W    #2,D2
	
	0x0882,  // BCLR     #10,D2             ; clear and test keep-A0 bit, 8<<2
	0x000A,
	
	0x2070,  // MOVEA.L  (A0,D2.W),A0       ; move trap address to A0
	0x2000,
	
	0x6708,  // BEQ.S    restore_a0         ; if keep-A0 is clear, restore A0
	
	0x4E90,  // JSR  (A0)                   ; call OS trap routine
	
	0x2F48,  // MOVE.L   A0,(4,A7)          ; overwrite saved A0 with new A0
	0x0004,
	
	0x6002,  // BRA.S    end
	
// restore_a0:
	0x4E90,  // JSR  (A0)                   ; call OS trap routine
	
// end:
	0x4CDF,  // MOVEM.L  (A7)+,D1-D2/A0-A1  ; restore regs
	0x0306,  // 
	0x588F,  // ADDQ.L   #4,A7              ; pop saved D0
	
	0x4E75   // RTS                         ; return to caller
};

}  // namespace user
}  // namespace v68k

