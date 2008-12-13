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
	
	typedef std::map< std::string, ViewParameters > ViewParametersSubMap;
	
	typedef std::map< const FSTree*, ViewParametersSubMap > ViewParametersMap;
	
	static ViewParametersMap gViewParametersMap;
	
	
	bool ViewExists( const FSTree* parent, const std::string& name )
	{
		ViewParametersMap::const_iterator it = gViewParametersMap.find( parent );
		
		if ( it != gViewParametersMap.end() )
		{
			const ViewParametersSubMap& submap = it->second;
			
			return submap.find( name ) != submap.end();
		}
		
		return false;
	}
	
	void RemoveViewParameters( const FSTree* parent, const std::string& name )
	{
		ViewParametersMap::iterator it = gViewParametersMap.find( parent );
		
		if ( it != gViewParametersMap.end() )
		{
			ViewParametersSubMap& submap = it->second;
			
			submap.erase( name );
			
			if ( submap.empty() )
			{
				gViewParametersMap.erase( it );
			}
		}
	}
	
	void RemoveAllViewParameters( const FSTree* parent )
	{
		gViewParametersMap.erase( parent );
	}
	
	
	void AddViewFactory( const FSTree* parent, const std::string& name, const boost::shared_ptr< ViewFactory >& factory )
	{
		gViewParametersMap[ parent ][ name ].itsFactory = factory;
	}
	
	const boost::shared_ptr< ViewFactory >& GetViewFactory( const FSTree* parent, const std::string& name )
	{
		return gViewParametersMap[ parent ][ name ].itsFactory;
	}
	
	
	void AddViewDelegate( const FSTree* parent, const std::string& name, const FSTreePtr& delegate )
	{
		gViewParametersMap[ parent ][ name ].itsDelegate = delegate;
	}
	
	const FSTreePtr& GetViewDelegate( const FSTree* parent, const std::string& name )
	{
		return gViewParametersMap[ parent ][ name ].itsDelegate;
	}
	
}

