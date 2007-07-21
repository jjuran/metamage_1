/*	=============
 *	GetOptions.hh
 *	=============
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
	
	void GetOptions( int argc, char const *const argv[] );
	
	const std::vector< const char* >& FreeArguments();
	
}

#endif

