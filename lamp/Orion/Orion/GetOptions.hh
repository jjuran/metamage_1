/*	=============
 *	GetOptions.hh
 *	=============
 */

#ifndef ORION_GETOPTIONS_HH
#define ORION_GETOPTIONS_HH

// Standard C++
#include <exception>
#include <map>
#include <set>
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
	
	class Options
	{
		private:
			bool ParamExpected( const std::string& opt ) const;
			
			void SetOption( const std::string& opt );
			void SetOption( const std::string& opt, const std::string& param );
			
			std::map< std::string, int > myFlagDefs;
			std::map< std::string, int > myIntegerDefs;
			std::map< std::string, int > myStringDefs;
			std::map< std::string, int > myStringListDefs;
			
			std::map< std::string, std::pair< int, int > > myEnumDefs;
			
			std::set< int> myFlags;
			std::map< int, int > myIntegers;
			std::map< int, std::string > myStrings;
			std::map< int, std::vector< std::string > > myStringLists;
			std::map< int, int > myEnums;
			
			std::vector< const char* > myFreeParams;
		
		public:
			void DefineSetFlag( const std::string& opt, int code );
			void DefineSetInteger( const std::string& opt, int code );
			void DefineSetString( const std::string& opt, int code );
			void DefineAppendToStringList( const std::string& opt, int code );
			
			void DefineSelectEnum( const std::string& opt, int code, int enumParam );
			
			void GetOptions( int argc, const char *const argv[] );
			
			bool GetFlag( int code ) const;
			int GetInteger( int code ) const;
			std::string GetString( int code ) const;
			std::vector< std::string > GetStringList( int code ) const;
			int GetEnum( int code ) const;
			
			const std::vector< const char* >& GetFreeParams() const  { return myFreeParams; }
			const char* const* Begin() const  { return &*myFreeParams.begin(); }
			const char* const* End()   const  { return &*myFreeParams.end();   }
	};
	
	
	typedef unsigned OptionID;
	
	OptionID NewOption( const char* optionSpec );
	
	void AliasOption( const char* from, const char* to );
	
	
	class OptionBinding
	{
		public:
			virtual bool ParameterExpected() const = 0;
			
			virtual void Set( const std::string& param = "" ) const = 0;
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
			
			void Set( const std::string& ) const  { itsData = itsValue; }
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
	
	boost::shared_ptr< OptionBinding > NewOptionBinding( int& i );
	
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

