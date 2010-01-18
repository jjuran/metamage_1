/*	============
 *	Expansion.cc
 *	============
 */

#include "ShellShock/Expansion.hh"

// Standard C/C++
#include <cctype>
#include <cstring>

// Standard C++
#include <functional>

// POSIX
#include "dirent.h"

// Debug
#include "debug/assert.hh"

// ShellShock
#include "ShellShock/PointerToFunction.h"


namespace ShellShock
{
	
	namespace N = Nitrogen;
	
	static bool IsAShellQuoteChar( char c )
	{
		switch ( c )
		{
			case '\\':  // backslash
			case '\'':  // single quote
			case '"':   // double quote
				return true;
				break;
			
			default:
				break;
		}
		
		return false;
	}
	
	static const char* SkipPastNextSingleQuote( const char* str )
	{
		const char* p = str;
		p++;
		
		while ( *p != '\0' && *p != '\'' )
		{
			p++;
		}
		
		if ( *p != '\0' )
		{
			p++;
		}
		
		return p;
	}
	
	static const char* SkipPastNextDoubleQuote( const char* str )
	{
		const char* p = str;
		p++;
		
		while ( *p != '\0' && *p != '"' )
		{
			if ( *p == '\\' )
			{
				p++;
				
				if ( *p != '\0' )
				{
					p++;
				}
			}
			else
			{
				p++;
			}
		}
		
		if ( *p != '\0' )
		{
			p++;
		}
		
		return p;
	}
	
	static const char* ScanForChar( char c, const char* str )
	{
		const char* p = str;
		
		while ( *p != '\0'  &&  *p != c )
		{
			switch (*p)
			{
				case '\\':
					p++;
					
					if ( *p != '\0' )
					{
						p++;
					}
					break;
				
				case '\'':
					p = SkipPastNextSingleQuote( p );
					break;
				
				case '"':
					p = SkipPastNextDoubleQuote( p );
					break;
				
				default:
					p++;
					break;
			}
		}
		
		return p;
	}
	
	static const char* ScanForDollar( const char* str, bool& double_quoted )
	{
		const char* p = str;
		const char c = '$';
		
		while ( *p != '\0'  &&  *p != c )
		{
			switch (*p)
			{
				case '\\':
					p++;
					
					if ( *p != '\0' )
					{
						p++;
					}
					break;
				
				case '"':
					double_quoted = !double_quoted;
					++p;
					break;
				
				case '\'':
					if ( !double_quoted )
					{
						p = SkipPastNextSingleQuote( p );
						break;
					}
					
					// fall through
				
				default:
					p++;
					break;
			}
		}
		
		return p;
	}
	
	static const char* ScanBracedGroup( const char* str )
	{
		const char *left, *right, *comma;
		int level = 1;
		
		const char* p = str;
		
		while ( *p != '\0'  &&  level > 0 )
		{
			left  = ScanForChar( '{', p );
			right = ScanForChar( '}', p );
			
			if ( left < right )
			{
				p = left;
				level++;
			}
			else
			{
				ASSERT( level > 0 );
				p = right;
				level--;
			}
			
			if ( level > 0  &&  *p != '\0' )
			{
				p++;
			}
		}
		
		comma = ScanForChar( ',', str );
		
		if ( p < comma )
		{
			// No comma in this group.
			// Set p to end-of-string to signal no expansion.
			p = ScanForChar( '\0', comma );
		}
		
		return p;
	}
	
	static std::vector< std::string > SplitBracedGroup( const char* str )
	{
		const char *p, *q;
		const char *comma, *left, *right;
		int level = 0;
		
		std::vector< std::string > words;
		
		p = q = str;
		
		while ( *q != '\0' )
		{
			if ( level == 0 )
			{
				comma = ScanForChar( ',', q );
				left  = ScanForChar( '{', q );
				
				if ( comma < left )
				{
					words.push_back( std::string( p, comma - p ) );
					p = q = comma + 1;
				}
				else if ( left < comma )
				{
					q = left + 1;
					level++;
				}
				else
				{
					q = left;  // end of string
				}
			}
			else
			{
				left  = ScanForChar( '{', q );
				right = ScanForChar( '}', q );
				
				if ( left < right )
				{
					q = left + 1;
					level++;
				}
				else if ( right < left )
				{
					ASSERT( level > 0 );
					q = right + 1;
					level--;
				}
				else
				{
					// This shouldn't happen 
					ASSERT( "Not reached" == 0 );
				}
			}
		}
		words.push_back( std::string( p ) );
		
		return words;
	}
	
