/*	========
 *	Views.cc
 *	========
 */

#include "Genie/FileSystem/Views.hh"

// POSeven
#include "POSeven/Errno.hh"

// PEdestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
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
	
	
	bool FSTree_View::Exists() const
	{
		return GetViewDelegate( ParentKey(), Name() ) != NULL;
	}
	
	void FSTree_View::SetTimes() const
	{
		if ( !InvalidateWindowForView( this ) )
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	void FSTree_View::Delete() const
	{
		const FSTree* parent = ParentKey();
		
		const std::string& name = Name();
		
		if ( ViewExists( parent, name ) )
		{
			DeleteCustomParameters();
			
			InvalidateWindowForView( this );
			
			RemoveViewParameters( parent, name );
		}
		else
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	void FSTree_View::CreateDirectory( mode_t mode ) const
	{
		const FSTree* parent = Parent().get();
		
		const std::string& name = Name();
		
		if ( const boost::shared_ptr< ViewFactory >& factory = GetViewFactory( parent, name ) )
		{
			const FSTree* windowKey = GetViewWindowKey( parent->Parent().get(), parent->Name() );
			
			AddViewWindowKey( parent, name, windowKey );
			
			AddCustomParameters( (*factory)() );
			
			InvalidateWindowForView( this );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	FSTreePtr FSTree_View::Lookup( const std::string& name ) const
	{
		const FSTreePtr& delegate = GetViewDelegate( ParentKey(), Name() );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Lookup( name );
	}
	
	FSIteratorPtr FSTree_View::Iterate() const
	{
		const FSTreePtr& delegate = GetViewDelegate( ParentKey(), Name() );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Iterate();
	}
	
}

