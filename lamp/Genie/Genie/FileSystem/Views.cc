/*	========
 *	Views.cc
 *	========
 */

#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	typedef std::map< const FSTree*, const ViewFactory* > ViewFactoryMap;
	
	typedef std::map< const FSTree*, FSTreePtr > ViewDelegateMap;
	
	static ViewFactoryMap   gViewFactoryMap;
	static ViewDelegateMap  gViewDelegateMap;
	
	
	void AddViewFactory( const FSTree* key, const ViewFactory& factory )
	{
		gViewFactoryMap[ key ] = &factory;
	}
	
	void RemoveViewFactory( const FSTree* key )
	{
		gViewFactoryMap.erase( key );
	}
	
	const ViewFactory* GetViewFactory( const FSTree* key )
	{
		ViewFactoryMap::const_iterator it = gViewFactoryMap.find( key );
		
		if ( it == gViewFactoryMap.end() )
		{
			return NULL;
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

