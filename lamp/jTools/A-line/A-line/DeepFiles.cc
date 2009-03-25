/*	============
 *	DeepFiles.cc
 *	============
 */

#include "A-line/DeepFiles.hh"


namespace tool
{
	
	struct AllFilter
	{
		template < class Arg1 >  bool operator()( Arg1 ) const  { return true; }
	};
	
	static AllFilter All;
	
	std::vector< std::string > DeepFiles( const std::string& item )
	{
		return DeepFiles( item, All );
	}
	
}

