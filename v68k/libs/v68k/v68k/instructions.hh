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
	
	extern instruction decoded_LINK_L;
	
	extern instruction decoded_MOVES;
	
	extern instruction decoded_MOVE_B;
	
	extern instruction decoded_MOVE_L;
	
	extern instruction decoded_MOVE_W;
	
	extern instruction decoded_LEA;
	
	extern instruction decoded_MOVE_to_CCR;
	
	extern instruction decoded_MOVE_to_SR;
	
	extern instruction decoded_PEA;
	
	extern instruction decoded_EXTB;
	
	extern instruction decoded_TRAP;
	
	extern instruction decoded_LINK;
	extern instruction decoded_UNLK;
	
	extern instruction decoded_MOVE_to_USP;
	extern instruction decoded_MOVE_from_USP;
	
	extern instruction decoded_RESET;
	
	extern instruction decoded_NOP;
	
	extern instruction decoded_STOP;
	
	extern instruction decoded_RTE;
	
	extern instruction decoded_RTD;
	
	extern instruction decoded_RTS;
	
	extern instruction decoded_TRAPV;
	
	extern instruction decoded_RTR;
	
	extern instruction decoded_MOVEC;
	
	extern instruction decoded_MOVEQ;
	
	extern instruction decoded_EXG;
	
}

#endif
