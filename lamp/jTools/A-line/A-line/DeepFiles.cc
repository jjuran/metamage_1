/*	============
 *	DeepFiles.cc
 *	============
 */

#include "A-line/DeepFiles.hh"

// Nitrogen
#include "Nitrogen/Str.h"


namespace ALine {
	
	namespace N = Nitrogen;
	
	struct AllFilter
	{
		template < class Arg1 >  bool operator()( Arg1 ) const  { return true; }
	};
	
	static AllFilter All;
	
	
	vector< FSSpec > DeepFiles( const N::FSDirSpec& dir )
	{
		return DeepFiles( dir, All );
	}
	
	vector< FSSpec > DeepFiles( const FSSpec& item )
	{
		return DeepFiles( item, All );
	}
	
	
	class NameFilter
	{
		private:
			string nameToMatch;
			
		public:
			NameFilter( const string& name ) : nameToMatch( name )  {}
			
			bool operator()( const FSSpec& file ) const
			{
				return N::Convert< string >( file.name ) == nameToMatch;
			}
	};
	
	vector< FSSpec > DeepFiles( const N::FSDirSpec& dir, const string& name )
	{
		return DeepFiles( dir, NameFilter( name ) );
	}
	
	vector< FSSpec > DeepFiles( const FSSpec& item, const string& name )
	{
		return DeepFiles( item, NameFilter( name ) );
	}
	
}

