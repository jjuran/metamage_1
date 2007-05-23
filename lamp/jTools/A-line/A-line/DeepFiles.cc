/*	============
 *	DeepFiles.cc
 *	============
 */

#include "A-line/DeepFiles.hh"

// Nitrogen
#include "Nitrogen/Str.h"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	
	struct AllFilter
	{
		template < class Arg1 >  bool operator()( Arg1 ) const  { return true; }
	};
	
	static AllFilter All;
	
	
	std::vector< FSSpec > DeepFiles( const N::FSDirSpec& dir )
	{
		return DeepFiles( dir, All );
	}
	
	std::vector< FSSpec > DeepFiles( const FSSpec& item )
	{
		return DeepFiles( item, All );
	}
	
	
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
	
	std::vector< FSSpec > DeepFiles( const N::FSDirSpec& dir, const std::string& name )
	{
		return DeepFiles( dir, NameFilter( name ) );
	}
	
	std::vector< FSSpec > DeepFiles( const FSSpec& item, const std::string& name )
	{
		return DeepFiles( item, NameFilter( name ) );
	}
	
}