	std::vector< std::string > BraceExpansion( const std::string& word )
	{
		// egg-{foo,bar,baz}-yung -> egg-foo-yung egg-bar-yung egg-baz-yung
		
		const char *p, *q;
		const char* left;
		
		// Start p pointing at the beginning of the word.
		p = word.c_str();
		
		// Scan for a left-brace.
		left = ScanForChar( '{', p );
		
		// If there's no left-brace, return the word as is.  (No expansion.)
		if ( *left == '\0' )
		{
			return std::vector< std::string >( 1, word );
		}
		
		// Copy from the beginning of the word up to the left-brace.
		// That's the preamble.
		std::string preamble( p, left - p );  // "egg-"
		
		// Point p to the character following the left-brace.
		p = left + 1;
		
		// Scan for the matching right-brace.
		q = ScanBracedGroup( p );  // "}-yung"
		
		// If there's no right-brace, return the word as is.  (No expansion.)
		if ( *q == '\0' )
		{
			return std::vector< std::string >( 1, word );
		}
		
		// Copy from after the left-brace up to the right-brace.
		// That's the brace group.
		std::string group( p, q - p );  // "foo,bar,baz"
		
		// Copy from after the right-brace to the end of the word.
		// That's the postscript.
		std::string postscript( q + 1 );  // "-yung"
		
		// Recursively expand the postscript, as it may contain more brace groups.
		std::vector< std::string > posts = BraceExpansion( postscript );
		
		// Split the comma-separated brace group.
		std::vector< std::string > words = SplitBracedGroup( group.c_str() );
		std::vector< std::string > expansion;  // The complete expansion
		
		typedef std::vector< std::string >::const_iterator const_iterator;
		
		// For each word in the first brace group:
		for ( const_iterator word = words.begin();  word < words.end();  ++word )
		{
			// Recursively expand the word.  (Expand subgroups.)
			std::vector< std::string > subs = BraceExpansion( *word );
			
			// For each expanded word:
			for ( const_iterator sub = subs.begin();  sub < subs.end();  ++sub )
			{
				// For each expanded postscript:
				for ( const_iterator post = posts.begin();  post < posts.end();  ++post )
				{
					expansion.push_back( preamble + *sub + *post );
				}
			}
		}
		
		return expansion;
	}
	
	std::string TildeExpansion( const std::string& word )
	{
		// ~jjuran -> /home/jjuran
		
		return word;
	}
	
	std::string CommandSubstitution( const std::string& word )
	{
		// $(hostname) -> toast.metamage.pdq
		
		return word;
	}
	
	std::string ArithmeticExpansion( const std::string& word )
	{
		// $((expression))
		
		return word;
	}
	
	std::string ProcessSubstitution( const std::string& word )
	{
		// <(log to /dev/fd/n) -> /dev/fd/n
		
		return word;
	}
	
	std::vector< std::string > WordSplitting( const std::string& word )
	{
		std::vector< std::string > vec;
		
		vec.push_back( word );
		
		return vec;
	}
	
	static bool NameMatchesPattern( const char* name, const char* pattern )
	{
		while ( *name && *pattern && *pattern != '*' )
		{
			// Breaks on ? or [
			if ( *name != *pattern  )
			{
				return false;
			}
			
			++name;
			++pattern;
		}
		
		if ( pattern[0] == '\0' || pattern[1] == '\0' )
		{
			return true;
		}
		
		const char* name_end    = name    + std::strlen( name    ) - 1;
		const char* pattern_end = pattern + std::strlen( pattern ) - 1;
		
		while ( name_end >= name && pattern_end >= pattern && *pattern_end != '*' )
		{
			// Breaks on ? or [
			if ( *name_end != *pattern_end  )
			{
				return false;
			}
			
			--name_end;
			--pattern_end;
		}
		
		if ( pattern == pattern_end )
		{
			// one char left, and must be *
			return true;
		}
		
		// Assume no match
		return false;
	}
	
