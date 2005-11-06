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


namespace Orion
{
	
	class UndefinedOption : public std::exception
	{
		private:
			std::string message;
		
		public:
			UndefinedOption( const std::string& option )
			: message( std::string() + "Undefined option " + option )  {}
			
			const char* what() const  { return message.c_str(); }
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
	
}

#endif

