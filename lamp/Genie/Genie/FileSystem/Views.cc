/*	========
 *	Views.cc
 *	========
 */

#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	typedef std::map< const FSTree*, boost::shared_ptr< ViewFactory > > ViewFactoryMap;
	
	typedef std::map< const FSTree*, FSTreePtr > ViewDelegateMap;
	
	static ViewFactoryMap   gViewFactoryMap;
	static ViewDelegateMap  gViewDelegateMap;
	
	
	void AddViewFactory( const FSTree* key, const boost::shared_ptr< ViewFactory >& factory )
	{
		gViewFactoryMap[ key ] = factory;
	}
	
	void RemoveViewFactory( const FSTree* key )
	{
		gViewFactoryMap.erase( key );
	}
	
	boost::shared_ptr< ViewFactory > gEmptyFactory;
	
	const boost::shared_ptr< ViewFactory >& GetViewFactory( const FSTree* key )
	{
		ViewFactoryMap::const_iterator it = gViewFactoryMap.find( key );
		
		if ( it == gViewFactoryMap.end() )
		{
			return gEmptyFactory;
		}
		
		return it->second;
	}
	
	
	void AddViewDelegate( const FSTree* key, const FSTreePtr& delegate )
	{
		gViewDelegateMap[ key ] = delegate;
	}
	
	void RemoveViewDelegate( const FSTree* key )
	{
		gViewDelegateMap.erase( key );
	}
	
	static FSTreePtr gEmptyFSTreePtr;
	
	const FSTreePtr& GetViewDelegate( const FSTree* key )
	{
		ViewDelegateMap::const_iterator it = gViewDelegateMap.find( key );
		
		if ( it == gViewDelegateMap.end() )
		{
			return gEmptyFSTreePtr;
		}
		
		return it->second;
	}
	
}

