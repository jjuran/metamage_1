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

// ShellShock
#include "ShellShock/Tokenization.hh"


namespace ShellShock
{
	
	class ParameterDictionary
	{
		public:
			virtual std::vector< plus::string > Lookup( const plus::string& param, bool double_quoted ) const = 0;
	};
	
	
	std::vector< plus::string > BraceExpansion( const plus::string& word );
	
	plus::string TildeExpansion( const plus::string& word );
	
	std::vector< plus::string > ParameterExpansion( const ParameterDictionary*  dictionary,
	                                                const plus::string&         word );
	
	plus::string CommandSubstitution( const plus::string& word );
	plus::string ArithmeticExpansion( const plus::string& word );
	plus::string ProcessSubstitution( const plus::string& word );
	
	std::vector< plus::string > WordSplitting    ( const plus::string& word );
	std::vector< plus::string > PathnameExpansion( const plus::string& word );
	
	plus::string QuoteRemoval( const plus::string& word );
	
	Command ParseCommand( const Command&              command,
	                      const ParameterDictionary&  dictionary );
	
}

#endif