	static void MatchPathnames( const std::string& in_dir, const char* pattern, std::vector< std::string >& result )
	{
		const char* dir_pathname = in_dir.empty() ? "." : in_dir.c_str();
		
		DIR* dir = opendir( dir_pathname );
		
		if ( dir == NULL )
		{
			//P7::ThrowErrno( ENOENT );
			return;
		}
		
		bool match_dotfiles = pattern[0] == '.';
		
		while ( dirent* entry = readdir( dir ) )
		{
			const char* name = entry->d_name;
			
			bool matched = (match_dotfiles || name[0] != '.')  &&  NameMatchesPattern( name, pattern );
			
			if ( matched )
			{
				result.push_back( in_dir + name );
			}
		}
		
		closedir( dir );
	}
	
	static void ExpandPathnames( const std::string& from_dir, const char* path, std::vector< std::string >& result )
	{
		//while ( *path == '/' ) continue;  // e.g. /foo//bar
		
		bool have_meta = false;
		const char* slash = NULL;
		
		const char* p = path;
		
		for ( p = path;  slash == NULL && *p != '\0';  ++p )
		{
			switch ( *p )
			{
				case '/':
					slash = p;
					break;
				case '*':  // fall through
				case '?':  // fall through
				case '[':
					have_meta = true;
					break;
				default:
					// do nothing
					break;
			}
		}
		
		if ( !have_meta )
		{
			if ( slash != NULL )
			{
				const char* remainder = slash + 1;
				std::string this_dir( path, remainder );
				
				ExpandPathnames( from_dir + this_dir, remainder, result );
			}
			else
			{
				result.push_back( from_dir + path );
			}
			
			return;
		}
		
		if ( slash == NULL )
		{
			MatchPathnames( from_dir, path, result );
		}
	}
	
	std::vector< std::string > PathnameExpansion( const std::string& word )
	{
		// Genie*.[ch]
		
		std::vector< std::string > result;
		
		std::size_t meta = word.find_first_of( "*?[" );
		
		if ( meta != word.npos )
		{
			//char path[ 1024 ];
			
			//if ( getcwd( path, 1024 ) == NULL throw N::ParamErr() );
			
			ExpandPathnames( "", word.c_str(), result );
		}
		
		if ( result.empty() )
		{
			result.push_back( word );
		}
		
		return result;
	}
	
	std::string QuoteRemoval( const std::string& word )
	{
		std::string newstr;
		const char *p, *q;
		
		p = q = word.c_str();
		
		while (*p != '\0')
		{
			while ( *q != '\0' && !IsAShellQuoteChar( *q ) )
			{
				q++;
			}
			
			newstr += std::string(p, q - p);
			
			switch ( *q )
			{
				case '\0':
					p = q;
					// *p is now NUL, which will break the loop.
					break;
				
				case '\\':
					// q points to the backslash
					p = q + 1;
					// p points to the char after the backslash
					// this char is escaped, so skip over it
					q = p + 1;
					// q points to the char *after* the escaped char
					break;
				
				case '\'':
					p = ++q;  // point after the single-quote
					while (*q != '\0' && *q != '\'')
					{
						q++;
					}
					newstr += std::string(p, q - p);
					if (*q != '\0')
					{
						q++;
					}
					p = q;
					break;
				
				case '"':
					p = ++q;
					
					while ( *q != '\0'  &&  *q != '"' )
					{
						if ( *q == '\\' )
						{
							newstr += std::string( p, q - p );
							p = q + 1;
							
							if ( *p == '\0' )
							{
								break;
							}
							
							q = p + 1;
						}
						else
						{
							q++;
						}
					}
					
					newstr += std::string( p, q - p );
					
					if (*q != '\0')
					{
						q++;
					}
					
					p = q;
					break;
				
				default:
					ASSERT( "Not reached" == 0 );
					break;
			}
		}
		
		return newstr;
	}
	
	static std::string ScanParameter( const char*& p )
	{
		// p is pointing to the '$'
		// At end, p points to the end of the parameter (after '}' if present)
		
		// var points to the start of the parameter name
		const char* var = p + 1;
		
		const char *end;
		
		switch ( *var )
		{
			case '#':
			case '$':
			case '?':
			case '!':
			case '*':
			case '@':
				p = end = var + 1;
				// p > var
				break;
			
			case '{':
				for ( end = ++var;  ( *end != '\0' ) && ( *end != '}' );  ++end ) continue;
				p = end + 1;
				// p > var
				break;
			
			default:
				for ( end = var;  std::isalnum( *end ) || *end == '_';  ++end ) continue;
				p = end;
				// p == var if name is empty, p > var otherwise
				break;
		}
		
		if ( p == var )
		{
			// Single '$', pretend it was "$$"
			--var;
		}
		
		return std::string( var, end - var );
	}
	
