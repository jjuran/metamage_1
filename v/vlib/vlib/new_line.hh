/*
	new_line.hh
	-----------
*/

#ifndef VLIB_NEWLINE_HH
#define VLIB_NEWLINE_HH


namespace vlib
{
	
	struct Token;
	
	bool new_line_continues( const Token& token );
	
}

#endif
