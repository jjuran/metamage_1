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

// Boost
#include <boost/shared_ptr.hpp>


namespace Orion
{
	
	class UndefinedOption : public std::exception
	{
		private:
			std::string message;
		
		public:
			UndefinedOption( const std::string& option )
			: message( std::string() + "Undefined option " + option )  {}
			
			~UndefinedOption() throw ()  {}
			
			const char* what() const throw ()  { return message.c_str(); }
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
	
	
	template < class Type >
	boost::shared_ptr< OptionBinding > NewOptionBinding( Type& data, Type value )
	{
		return boost::shared_ptr< OptionBinding >( new SelectorOptionBinding< Type >( data, value ) );
	}
	
	inline boost::shared_ptr< OptionBinding > NewOptionBinding( bool& flag )
	{
		return NewOptionBinding( flag, true );
	}
	
	boost::shared_ptr< OptionBinding > NewOptionBinding( const char*& string );
	
	boost::shared_ptr< OptionBinding > NewOptionBinding( std::string& string );
	
	boost::shared_ptr< OptionBinding > NewOptionBinding( std::vector< std::string >& strings );
	
	template < class Trigger >
	boost::shared_ptr< OptionBinding > NewOptionBinding( Trigger trigger )
	{
		return boost::shared_ptr< OptionBinding >( new TriggerOptionBinding< Trigger >( trigger ) );
	}
	
	
	void AddBinding( OptionID optionID, const boost::shared_ptr< OptionBinding >& binding );
	
	template < class Type >
	void BindOption( const char* optionSpec, Type& data )
	{
		OptionID optionID = NewOption( optionSpec );
		
		boost::shared_ptr< OptionBinding > binding = NewOptionBinding( data );
		
		AddBinding( optionID, binding );
	}
	
	template < class Type >
	void BindOption( const char* optionSpec, Type& data, Type value )
	{
		OptionID optionID = NewOption( optionSpec );
		
		boost::shared_ptr< OptionBinding > binding = NewOptionBinding( data, value );
		
		AddBinding( optionID, binding );
	}
	
	template < class Trigger >
	void BindOptionTrigger( const char* optionSpec, Trigger trigger )
	{
		OptionID optionID = NewOption( optionSpec );
		
		boost::shared_ptr< OptionBinding > binding = NewOptionBinding( trigger );
		
		AddBinding( optionID, binding );
	}
	
	void GetOptions( int argc, char const *const argv[] );
	
	char const* const* FreeArguments();
	
	std::size_t FreeArgumentCount();
	
}

#endif