	std::vector< std::string > ParameterExpansion( const ParameterDictionary*  dictionary,
	                                               const std::string&          word )
	{
		// $HOME -> /home/jjuran
		
		std::vector< std::string > result;
		
		bool has_empty_params = false;
		
		std::string expansion;  // The expanded string
		
		// Start p pointing at the beginning of the word.
		const char* p = word.c_str();
		
		bool double_quoted = false;
		
		while ( *p != '\0' )
		{
			// Find the next '$'.
			const char* var = ScanForDollar( p, double_quoted );
			
			// Include what we just scanned over in the string.
			expansion += std::string( p, var - p );
			
			if ( *var == '\0' )
			{
				// We've reached the end; we're done.
				break;
			}
			
			p = var;
			
			std::string varName = ScanParameter( p );
			
			std::vector< std::string > lookup = dictionary->Lookup( varName, double_quoted );
			
			// lookup.size() equals 1 except in the case of $*, $@, or "$@"
			
			if ( lookup.size() > 0 )
			{
				expansion += lookup.front();
				
				if ( lookup.size() > 1 )
				{
					result.push_back( expansion );
					
					result.insert( result.end(),
					               lookup.begin() + 1,
					               lookup.end  () - 1 );
					
					expansion = lookup.back();
				}
			}
			else
			{
				has_empty_params = true;
			}
		}
		
		// A plain empty string produces [ "" ], but if it contains $@, then [].
		bool expands_to_empty_list = has_empty_params && QuoteRemoval( expansion ).empty();
		
		if ( !expands_to_empty_list )
		{
			result.push_back( expansion );
		}
		
		return result;
	}
	
	template < class Inserter >
	static inline void Copy( const std::string& word, Inserter inserter )
	{
		*inserter++ = word;
	}
	
	template < class Inserter >
	void Copy( const std::vector< std::string >& words, Inserter inserter )
	{
		std::copy( words.begin(), words.end(), inserter );
	}
	
	static inline std::string Join( const std::string& word )
	{
		return word;
	}
	
	static std::string Join( const std::vector< std::string >& words )
	{
		if ( words.empty() )
		{
			return "";
		}
		
		std::string result = words[0];
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = words.begin() + 1;  it < words.end();  ++it )
		{
			result += " " + *it;
		}
		
		return result;
	}
	
	
	template < class Algorithm >
	class ApplyCommand
	{
		public:
			ApplyCommand( const Algorithm& algorithm ) : algorithm( algorithm )  {}
			
			Command operator()( const Command& command ) const;
		
		private:
			const Algorithm& algorithm;
	};
	
	template < class Algorithm >
	Command ApplyCommand< Algorithm >::operator()( const Command& command ) const
	{
		Command result;
		
		typedef std::vector< std::string >::const_iterator StrIter;
		typedef std::vector< Redirection >::const_iterator RedirIter;
		
		for ( StrIter iArg = command.args.begin();  iArg < command.args.end();  ++iArg )
		{
			Copy( algorithm( *iArg ), back_inserter( result.args ) );
		}
		
		for ( RedirIter itRedir = command.redirections.begin();  itRedir < command.redirections.end();  ++itRedir )
		{
			std::string param = Join( algorithm( itRedir->param ) );
			
			result.redirections.push_back( Redirection( RedirectInfo( *itRedir ), param ) );
		}
		
		return result;
	}
	
	
	template < class Algorithm >
	static inline Command Apply( Algorithm algorithm, const Command& command )
	{
		return ApplyCommand< Algorithm >( algorithm )( command );
	}
	
	Command ParseCommand( const Command&              command,
	                      const ParameterDictionary&  dictionary )
	{
		return 
			Apply( QuoteRemoval,
				Apply( PathnameExpansion,
					Apply( WordSplitting,
						Apply( ProcessSubstitution,
							Apply( ArithmeticExpansion,
								Apply( CommandSubstitution,
									Apply
									(
										std::bind1st
										(
											N::PtrFun( ParameterExpansion ),
											&dictionary
										),
										Apply( TildeExpansion,
											Apply( BraceExpansion,
												command
											)
										)
									)
								)
							)
						)
					)
				)
			);
	}
	
}

