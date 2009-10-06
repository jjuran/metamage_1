/*	========
 *	Views.cc
 *	========
 */

#include "Genie/FS/Views.hh"

// poseven
#include "poseven/Errno.hh"

// PEdestal
#include "Pedestal/EmptyView.hh"

// Genie
#include "Genie/FS/FSTree_sys_window_REF.hh"


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
		
		void swap( ViewParameters& other );
	};
	
	void ViewParameters::swap( ViewParameters& other )
	{
		itsDelegate.swap( other.itsDelegate );
		
		std::swap( itsFactory,   other.itsFactory   );
		std::swap( itsWindowKey, other.itsWindowKey );
	}
	
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
	
	static bool ViewExists( const FSTree* parent, const std::string& name )
	{
		return FindView( parent, name ) != NULL;
	}
	
	static void AddViewParameters( const FSTree*       parent,
	                               const std::string&  name,
	                               const FSTreePtr&    delegate,
	                               ViewFactory         factory )
	{
		ASSERT( delegate.get() != NULL );
		
		ViewParameters& params = gViewParametersMap[ parent ][ name ];
		
		params.itsFactory  = factory;
		params.itsDelegate = delegate;
	}
	
	static void AddViewWindowKey( const FSTree* parent, const std::string& name, const FSTree* windowKey )
	{
		ASSERT( FindView( parent, name ) != NULL );
		
		ASSERT( FindView( parent, name )->itsDelegate.get() != NULL );
		
		gViewParametersMap[ parent ][ name ].itsWindowKey = windowKey;
	}
	
	static void DeleteDelegate( FSTreePtr& delegate_ref )
	{
		if ( const FSTree* delegate = delegate_ref.get() )
		{
			FSTreePtr delegate_copy;
			
			delegate_copy.swap( delegate_ref );
			
			try
			{
				delegate->Delete();
			}
			catch ( ... )
			{
				if ( TARGET_CONFIG_DEBUGGING )
				{
					// This might happen in __destroy_global_chain(),
					// so don't ASSERT which relies on trashed infrastructure.
					
					::DebugStr( "\p" "Delegate's Delete() method may not throw" );
				}
			}
			
			RemoveAllViewParameters( delegate );
		}
	}
	
	static void RemoveViewParameters( const FSTree* parent, const std::string& name )
	{
		ViewParametersMap::iterator it = gViewParametersMap.find( parent );
		
		if ( it != gViewParametersMap.end() )
		{
			ViewParametersSubMap& submap = it->second;
			
			ViewParametersSubMap::iterator jt = submap.find( name );
			
			ViewParameters temp;
			
			if ( jt != submap.end() )
			{
				temp.swap( jt->second );
				
				submap.erase( jt );
				
				if ( submap.empty() )
				{
					gViewParametersMap.erase( it );
				}
				
				DeleteDelegate( temp.itsDelegate );
			}
		}
	}
	
	void RemoveAllViewParameters( const FSTree* parent )
	{
		ViewParametersMap::iterator it = gViewParametersMap.find( parent );
		
		if ( it != gViewParametersMap.end() )
		{
			ViewParametersSubMap temp;
			
			temp.swap( it->second );
			
			gViewParametersMap.erase( it );
			
			for ( ViewParametersSubMap::iterator jt = temp.begin();  jt != temp.end();  ++jt )
			{
				DeleteDelegate( jt->second.itsDelegate );
			}
		}
	}
	
	static boost::shared_ptr< Ped::View > MakeView( const FSTree*       parent,
	                                                const std::string&  name )
	{
		if ( const ViewParameters* params = FindView( parent, name ) )
		{
			const FSTree* delegate = params->itsDelegate.get();
			
			ViewFactory factory = params->itsFactory;
			
			ASSERT( delegate != NULL );
			ASSERT( factory  != NULL );
			
			return factory( delegate );
		}
		
		return boost::shared_ptr< Pedestal::View >();
	}
	
	static inline const FSTreePtr& GetViewDelegate( const FSTree* parent, const std::string& name )
	{
		const ViewParameters* params = FindView( parent, name );
		
		if ( params == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return params->itsDelegate;
	}
	
	static const FSTreePtr& GetViewDelegate( const FSTree* view )
	{
		const FSTreePtr& delegate = GetViewDelegate( view->ParentRef().get(), view->Name() );
		
		return delegate;
	}
	
	static inline const FSTree* GetViewWindowKey( const FSTree* parent, const std::string& name )
	{
		const ViewParameters* params = FindView( parent, name );
		
		return params ? params->itsWindowKey : NULL;
	}
	
	const FSTree* GetViewWindowKey( const FSTree* view )
	{
		return GetViewWindowKey( view->ParentRef().get(), view->Name() );
	}
	
	
	bool InvalidateWindowForView( const FSTree* view )
	{
		const FSTree* windowKey = GetViewWindowKey( view );
		
		return InvalidateWindow( windowKey );
	}
	
	
	FSTreePtr FSTree_new_View::CreateDelegate( const FSTreePtr&    parent,
	                                           const std::string&  name ) const
	{
		FSTreePtr delegate = Premapped_Factory( parent, name, itsMappings, itsDestructor );
		
		return delegate;
	}
	
	void FSTree_new_View::HardLink( const FSTreePtr& target ) const
	{
		const FSTreePtr& parent = target->ParentRef();
		
		const FSTree* key = parent.get();
		
		const std::string& name = target->Name();
		
		FSTreePtr delegate = CreateDelegate( parent, name );
		
		AddViewParameters( key, name, delegate, itsFactory );
		
		target->CreateDirectory( 0 );  // mode is ignored
	}
	
	
	bool FSTree_View::Exists() const
	{
		return ViewExists( ParentRef().get(), Name() );
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
			const FSTree* windowKey = GetViewWindowKey( this );
			
			UninstallViewFromWindow( Get(), windowKey );
			
			Get() = Ped::EmptyView::Get();
			
			RemoveViewParameters( parent, name );
		}
		else
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	void FSTree_View::CreateDirectory( mode_t mode ) const
	{
		const FSTree* parent = ParentRef().get();
		
		const std::string& name = Name();
		
		if ( ViewExists( parent, name ) )
		{
			const FSTree* windowKey = GetViewWindowKey( parent );
			
			if ( windowKey == NULL )
			{
				windowKey = parent;
			}
			
			AddViewWindowKey( parent, name, windowKey );
			
			boost::shared_ptr< Ped::View > view = MakeView( parent, name );
			
			Get() = view;
			
			// Install and invalidate if window exists
			InstallViewInWindow( view, windowKey );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	FSTreePtr FSTree_View::Lookup( const std::string& name ) const
	{
		return GetViewDelegate( this )->Lookup( name );
	}
	
	FSIteratorPtr FSTree_View::Iterate() const
	{
		return GetViewDelegate( this )->Iterate();
	}
	
}

