/*	============
 *	DeepFiles.cc
 *	============
 */

#include "A-line/DeepFiles.hh"

// Nitrogen
#include "Nitrogen/Str.h"


namespace ALine
{
	
	struct AllFilter
	{
		template < class Arg1 >  bool operator()( Arg1 ) const  { return true; }
	};
	
	static AllFilter All;
	
	std::vector< FSSpec > DeepFiles( const FSSpec& item )
	{
		return DeepFiles( item, All );
	}
	
	std::vector< std::string > DeepFiles( const std::string& item )
	{
		return DeepFiles( item, All );
	}
	
	
	/*
	class NameFilter
	{
		private:
			std::string nameToMatch;
			
		public:
			NameFilter( const std::string& name ) : nameToMatch( name )  {}
			
			bool operator()( const FSSpec& file ) const
			{
				return io::get_filename_string( file ) == nameToMatch;
			}
	};
	
	std::vector< FSSpec > DeepFiles( const FSSpec& item, const std::string& name )
	{
		return DeepFiles( item, NameFilter( name ) );
	}
	*/
	
}

