/*	=============
 *	GetOptions.cc
 *	=============
 */

#include "Orion/GetOptions.hh"

// Standard C++
#include <algorithm>
#include <map>

// Standard C/C++
#include <cstring>

// Nucleus
#include "Nucleus/NAssert.h"

// Option vocabulary

// Syntax
// 
// --				end of option marker
// --foo=bar		long format, inline param (always accepted)
// --foo bar		long format, param is required (strings and lists)
// --foo bar		long format, no param, arg follows (enums, flags, counters)
// -bar				long format, X11 style (not supported)
// -bar				short format, same as -b -a -r (current behavior)
// -bar				short format, "ar" is inline param to -b (short-inlines)
// +z				inverse of -z (not supported)

// Semantics
// 
// Param types:  string, integer, decimal
// Param behaviors:  replace, concat (string only)
// Non-param types:  flag, enum, counter
// Non-param behaviors:  set (flag), select (enum), increment (counter)


namespace Orion
{
	
	class CStringOptionBinding : public OptionBinding
	{
		private:
			const char*& itsString;
		
		public:
			CStringOptionBinding( const char*& string ) : itsString( string )  {}
			
			bool ParameterExpected() const  { return true; }
			
			void Set( const char* param ) const  { itsString = param; }
	};
	
	class StringOptionBinding : public OptionBinding
	{
		private:
			std::string& itsString;
		
		public:
			StringOptionBinding( std::string& string ) : itsString( string )  {}
			
			bool ParameterExpected() const  { return true; }
			
			void Set( const char* param ) const  { itsString = param; }
	};
	
	class StringListOptionBinding : public OptionBinding
	{
		private:
			std::vector< std::string >& itsStrings;
		
		public:
			StringListOptionBinding( std::vector< std::string >& strings ) : itsStrings( strings )  {}
			
			bool ParameterExpected() const  { return true; }
			
			void Set( const char* param ) const  { itsStrings.push_back( param ); }
	};
	
	
	typedef std::map< std::string, OptionID > OptionMap;
	
	typedef std::map< OptionID, boost::shared_ptr< OptionBinding > > BindingMap;
	
	
	static OptionID gLastOptionID = 0;
	
	static OptionMap  gOptionMap;
	static BindingMap gBindingMap;
	
	static std::vector< const char* > gFreeArguments;
	
	
	OptionID NewOption( const char* optionSpec )
	{
		return gOptionMap[ optionSpec ] = ++gLastOptionID;
	}
	
	void AliasOption( const char* from, const char* to )
	{
		gOptionMap[ to ] = gOptionMap[ from ];
	}
	
	
	boost::shared_ptr< OptionBinding > NewOptionBinding( const char*& string )
	{
		return boost::shared_ptr< OptionBinding >( new CStringOptionBinding( string ) );
	}
	
	boost::shared_ptr< OptionBinding > NewOptionBinding( std::string& string )
	{
		return boost::shared_ptr< OptionBinding >( new StringOptionBinding( string ) );
	}
	
	boost::shared_ptr< OptionBinding > NewOptionBinding( std::vector< std::string >& strings )
	{
		return boost::shared_ptr< OptionBinding >( new StringListOptionBinding( strings ) );
	}
	
	
	void AddBinding( OptionID optionID, const boost::shared_ptr< OptionBinding >& binding )
	{
		gBindingMap[ optionID ] = binding;
	}
	
	
	static OptionID FindOptionID( const std::string& name )
	{
		OptionMap::const_iterator it = gOptionMap.find( name );
		
		if ( it == gOptionMap.end() )
		{
			throw UndefinedOption( name );
		}
		
		return it->second;
	}
	
	static boost::shared_ptr< OptionBinding > FindOptionBinding( OptionID optionID )
	{
		BindingMap::const_iterator it = gBindingMap.find( optionID );
		
		ASSERT( it != gBindingMap.end() );
		
		return it->second;
	}
	
	static const OptionBinding& FindOption( const std::string& name )
	{
		return *FindOptionBinding( FindOptionID( name ) );
	}
	
	
	static void SetOption( const std::string& name, char const* const*& it )
	{
		const OptionBinding& binding = FindOption( name );
		
		if ( binding.ParameterExpected() )
		{
			binding.Set( *++it );
		}
		else
		{
			binding.Set();
		}
	}
	
	void GetOptions( int argc, char const *const *argv )
	{
		char const* const* begin = argv + 1;  // Skip the command
		char const* const* end = argv + argc;
		
		for ( char const* const* it = begin;  it != end;  ++it )
		{
			const char* token = *it;
			
			if ( token[ 0 ] == '-' )
			{
				// Starts with "-"
				
				if ( token[ 1 ] == '\0' )
				{
					// "-" is considered a free argument, not an option
				}
				else if ( token[ 1 ] == '-' )
				{
					// Starts with "--"
					
					if ( token[ 2 ] == '\0' )
					{
						// End of option processing
						std::copy( ++it, end, back_inserter( gFreeArguments ) );
						
						break;
					}
					else
					{
						// Long format option
						std::size_t len = std::strlen( token );
						
						// Inline parameter?
						const char* eq = std::find( token + 2, token + len, '=' );
						
						if ( eq == token + len )
						{
							// No inline parameter to option
							SetOption( token, it );
						}
						else
						{
							// Option has parameter
							std::string opt( token, eq - token );
							
							const char* param = eq + 1;
							
							FindOption( opt ).Set( param );
						}
					}
					
					continue;
				}
				else
				{
					// Short format option
					
					++token;
					
					while ( *token != '\0' )
					{
						std::string opt = "-";
						
						opt += *token++;
						
						const OptionBinding& binding = FindOption( opt );
						
						if ( binding.ParameterExpected() )
						{
							binding.Set( *token != '\0' ? token : *++it );
							
							break;
						}
						else
						{
							binding.Set();
						}
					}
					
					continue;
				}
			}
			
			// Not an option
			gFreeArguments.push_back( token );
		}
		
		gOptionMap.clear();
		gBindingMap.clear();
		
		gFreeArguments.push_back( NULL );
	}
	
	char const* const* FreeArguments()
	{
		return &gFreeArguments[0];
	}
	
	std::size_t FreeArgumentCount()
	{
		return gFreeArguments.size() - 1;  // ignore trailing NULL
	}
	
}

