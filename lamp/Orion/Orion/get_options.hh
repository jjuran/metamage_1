/*
	====================
	orion/get_options.hh
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

#ifndef ORION_GET_OPTIONS_HH
#define ORION_GET_OPTIONS_HH

// Standard C++
#include <string>
#include <vector>

// Iota
#include "iota/argv.hh"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/Exception.h"


namespace orion
{
	
	class undefined_option : public Nucleus::Exception
	{
		public:
			undefined_option( const std::string& option )
			:
				Nucleus::Exception( "Undefined option " + option )
			{
			}
			
			~undefined_option() throw ()
			{
			}
	};
	
	
	typedef unsigned option_id_t;
	
	option_id_t new_option( const char* option_spec );
	
	void alias_option( const char* from, const char* to );
	
	
	class option_binding
	{
		public:
			virtual bool argument_expected() const = 0;
			
			virtual void set( const char* param = "" ) const = 0;
	};
	
	template < class Type >
	class selector_option_binding : public option_binding
	{
		private:
			Type&  its_data;
			Type   its_value;
		
		public:
			selector_option_binding( Type& data, Type value )
			:
				its_data ( data  ),
				its_value( value )
			{
			}
			
			bool argument_expected() const  { return false; }
			
			void set( const char* ) const  { its_data = its_value; }
	};
	
	template < class Type >
	class converter_option_binding : public option_binding
	{
		private:
			Type& its_data;
		
		public:
			converter_option_binding( Type& data ) : its_data( data )
			{
			}
			
			bool argument_expected() const  { return true; }
			
			void set( const char* value ) const  { its_data = Nucleus::Convert< Type >( value ); }
	};
	
	template < class Trigger >
	class trigger_option_binding : public option_binding
	{
		private:
			Trigger its_trigger;
		
		public:
			trigger_option_binding( Trigger trigger ) : its_trigger( trigger )
			{
			}
			
			bool argument_expected() const  { return false; }
			
			void set( const char* param ) const  { its_trigger( param ); }
	};
	
	template < class Trigger, class ParamType >
	class trigger_with_parameter_option_binding : public option_binding
	{
		private:
			Trigger its_trigger;
		
		public:
			trigger_with_parameter_option_binding( Trigger trigger )
			:
				its_trigger( trigger )
			{
			}
			
			bool argument_expected() const  { return true; }
			
			void set( const char* param ) const
			{
				ParamType converted = Nucleus::Convert< ParamType, std::string >( param );
				
				its_trigger( converted );
			}
	};
	
	
	template < class Type >
	option_binding* new_option_binding( Type& data, Type value )
	{
		return new selector_option_binding< Type >( data, value );
	}
	
	template < class Type >
	option_binding* new_option_binding( Type& data )
	{
		return new converter_option_binding< Type >( data );
	}
	
	inline option_binding* new_option_binding( bool& flag )
	{
		return new_option_binding( flag, true );
	}
	
	option_binding* new_option_binding( std::size_t& integer );
	
	option_binding* new_option_binding( const char*& string );
	
	option_binding* new_option_binding( std::string& string );
	
	option_binding* new_option_binding( std::vector< std::string >& strings );
	
	template < class Trigger >
	option_binding* new_trigger_option_binding( Trigger trigger )
	{
		return new trigger_option_binding< Trigger >( trigger );
	}
	
	template < class ParamType, class Trigger >
	option_binding* new_trigger_with_parameter_option_binding( Trigger trigger )
	{
		return new trigger_with_parameter_option_binding< Trigger, ParamType >( trigger );
	}
	
	
	void add_binding( option_id_t id, option_binding* binding );
	
	template < class Type >
	void bind_option_to_variable( const char* option_spec, Type& data )
	{
		option_id_t id = new_option( option_spec );
		
		option_binding* binding = new_option_binding( data );
		
		add_binding( id, binding );
	}
	
	template < class Type >
	void bind_option_to_variable( const char* option_spec, Type& data, Type value )
	{
		option_id_t id = new_option( option_spec );
		
		option_binding* binding = new_option_binding( data, value );
		
		add_binding( id, binding );
	}
	
	template < class Trigger >
	void bind_option_trigger( const char* option_spec, Trigger trigger )
	{
		option_id_t id = new_option( option_spec );
		
		option_binding* binding = new_trigger_option_binding( trigger );
		
		add_binding( id, binding );
	}
	
	template < class ParamType, class Trigger >
	void bind_option_trigger( const char* option_spec, Trigger trigger )
	{
		option_id_t id = new_option( option_spec );
		
		option_binding* binding = new_trigger_with_parameter_option_binding< ParamType >( trigger );
		
		add_binding( id, binding );
	}
	
	void get_options( int argc, iota::argp_t argv );
	
	iota::argp_t free_arguments();
	
	std::size_t free_argument_count();
	
}

#endif

