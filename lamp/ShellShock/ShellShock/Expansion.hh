/*	============
 *	Expansion.hh
 *	============
 */

#ifndef SHELLSHOCK_EXPANSION_HH
#define SHELLSHOCK_EXPANSION_HH

// Standard C++
#include <string>
#include <vector>

// ShellShock
#include "ShellShock/Tokenization.hh"


namespace ShellShock
{
	
	class ParameterDictionary
	{
		public:
			virtual std::vector< std::string > Lookup( const std::string& param, bool double_quoted ) const = 0;
	};
	
	
	std::vector< std::string > BraceExpansion( const std::string& word );
	
	std::string TildeExpansion( const std::string& word );
	
	std::vector< std::string > ParameterExpansion( const ParameterDictionary*  dictionary,
	                                               const std::string&          word );
	
	std::string CommandSubstitution( const std::string& word );
	std::string ArithmeticExpansion( const std::string& word );
	std::string ProcessSubstitution( const std::string& word );
	
	std::vector< std::string > WordSplitting    ( const std::string& word );
	std::vector< std::string > PathnameExpansion( const std::string& word );
	
	std::string QuoteRemoval( const std::string& word );
	
	Command ParseCommand( const Command&              command,
	                      const ParameterDictionary&  dictionary );
	
}

#endif

