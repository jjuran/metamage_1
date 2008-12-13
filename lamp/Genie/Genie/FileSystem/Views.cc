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
		FSTreePtr                         itsDelegate;
		const FSTree*                     itsWindowKey;
		
		ViewParameters() : itsWindowKey()
		{
		}
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
	
	void AddViewParameters( const FSTree*                            parent,
	                        const std::string&                       name,
	                        const boost::shared_ptr< ViewFactory >&  factory,
	                        const FSTreePtr&                         delegate )
	{
		ViewParameters& params = gViewParametersMap[ parent ][ name ];
		
		params.itsFactory  = factory;
		params.itsDelegate = delegate;
	}
	
	void AddViewWindowKey( const FSTree* parent, const std::string& name, const FSTree* windowKey )
	{
		gViewParametersMap[ parent ][ name ].itsWindowKey = windowKey;
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
	
	const boost::shared_ptr< ViewFactory >& GetViewFactory( const FSTree* parent, const std::string& name )
	{
		return gViewParametersMap[ parent ][ name ].itsFactory;
	}
	
	const FSTreePtr& GetViewDelegate( const FSTree* parent, const std::string& name )
	{
		return gViewParametersMap[ parent ][ name ].itsDelegate;
	}
	
	const FSTree* GetViewWindowKey( const FSTree* parent, const std::string& name )
	{
		return gViewParametersMap[ parent ][ name ].itsWindowKey;
	}
	
}

