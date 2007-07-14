/*	=============
 *	GetOptions.cc
 *	=============
 */

#include "Orion/GetOptions.hh"

// Standard C++
#include <algorithm>
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
	
	void Options::DefineSetFlag( const std::string& opt, int code )
	{
		myFlagDefs[ opt ] = code;
	}
	
	void Options::DefineSetInteger( const std::string& opt, int code )
	{
		myIntegerDefs[ opt ] = code;
	}
	
	void Options::DefineSetString( const std::string& opt, int code )
	{
		myStringDefs[ opt ] = code;
	}
	
	void Options::DefineAppendToStringList( const std::string& opt, int code )
	{
		myStringListDefs[ opt ] = code;
	}
	
	void Options::DefineSelectEnum( const std::string& opt, int code, int enumParam )
	{
		myEnumDefs[ opt ] = std::make_pair( code, enumParam );
	}
	
	bool Options::ParamExpected( const std::string& opt ) const
	{
		if ( myFlagDefs      .count( opt ) )  return false;
		if ( myIntegerDefs   .count( opt ) )  return true;
		if ( myStringDefs    .count( opt ) )  return true;
		if ( myStringListDefs.count( opt ) )  return true;
		if ( myEnumDefs      .count( opt ) )  return false;
		
		throw UndefinedOption( opt );
	}
	
	void Options::SetOption( const std::string& opt )
	{
		if ( myFlagDefs.count( opt ) )
		{
			myFlags.insert( myFlagDefs[ opt ] );
		}
		else if ( myEnumDefs.count( opt ) )
		{
			myEnums[ myEnumDefs[ opt ].first ] = myEnumDefs[ opt ].second;
		}
	}
	
	void Options::SetOption( const std::string& opt, const std::string& param )
	{
		if ( myIntegerDefs.count( opt ) )
		{
			// FIXME:  This should check for conversion error
			myIntegers[ myIntegerDefs[ opt ] ] = std::atoi( param.c_str() );
		}
		else if ( myStringDefs.count( opt ) )
		{
			myStrings[ myStringDefs[ opt ] ] = param;
		}
		else if ( myStringListDefs.count( opt ) )
		{
			myStringLists[ myStringListDefs[ opt ] ].push_back( param );
		}
	}
	
	void Options::GetOptions( int argc, const char *const argv[] )
	{
		char const* const* begin = argv + 1;  // Skip the command
		char const* const* end = argv + argc;
		
		for ( char const* const* it = begin;  it != end;  ++it )
		{
			const char* token = *it;
			
			if ( token[ 0 ] == '-' )
			{
				// Starts with "-"
				
				if ( token[ 1 ] == '-' )
				{
					// Starts with "--"
					
					if ( token[ 2 ] == '\0' )
					{
						// End of option processing
						std::copy( ++it, end, back_inserter( myFreeParams ) );
						break;
					}
					else
					{
						// Long format option
						std::size_t len = std::strlen( token );
						const char* eq = std::find( token + 2, token + len, '=' );
						
						if ( eq == token + len )
						{
							// No inline parameter to option
							if ( ParamExpected( token ) )
							{
								SetOption( token, *++it );
							}
							else
							{
								SetOption( token );
							}
						}
						else
						{
							// Option has parameter
							std::string opt( token, eq - token );
							const char* paramStart = eq + 1;
							unsigned int paramLen = token + len - paramStart;
							std::string param( paramStart, paramLen );
							
							SetOption( opt, param );
						}
					}
				}
				else
				{
					// Short format option
					
					while ( *++token != '\0' )
					{
						std::string opt = "-";
						opt += *token;
						
						if ( ParamExpected( opt ) )
						{
							SetOption( opt, *++it );
						}
						else
						{
							SetOption( opt );
						}
					}
				}
			}
			else
			{
				// Not an option
				myFreeParams.push_back( token );
			}
		}
	}
	
	bool Options::GetFlag( int code ) const
	{
		return myFlags.count( code );
	}
	
	int Options::GetInteger( int code ) const
	{
		if ( myIntegers.count( code ) )
		{
			return myIntegers.find( code )->second;
		}
		else
		{
			return 0;  // FIXME
		}
	}
	
	std::string Options::GetString( int code ) const
	{
		if ( myStrings.count( code ) )
		{
			return myStrings.find( code )->second;
		}
		else
		{
			return std::string();  // FIXME
		}
	}
	
	std::vector< std::string > Options::GetStringList( int code ) const
	{
		if ( myStringLists.count( code ) )
		{
			return myStringLists.find( code )->second;
		}
		else
		{
			return std::vector< std::string >();  // FIXME
		}
	}
	
	int Options::GetEnum( int code ) const
	{
		if ( myEnums.count( code ) )
		{
			return myEnums.find( code )->second;
		}
		else
		{
			return 0;  // FIXME
		}
	}
	
	class StringListOptionBinding : public OptionBinding
	{
		private:
			std::vector< std::string >& itsStrings;
		
		public:
			StringListOptionBinding( std::vector< std::string >& strings ) : itsStrings( strings )  {}
			
			bool ParameterExpected() const  { return true; }
			
			void Set( const std::string& param ) const  { itsStrings.push_back( param ); }
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
	
	static void SetOption( const std::string& name, char const* const*& argv )
	{
		const OptionBinding& binding = FindOption( name );
		
		if ( binding.ParameterExpected() )
		{
			binding.Set( *++argv );
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
							SetOption( token, argv );
						}
						else
						{
							// Option has parameter
							std::string opt( token, eq - token );
							
							const char* paramStart = eq + 1;
							
							unsigned int paramLen = token + len - paramStart;
							
							std::string param( paramStart, paramLen );
							
							FindOption( opt ).Set( param );
						}
					}
					
					continue;
				}
				else
				{
					// Short format option
					
					while ( *++token != '\0' )
					{
						std::string opt = "-";
						
						opt += *token;
						
						SetOption( opt, argv );
					}
					
					continue;
				}
			}
			
			// Not an option
			gFreeArguments.push_back( token );
		}
		
		gOptionMap.clear();
		gBindingMap.clear();
	}
	
	const std::vector< const char* >& FreeArguments()
	{
		return gFreeArguments;
	}
	
}

