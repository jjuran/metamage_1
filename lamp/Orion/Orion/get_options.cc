/*
	====================
	orion/get_options.cc
	====================
	
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

#include "Orion/get_options.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <map>

// Standard C
#include <string.h>

// iota
#include "iota/decimal.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/functions/write.hh"


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


namespace orion
{
	
	namespace p7 = poseven;
	
	
	template < class Integer >
	class integer_option_binding : public option_binding
	{
		private:
			Integer& its_integer;
		
		public:
			integer_option_binding( Integer& integer ) : its_integer( integer )
			{
			}
			
			bool argument_expected() const  { return true; }
			
			void set( const char* param ) const
			{
				its_integer = iota::parse_decimal( param );
			}
	};
	
	class c_string_option_binding : public option_binding
	{
		private:
			const char*& its_string;
		
		public:
			c_string_option_binding( const char*& string ) : its_string( string )
			{
			}
			
			bool argument_expected() const  { return true; }
			
			void set( const char* param ) const  { its_string = param; }
	};
	
	class string_option_binding : public option_binding
	{
		private:
			std::string& its_string;
		
		public:
			string_option_binding( std::string& string ) : its_string( string )
			{
			}
			
			bool argument_expected() const  { return true; }
			
			void set( const char* param ) const  { its_string = param; }
	};
	
	class string_list_option_binding : public option_binding
	{
		private:
			std::vector< std::string >& its_strings;
		
		public:
			string_list_option_binding( std::vector< std::string >& strings )
			:
				its_strings( strings )
			{
			}
			
			bool argument_expected() const  { return true; }
			
			void set( const char* param ) const
			{
				its_strings.push_back( param );
			}
	};
	
	
	typedef std::map< std::string, option_id_t > option_map_t;
	
	typedef std::vector< option_binding* > binding_vector_t;
	
	
	static option_id_t global_next_option_id = 0;
	
	static option_map_t      global_option_map;
	static binding_vector_t  global_bindings;
	
	static std::vector< iota::arg_t > global_free_arguments;
	
	
	option_id_t new_option( const char* option_spec )
	{
		const unsigned id = global_next_option_id;
		
		global_bindings.resize( ++global_next_option_id );
		
		return global_option_map[ option_spec ] = id;
	}
	
	void alias_option( const char* from, const char* to )
	{
		global_option_map[ to ] = global_option_map[ from ];
	}
	
	
	option_binding* new_option_binding( size_t& integer )
	{
		return new integer_option_binding< size_t >( integer );
	}
	
	option_binding* new_option_binding( const char*& string )
	{
		return new c_string_option_binding( string );
	}
	
	option_binding* new_option_binding( std::string& string )
	{
		return new string_option_binding( string );
	}
	
	option_binding* new_option_binding( std::vector< std::string >& strings )
	{
		return new string_list_option_binding( strings );
	}
	
	
	void add_binding( option_id_t id, option_binding* binding )
	{
		global_bindings[ id ] = binding;
	}
	
	
	static option_id_t find_option_id( const std::string& name )
	{
		option_map_t::const_iterator it = global_option_map.find( name );
		
		if ( it == global_option_map.end() )
		{
			throw undefined_option( name );
		}
		
		return it->second;
	}
	
	static option_binding* find_option_binding( option_id_t id )
	{
		ASSERT( id < global_bindings.size() );
		
		return global_bindings[ id ];
	}
	
	static const option_binding& find_option( const std::string& name )
	{
		return *find_option_binding( find_option_id( name ) );
	}
	
	
	static void SetOption( const std::string& name, iota::argp_t& it )
	{
		const option_binding& binding = find_option( name );
		
		if ( binding.argument_expected() )
		{
			binding.set( *++it );
		}
		else
		{
			binding.set();
		}
	}
	
	static void default_help( const char* )
	{
		typedef option_map_t::const_iterator Iter;
		
		const Iter end = global_option_map.end();
		
		for ( Iter it = global_option_map.begin();  it != end;  ++it )
		{
			p7::write( p7::stdout_fileno, it->first + "\n" );
		}
		
		exit( EXIT_SUCCESS );
	}
	
	static void clear_bindings()
	{
		for ( int i = 0; i < global_bindings.size();  ++i )
		{
			delete global_bindings[i];
		}
		
		global_bindings.clear();
	}
	
	void get_options( int argc, iota::argp_t argv )
	{
		if ( global_option_map.find( "--help" ) == global_option_map.end() )
		{
			bind_option_trigger( "--help", std::ptr_fun( default_help ) );
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
						std::copy( ++it, end, back_inserter( global_free_arguments ) );
						
						break;
					}
					else
					{
						// Long format option
						size_t len = strlen( token );
						
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
							
							find_option( opt ).set( param );
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
						
						const option_binding& binding = find_option( opt );
						
						if ( binding.argument_expected() )
						{
							binding.set( *token != '\0' ? token : *++it );
							
							break;
						}
						else
						{
							binding.set();
						}
					}
					
					continue;
				}
			}
			
			// Not an option
			global_free_arguments.push_back( token );
		}
		
		global_option_map.clear();
		
		clear_bindings();
		
		global_free_arguments.push_back( NULL );
	}
	
	iota::argp_t free_arguments()
	{
		return &global_free_arguments[0];
	}
	
	size_t free_argument_count()
	{
		return global_free_arguments.size() - 1;  // ignore trailing NULL
	}
	
}

