/*
	instructions.hh
	---------------
*/

#ifndef V68K_INSTRUCTIONS_HH
#define V68K_INSTRUCTIONS_HH

// v68k
#include "v68k/instruction.hh"


namespace v68k
{
	
	extern instruction decoded_MOVEP_to;
	extern instruction decoded_MOVEP_from;
	
	extern instruction decoded_LINK_L;
	
	extern instruction decoded_MOVE_B_to_Dn;
	extern instruction decoded_MOVE_B;
	
	extern instruction decoded_MOVE_L_to_Dn;
	extern instruction decoded_MOVEA_L;
	extern instruction decoded_MOVE_L;
	
	extern instruction decoded_MOVE_W_to_Dn;
	extern instruction decoded_MOVEA_W;
	extern instruction decoded_MOVE_W;
	
	extern instruction decoded_LEA;
	
	extern instruction decoded_BKPT;
	
	extern instruction decoded_PEA;
	
	extern instruction decoded_EXT_W;
	extern instruction decoded_EXT_L;
	
	extern instruction decoded_EXTB;
	
	extern instruction decoded_LINK;
	extern instruction decoded_UNLK;
	
	extern instruction decoded_MOVE_to_USP;
	extern instruction decoded_MOVE_from_USP;
	
	extern instruction decoded_NOP;
	
	extern instruction decoded_STOP;
	
	extern instruction decoded_RTE;
	
	extern instruction decoded_RTS;
	
	extern instruction decoded_JSR;
	
	extern instruction decoded_JMP;
	
	extern instruction decoded_BRA_S;
	extern instruction decoded_BRA;
	extern instruction decoded_BRA_L;
	
	extern instruction decoded_BSR_S;
	extern instruction decoded_BSR;
	extern instruction decoded_BSR_L;
	
	extern instruction decoded_Bcc_S;
	extern instruction decoded_Bcc;
	extern instruction decoded_Bcc_L;
	
	extern instruction decoded_MOVEQ;
	
	extern instruction decoded_EXG;
	
}

#endif

