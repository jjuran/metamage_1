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
	
	std::vector< plus::string > DeepFiles( const plus::string& item )
	{
		return DeepFiles( item, All );
	}
	
}

