/*
	===================
	Orion/GetOptions.hh
	===================
	
	Copyright 2005-2007 Joshua Juran.
	
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

#ifndef ORION_GETOPTIONS_HH
#define ORION_GETOPTIONS_HH

// Standard C++
#include <exception>
#include <string>
#include <vector>

// Iota
#include "iota/argv.hh"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/Exception.h"


namespace Orion
{
	
	class UndefinedOption : public Nucleus::Exception
	{
		public:
			UndefinedOption( const std::string& option ) : Nucleus::Exception( "Undefined option " + option )  {}
			
			~UndefinedOption() throw ()  {}
	};
	
	
	typedef unsigned OptionID;
	
	OptionID NewOption( const char* optionSpec );
	
	void AliasOption( const char* from, const char* to );
	
	
	class OptionBinding
	{
		public:
			virtual bool ParameterExpected() const = 0;
			
			virtual void Set( const char* param = "" ) const = 0;
	};
	
	template < class Type >
	class SelectorOptionBinding : public OptionBinding
	{
		private:
			Type& itsData;
			Type itsValue;
		
		public:
			SelectorOptionBinding( Type& data, Type value ) : itsData( data ), itsValue( value )  {}
			
			bool ParameterExpected() const  { return false; }
			
			void Set( const char* ) const  { itsData = itsValue; }
	};
	
	template < class Type >
	class ConverterOptionBinding : public OptionBinding
	{
		private:
			Type& itsData;
		
		public:
			ConverterOptionBinding( Type& data ) : itsData( data )  {}
			
			bool ParameterExpected() const  { return true; }
			
			void Set( const char* value ) const  { itsData = Nucleus::Convert< Type >( value ); }
	};
	
	template < class Trigger >
	class TriggerOptionBinding : public OptionBinding
	{
		private:
			Trigger itsTrigger;
		
		public:
			TriggerOptionBinding( Trigger trigger ) : itsTrigger( trigger )  {}
			
			bool ParameterExpected() const  { return false; }
			
			void Set( const char* param ) const  { itsTrigger( param ); }
	};
	
	template < class Trigger, class ParamType >
	class TriggerWithParameterOptionBinding : public OptionBinding
	{
		private:
			Trigger itsTrigger;
		
		public:
			TriggerWithParameterOptionBinding( Trigger trigger ) : itsTrigger( trigger )  {}
			
			bool ParameterExpected() const  { return true; }
			
			void Set( const char* param ) const
			{
				ParamType converted = Nucleus::Convert< ParamType >( std::string( param ) );
				
				itsTrigger( converted );
			}
	};
	
	
	template < class Type >
	OptionBinding* NewOptionBinding( Type& data, Type value )
	{
		return new SelectorOptionBinding< Type >( data, value );
	}
	
	template < class Type >
	OptionBinding* NewOptionBinding( Type& data )
	{
		return new ConverterOptionBinding< Type >( data );
	}
	
	inline OptionBinding* NewOptionBinding( bool& flag )
	{
		return NewOptionBinding( flag, true );
	}
	
	OptionBinding* NewOptionBinding( std::size_t& integer );
	
	OptionBinding* NewOptionBinding( const char*& string );
	
	OptionBinding* NewOptionBinding( std::string& string );
	
	OptionBinding* NewOptionBinding( std::vector< std::string >& strings );
	
	template < class Trigger >
	OptionBinding* NewTriggerOptionBinding( Trigger trigger )
	{
		return new TriggerOptionBinding< Trigger >( trigger );
	}
	
	template < class ParamType, class Trigger >
	OptionBinding* NewTriggerWithParameterOptionBinding( Trigger trigger )
	{
		return new TriggerWithParameterOptionBinding< Trigger, ParamType >( trigger );
	}
	
	
	void AddBinding( OptionID optionID, OptionBinding* binding );
	
	template < class Type >
	void BindOption( const char* optionSpec, Type& data )
	{
		OptionID optionID = NewOption( optionSpec );
		
		OptionBinding* binding = NewOptionBinding( data );
		
		AddBinding( optionID, binding );
	}
	
	template < class Type >
	void BindOption( const char* optionSpec, Type& data, Type value )
	{
		OptionID optionID = NewOption( optionSpec );
		
		OptionBinding* binding = NewOptionBinding( data, value );
		
		AddBinding( optionID, binding );
	}
	
	template < class Trigger >
	void BindOptionTrigger( const char* optionSpec, Trigger trigger )
	{
		OptionID optionID = NewOption( optionSpec );
		
		OptionBinding* binding = NewTriggerOptionBinding( trigger );
		
		AddBinding( optionID, binding );
	}
	
	template < class ParamType, class Trigger >
	void BindOptionTrigger( const char* optionSpec, Trigger trigger )
	{
		OptionID optionID = NewOption( optionSpec );
		
		OptionBinding* binding = NewTriggerWithParameterOptionBinding< ParamType >( trigger );
		
		AddBinding( optionID, binding );
	}
	
	void GetOptions( int argc, iota::argp_t argv );
	
	iota::argp_t FreeArguments();
	
	std::size_t FreeArgumentCount();
	
}

#endif

