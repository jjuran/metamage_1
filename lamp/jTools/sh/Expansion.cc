/*	============
 *	Expansion.cc
 *	============
 */

#include "Expansion.hh"

// POSIX
#include <dirent.h>
#include <pwd.h>

// Standard C/C++
#include <cstdlib>
#include <cstring>

// more-libc
#include "more/string.h"

// iota
#include "iota/char_types.hh"

// gear
#include "gear/find.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"
#include "plus/string/concat.hh"


namespace ShellShock
{
	
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
	
	static
	StringVector SplitBracedGroup( const char* str )
	{
		const char *p, *q;
		const char *comma, *left, *right;
		int level = 0;
		
		StringVector words;
		
		p = q = str;
		
		while ( *q != '\0' )
		{
			if ( level == 0 )
			{
				comma = ScanForChar( ',', q );
				left  = ScanForChar( '{', q );
				
				if ( comma < left )
				{
					words.push_back( plus::string( p, comma - p ) );
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
		words.push_back( plus::string( p ) );
		
		return words;
	}
	
	StringVector BraceExpansion( const plus::string& word )
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
			return MakeVector( word );
		}
		
		// Copy from the beginning of the word up to the left-brace.
		// That's the preamble.
		plus::string preamble( p, left - p );  // "egg-"
		
		// Point p to the character following the left-brace.
		p = left + 1;
		
		// Scan for the matching right-brace.
		q = ScanBracedGroup( p );  // "}-yung"
		
		// If there's no right-brace, return the word as is.  (No expansion.)
		if ( *q == '\0' )
		{
			return MakeVector( word );
		}
		
		// Copy from after the left-brace up to the right-brace.
		// That's the brace group.
		plus::string group( p, q - p );  // "foo,bar,baz"
		
		// Copy from after the right-brace to the end of the word.
		// That's the postscript.
		plus::string postscript( q + 1 );  // "-yung"
		
		// Recursively expand the postscript, as it may contain more brace groups.
		StringVector posts = BraceExpansion( postscript );
		
		// Split the comma-separated brace group.
		StringVector words = SplitBracedGroup( group.c_str() );
		StringVector expansion;  // The complete expansion
		
		typedef StringVector::const_iterator const_iterator;
		
		// For each word in the first brace group:
		for ( const_iterator word = words.begin();  word < words.end();  ++word )
		{
			// Recursively expand the word.  (Expand subgroups.)
			StringVector subs = BraceExpansion( *word );
			
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
	
	static plus::string quoted_tilde_expansion( const char* homedir, const plus::string& subpath )
	{
		const size_t home_len = std::strlen( homedir );
		const size_t path_len = subpath.size();
		
		size_t n_quotes = 0;
		
		const char* end = homedir + home_len;
		
		for ( const char* p = homedir;  p < end;  ++p )
		{
			if ( *p == '\'' )
			{
				++n_quotes;
			}
		}
		
		const size_t result_len = 1 + home_len + n_quotes * 3 + 1 + path_len;
		
		plus::string result;
		
		char* q = result.reset( result_len );
		
		*q++ = '\'';
		
		if ( n_quotes != 0 )
		{
			for ( const char* p = homedir;  p < end; )
			{
				const char c = *q++ = *p++;
				
				if ( c == '\'' )
				{
					*q++ = '\\';
					*q++ = '\'';
					*q++ = '\'';
				}
			}
		}
		else
		{
			q = (char*) mempcpy( q, homedir, home_len );
		}
		
		*q++ = '\'';
		
		mempcpy( q, subpath.data(), path_len );
		
		return result;
	}
	
	plus::string TildeExpansion( const plus::string& word )
	{
		// POSIX.1-2013 Description (2.6.1 Tilde Expansion):
		// http://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_06_01
		
		const char* path = word.c_str();
		
		// Look for a tilde at the start of the first path element.
		if ( path[ 0 ] == '~' )
		{
			if ( path[ 1 ] == '\0'  ||  path[ 1 ] == '/' )
			{
				// Tilde is single first path element; expand to $HOME.
				// ~ -> /home/jjuran
				
				const char* homedir = std::getenv( "HOME" );
				
				// ... but only if $HOME is defined.
				if ( homedir != NULL )
				{
					return quoted_tilde_expansion( homedir, word.substr( 1 ) );
				}
			}
			else
			{
				// ~username format. Expand to appropriate user's home directory.
				// ~jjuran -> /home/jjuran
				
				const size_t first_slash = word.find( '/' );
				
				const plus::string username = word.substr( 1, first_slash - 1 );
				
				struct passwd* userinfo = getpwnam( username.c_str() );
				
				if ( userinfo != NULL )
				{
					// We got something in ~username format. Try to substitute the path.
					const char* homedir = userinfo->pw_dir;
					
					if ( homedir[ 0 ] != '\0' )
					{
						plus::string subpath;
						
						if ( first_slash != plus::string::npos )
						{
							subpath = word.substr( first_slash );
						}
						
						return quoted_tilde_expansion( homedir, subpath );
					}
				}
			}
		}
		
		// No tilde substitution performed.
		return word;
	}
	
	plus::string CommandSubstitution( const plus::string& word )
	{
		// $(hostname) -> toast.metamage.pdq
		
		return word;
	}
	
	plus::string ArithmeticExpansion( const plus::string& word )
	{
		// $((expression))
		
		return word;
	}
	
	plus::string ProcessSubstitution( const plus::string& word )
	{
		// <(log to /dev/fd/n) -> /dev/fd/n
		
		return word;
	}
	
	StringVector WordSplitting( const plus::string& word )
	{
		return MakeVector( word );
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
	
	static
	void MatchPathnames( const plus::string&  in_dir,
	                     const char*          pattern,
	                     StringVector&        result )
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
	
	static
	void ExpandPathnames( const plus::string&  from_dir,
	                      const char*          path,
	                      StringVector&        result )
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
				plus::string this_dir( path, remainder );
				
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
		else
		{
			const char* remainder = slash + 1;
			
			const plus::string filename( path, slash );
			
			StringVector matches;
			
			MatchPathnames( from_dir, filename.c_str(), matches );
			
			typedef StringVector::const_iterator Iter;
			
			for ( Iter it = matches.begin();  it != matches.end();  ++it )
			{
				plus::string this_dir = *it + "/";
				
				ExpandPathnames( from_dir + this_dir, remainder, result );
			}
		}
	}
	
	StringVector PathnameExpansion( const plus::string& word )
	{
		// Genie*.[ch]
		
		StringVector result;
		
		const unsigned char metachars[] = { 3, '*', '?', '[' };
		
		if ( gear::find_first_match( word.data(), word.size(), metachars ) )
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
	
	plus::string QuoteRemoval( const plus::string& word )
	{
		plus::var_string newstr;
		
		const char *p, *q;
		
		p = q = word.c_str();
		
		while (*p != '\0')
		{
			while ( *q != '\0' && !IsAShellQuoteChar( *q ) )
			{
				q++;
			}
			
			newstr.append( p, q - p );
			
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
					
					newstr.append( p, q - p );
					
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
						const char* mark = q;
						
						if ( *q++ == '\\' )
						{
							const char c = *q;
							
							if ( c == '\0' )
							{
								break;
							}
							
							if ( c == '\\'  ||  c == '"'  ||  c == '$' )
							{
								newstr.append( p, mark );
								
								p = q;
							}
							
							++q;
						}
					}
					
					newstr.append( p, q - p );
					
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
	
	static plus::string ScanParameter( const char*& p )
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
				for ( end = var;  iota::is_alnum( *end ) || *end == '_';  ++end ) continue;
				p = end;
				// p == var if name is empty, p > var otherwise
				break;
		}
		
		if ( p == var )
		{
			// Single '$', pretend it was "$$"
			--var;
		}
		
		return plus::string( var, end - var );
	}
	
	StringVector ParameterExpansion( param_lookup_f       lookup_param,
	                                 const plus::string&  word )
	{
		// $HOME -> /home/jjuran
		
		StringVector result;
		
		bool has_empty_params = false;
		
		plus::var_string expansion;  // The expanded string
		
		// Start p pointing at the beginning of the word.
		const char* p = word.c_str();
		
		bool double_quoted = false;
		
		while ( *p != '\0' )
		{
			// Find the next '$'.
			const char* var = ScanForDollar( p, double_quoted );
			
			// Include what we just scanned over in the string.
			expansion.append( p, var - p );
			
			if ( *var == '\0' )
			{
				// We've reached the end; we're done.
				break;
			}
			
			p = var;
			
			plus::string varName = ScanParameter( p );
			
			StringVector lookup = lookup_param( varName, double_quoted );
			
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
	
	class parameter_expander
	{
		private:
			param_lookup_f its_lookup;
		
		public:
			typedef StringVector result_type;
			
			parameter_expander( param_lookup_f f ) : its_lookup( f )
			{
			}
			
			result_type operator()( const plus::string& word ) const
			{
				return ParameterExpansion( its_lookup, word );
			}
	};
	
	static inline
	void Copy( const plus::string& word, StringVector& container )
	{
		container.push_back( word );
	}
	
	static
	void Copy( const StringVector& words, StringVector& container )
	{
		size_t n = words.size();
		
		for ( size_t i = 0;  i < n;  ++i )
		{
			container.push_back( words[ i ] );
		}
	}
	
	static inline plus::string Join( const plus::string& word )
	{
		return word;
	}
	
	static
	plus::string Join( const StringVector& words )
	{
		if ( words.empty() )
		{
			return "";
		}
		
		plus::var_string result = words[0];
		
		typedef StringVector::const_iterator Iter;
		
		for ( Iter it = words.begin() + 1;  it < words.end();  ++it )
		{
			result += " ";
			result += *it;
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
		
		typedef StringVector               ::const_iterator StrIter;
		typedef std::vector< Redirection  >::const_iterator RedirIter;
		
		for ( StrIter iArg = command.args.begin();  iArg < command.args.end();  ++iArg )
		{
			Copy( algorithm( *iArg ), result.args );
		}
		
		for ( RedirIter itRedir = command.redirections.begin();  itRedir < command.redirections.end();  ++itRedir )
		{
			plus::string param = Join( algorithm( itRedir->param ) );
			
			result.redirections.push_back( Redirection( RedirectInfo( *itRedir ), param ) );
		}
		
		return result;
	}
	
	
	template < class Algorithm >
	static inline Command Apply( Algorithm algorithm, const Command& command )
	{
		return ApplyCommand< Algorithm >( algorithm )( command );
	}
	
	Command ParseCommand( const Command& command, param_lookup_f lookup_param )
	{
		return 
			Apply( QuoteRemoval,
				Apply( PathnameExpansion,
					Apply( WordSplitting,
						Apply( ProcessSubstitution,
							Apply( ArithmeticExpansion,
								Apply( CommandSubstitution,
									Apply( parameter_expander( lookup_param ),
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
