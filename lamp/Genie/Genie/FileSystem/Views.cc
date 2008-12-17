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
	namespace Ped = Pedestal;
	
	
	struct ViewParameters
	{
		FSTreePtr      itsDelegate;
		ViewFactory    itsFactory;
		const FSTree*  itsWindowKey;
		
		ViewParameters() : itsFactory(), itsWindowKey()
		{
		}
	};
	
	typedef std::map< std::string, ViewParameters > ViewParametersSubMap;
	
	typedef std::map< const FSTree*, ViewParametersSubMap > ViewParametersMap;
	
	static ViewParametersMap gViewParametersMap;
	
	
	static const ViewParameters* FindView( const FSTree* parent, const std::string& name )
	{
		ViewParametersMap::const_iterator the_submap = gViewParametersMap.find( parent );
		
		if ( the_submap != gViewParametersMap.end() )
		{
			const ViewParametersSubMap& submap = the_submap->second;
			
			ViewParametersSubMap::const_iterator it = submap.find( name );
			
			if ( it != submap.end() )
			{
				return &it->second;
			}
		}
		
		return NULL;
	}
	
	bool ViewExists( const FSTree* parent, const std::string& name )
	{
		return FindView( parent, name ) != NULL;
	}
	
	void AddViewParameters( const FSTree*       parent,
	                        const std::string&  name,
	                        const FSTreePtr&    delegate,
	                        ViewFactory         factory )
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
	
	std::auto_ptr< Pedestal::View > MakeView( const FSTree* parent, const std::string& name )
	{
		if ( const ViewParameters* params = FindView( parent, name ) )
		{
			const FSTree* delegate = params->itsDelegate.get();
			
			ViewFactory factory = params->itsFactory;
			
			ASSERT( delegate != NULL );
			ASSERT( factory  != NULL );
			
			return factory( delegate );
		}
		
		return std::auto_ptr< Pedestal::View >();
	}
	
	static inline const FSTreePtr& GetViewDelegate( const FSTree* parent, const std::string& name )
	{
		return gViewParametersMap[ parent ][ name ].itsDelegate;
	}
	
	static const FSTreePtr& GetViewDelegate( const FSTree* view )
	{
		return GetViewDelegate( view->Parent().get(), view->Name() );
	}
	
	static inline const FSTree* GetViewWindowKey( const FSTree* parent, const std::string& name )
	{
		return gViewParametersMap[ parent ][ name ].itsWindowKey;
	}
	
	static const FSTree* GetViewWindowKey( const FSTree* view )
	{
		return GetViewWindowKey( view->Parent().get(), view->Name() );
	}
	
	
	bool InvalidateWindowForView( const FSTree* view )
	{
		const FSTree* windowKey = GetViewWindowKey( view );
		
		return InvalidateWindow( windowKey );
	}
	
	
	void FSTree_new_View::HardLink( const FSTreePtr& target ) const
	{
		const FSTreePtr& parent = target->Parent();
		
		const FSTree* key = parent.get();
		
		const std::string& name = target->Name();
		
		FSTreePtr delegate = MakeDelegate( parent, name );
		
		AddViewParameters( key, name, delegate, itsFactory );
		
		target->CreateDirectory( 0 );  // mode is ignored
	}
	
	
	bool FSTree_View::Exists() const
	{
		return GetViewDelegate( this ) != NULL;
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
		
		std::auto_ptr< Ped::View > view = MakeView( parent, name );
		
		if ( view.get() != NULL )
		{
			const FSTree* windowKey = GetViewWindowKey( parent );
			
			if ( windowKey == NULL )
			{
				windowKey = parent;
			}
			
			AddViewWindowKey( parent, name, windowKey );
			
			AddCustomParameters( view );
			
			InvalidateWindow( windowKey );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	FSTreePtr FSTree_View::Lookup( const std::string& name ) const
	{
		const FSTreePtr& delegate = GetViewDelegate( this );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Lookup( name );
	}
	
	FSIteratorPtr FSTree_View::Iterate() const
	{
		const FSTreePtr& delegate = GetViewDelegate( this );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Iterate();
	}
	
}

