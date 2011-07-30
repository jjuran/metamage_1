/*
	line_4.hh
	---------
*/

#ifndef V68K_LINE4_HH
#define V68K_LINE4_HH


namespace v68k
{
	
	struct instruction;
	
	struct registers;
	
	class memory;
	
	
	const instruction* decode_line_4( const registers& regs, const memory& mem );
	
}

#endif

