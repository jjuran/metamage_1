/*	========
 *	Views.cc
 *	========
 */

#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	struct ViewParameters
	{
		boost::shared_ptr< ViewFactory >  itsFactory;
		
		FSTreePtr  itsDelegate;
	};
	
	typedef std::map< const FSTree*, ViewParameters > ViewParametersMap;
	
	static ViewParametersMap gViewParametersMap;
	
	
	bool ViewExists( const FSTree* key )
	{
		return gViewParametersMap.find( key ) != gViewParametersMap.end();
	}
	
	void RemoveViewParameters( const FSTree* key )
	{
		gViewParametersMap.erase( key );
	}
	
	
	void AddViewFactory( const FSTree* key, const boost::shared_ptr< ViewFactory >& factory )
	{
		gViewParametersMap[ key ].itsFactory = factory;
	}
	
	const boost::shared_ptr< ViewFactory >& GetViewFactory( const FSTree* key )
	{
		return gViewParametersMap[ key ].itsFactory;
	}
	
	
	void AddViewDelegate( const FSTree* key, const FSTreePtr& delegate )
	{
		gViewParametersMap[ key ].itsDelegate = delegate;
	}
	
	const FSTreePtr& GetViewDelegate( const FSTree* key )
	{
		return gViewParametersMap[ key ].itsDelegate;
	}
	
}

