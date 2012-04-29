/*	============
 *	Expansion.hh
 *	============
 */

#ifndef SHELLSHOCK_EXPANSION_HH
#define SHELLSHOCK_EXPANSION_HH

// Standard C++
#include <vector>

// plus
#include "plus/string.hh"

// sh
#include "Tokenization.hh"


namespace ShellShock
{
	
	typedef std::vector< plus::string > (*param_lookup_f)( const plus::string& param, bool double_quoted );
	
	
	std::vector< plus::string > BraceExpansion( const plus::string& word );
	
	plus::string TildeExpansion( const plus::string& word );
	
	std::vector< plus::string > ParameterExpansion( param_lookup_f       lookup_param,
	                                                const plus::string&  word );
	
	plus::string CommandSubstitution( const plus::string& word );
	plus::string ArithmeticExpansion( const plus::string& word );
	plus::string ProcessSubstitution( const plus::string& word );
	
	std::vector< plus::string > WordSplitting    ( const plus::string& word );
	std::vector< plus::string > PathnameExpansion( const plus::string& word );
	
	plus::string QuoteRemoval( const plus::string& word );
	
	Command ParseCommand( const Command& command, param_lookup_f lookup_param );
	
}

#endif

