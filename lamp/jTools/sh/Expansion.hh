/*	============
 *	Expansion.hh
 *	============
 */

#ifndef SHELLSHOCK_EXPANSION_HH
#define SHELLSHOCK_EXPANSION_HH

// plus
#include "plus/string.hh"

// sh
#include "Tokenization.hh"


namespace ShellShock
{
	
	typedef StringVector (*param_lookup_f)( const plus::string& param, bool double_quoted );
	
	
	StringVector BraceExpansion( const plus::string& word );
	
	plus::string TildeExpansion( const plus::string& word );
	
	StringVector ParameterExpansion( param_lookup_f       lookup_param,
	                                 const plus::string&  word );
	
	plus::string CommandSubstitution( const plus::string& word );
	plus::string ArithmeticExpansion( const plus::string& word );
	plus::string ProcessSubstitution( const plus::string& word );
	
	StringVector WordSplitting    ( const plus::string& word );
	StringVector PathnameExpansion( const plus::string& word );
	
	plus::string QuoteRemoval( const plus::string& word );
	
	Command ParseCommand( const Command& command, param_lookup_f lookup_param );
	
}

#endif
