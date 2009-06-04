/*
	===================
	Orion/GetOptions.cc
	===================
	
	Copyright 2005-2009 Joshua Juran.
	
	This file is part of Orion.
	
    Orion is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
	
    Orion is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
	
    You should have received a copy of the GNU Lesser General Public License
    along with Orion.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Orion/GetOptions.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <map>

// Standard C/C++
#include <cstdlib>
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
	
	template < class Integer >
	class IntegerOptionBinding : public OptionBinding
	{
		private:
			Integer& itsInteger;
		
		public:
			IntegerOptionBinding( Integer& integer ) : itsInteger( integer )  {}
			
			bool ParameterExpected() const  { return true; }
			
			void Set( const char* param ) const  { itsInteger = std::atoi( param ); }
	};
	
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
	
	typedef std::vector< boost::shared_ptr< OptionBinding > > BindingVector;
	
	
	static OptionID gNextOptionID = 0;
	
	static OptionMap  gOptionMap;
	static BindingVector gBindings;
	
	static std::vector< iota::arg_t > gFreeArguments;
	
	
	OptionID NewOption( const char* optionSpec )
	{
		const unsigned id = gNextOptionID;
		
		gBindings.resize( ++gNextOptionID );
		
		return gOptionMap[ optionSpec ] = id;
	}
	
	void AliasOption( const char* from, const char* to )
	{
		gOptionMap[ to ] = gOptionMap[ from ];
	}
	
	
	boost::shared_ptr< OptionBinding > NewOptionBinding( std::size_t& integer )
	{
		return boost::shared_ptr< OptionBinding >( new IntegerOptionBinding< std::size_t >( integer ) );
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
		gBindings[ optionID ] = binding;
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
	
	static const boost::shared_ptr< OptionBinding >& FindOptionBinding( OptionID optionID )
	{
		ASSERT( optionID < gBindings.size() );
		
		return gBindings[ optionID ];
	}
	
	static const OptionBinding& FindOption( const std::string& name )
	{
		return *FindOptionBinding( FindOptionID( name ) );
	}
	
	
	static void SetOption( const std::string& name, iota::argp_t& it )
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
	
	static void DefaultHelp( const char* )
	{
		typedef OptionMap::const_iterator Iter;
		
		for ( Iter it = gOptionMap.begin();  it != gOptionMap.end();  ++it )
		{
			std::printf( "%s\n", it->first.c_str() );
		}
		
		std::exit( EXIT_SUCCESS );
	}
	
	void GetOptions( int argc, iota::argp_t argv )
	{
		if ( gOptionMap.find( "--help" ) == gOptionMap.end() )
		{
			BindOptionTrigger( "--help", std::ptr_fun( DefaultHelp ) );
		}
		
		iota::argp_t begin = argv + 1;  // Skip the command
		iota::argp_t end = argv + argc;
		
		for ( iota::argp_t it = begin;  it != end;  ++it )
		{
			iota::arg_t token = *it;
			
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
		gBindings.clear();
		
		gFreeArguments.push_back( NULL );
	}
	
	iota::argp_t FreeArguments()
	{
		return &gFreeArguments[0];
	}
	
	std::size_t FreeArgumentCount()
	{
		return gFreeArguments.size() - 1;  // ignore trailing NULL
	}
	
}

