/*	========
 *	Views.cc
 *	========
 */

#include "Genie/FS/Views.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// POSIX
#include <sys/stat.h>

// iota
#include "iota/swap.hh"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/simple_map.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/View.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/null.hh"
#include "vfs/primitives/listdir.hh"
#include "vfs/primitives/lookup.hh"
#include "vfs/primitives/mkdir.hh"
#include "vfs/primitives/remove.hh"

// Genie
#include "Genie/FS/gui/port/ADDR.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct ViewParameters
	{
		vfs::node_ptr     itsDelegate;
		ViewFactory       itsFactory;
		const vfs::node*  itsWindowKey;
		
		ViewParameters() : itsFactory(), itsWindowKey()
		{
		}
		
		void swap( ViewParameters& other );
	};
	
	void ViewParameters::swap( ViewParameters& other )
	{
		itsDelegate.swap( other.itsDelegate );
		
		using iota::swap;
		
		swap( itsFactory,   other.itsFactory   );
		swap( itsWindowKey, other.itsWindowKey );
	}
	
	typedef plus::simple_map< const vfs::node*, ViewParameters > ViewParametersMap;
	
	static ViewParametersMap gViewParametersMap;
	
	
	static inline const ViewParameters* find_view( const vfs::node* parent )
	{
		return gViewParametersMap.find( parent );
	}
	
	static bool view_exists( const vfs::node* parent )
	{
		return find_view( parent ) != NULL;
	}
	
	static void add_view_parameters( const vfs::node*  parent,
	                                 const vfs::node&  delegate,
	                                 ViewFactory       factory )
	{
		ViewParameters& params = gViewParametersMap[ parent ];
		
		params.itsFactory  = factory;
		params.itsDelegate = &delegate;
	}
	
	static void add_view_port_key( const vfs::node* parent, const vfs::node* windowKey )
	{
		ASSERT( find_view( parent ) != NULL );
		
		ASSERT( find_view( parent )->itsDelegate.get() != NULL );
		
		gViewParametersMap[ parent ].itsWindowKey = windowKey;
	}
	
	static void DeleteDelegate( vfs::node_ptr& delegate_ref )
	{
		if ( const vfs::node* delegate = delegate_ref.get() )
		{
			vfs::node_ptr delegate_copy;
			
			delegate_copy.swap( delegate_ref );
			
			try
			{
				remove( *delegate );
			}
			catch ( ... )
			{
				if ( CONFIG_DEBUGGING )
				{
					// This might happen in __destroy_global_chain(),
					// so don't ASSERT which relies on trashed infrastructure.
					
					::DebugStr( "\p" "remove( delegate ) may not throw" );
				}
			}
			
			RemoveAllViewParameters( delegate );
		}
	}
	
	void RemoveAllViewParameters( const vfs::node* parent )
	{
		if ( ViewParameters* it = gViewParametersMap.find( parent ) )
		{
			ViewParameters temp;
			
			temp.swap( *it );
			
			gViewParametersMap.erase( parent );
			
			notify_port_of_view_loss( temp.itsWindowKey, temp.itsDelegate.get() );
			
			DeleteDelegate( temp.itsDelegate );
		}
	}
	
	static boost::intrusive_ptr< Ped::View > make_view( const vfs::node* parent )
	{
		if ( const ViewParameters* params = find_view( parent ) )
		{
			const vfs::node* delegate = params->itsDelegate.get();
			
			ViewFactory factory = params->itsFactory;
			
			ASSERT( delegate != NULL );
			ASSERT( factory  != NULL );
			
			return factory( delegate );
		}
		
		return boost::intrusive_ptr< Ped::View >();
	}
	
	static const vfs::node_ptr& GetViewDelegate( const vfs::node* view )
	{
		const ViewParameters* params = find_view( view->owner() );
		
		if ( params == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return params->itsDelegate;
	}
	
	const vfs::node* GetViewWindowKey( const vfs::node* view )
	{
		const ViewParameters* params = find_view( view->owner() );
		
		return params ? params->itsWindowKey : NULL;
	}
	
	
	bool InvalidateWindowForView( const vfs::node* view )
	{
		const vfs::node* windowKey = GetViewWindowKey( view );
		
		return invalidate_port_WindowRef( windowKey );
	}
	
	
	static void new_view_hardlink( const vfs::node*  that,
	                               const vfs::node*  dest );
	
	static const vfs::file_method_set new_view_file_methods =
	{
		NULL,
		NULL,
		&new_view_hardlink
	};
	
	static const vfs::node_method_set new_view_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&new_view_file_methods
	};
	
	struct new_view_extra
	{
		vfs::fixed_mapping const*  mappings;
		vfs::node_destructor       destructor;
		ViewFactory                view_factory;
		DelegateFactory            delegate_factory;
		size_t                     extra_annex_size;
	};
	
	vfs::node_ptr New_new_view( const vfs::node*           parent,
	                            const plus::string&        name,
	                            ViewFactory                factory,
	                            const vfs::fixed_mapping*  mappings,
	                            vfs::node_destructor       dtor,
	                            size_t                     extra_annex_size,
	                            DelegateFactory            delegate_factory )
	
	{
		vfs::node* result = new vfs::node( parent,
		                                   name,
		                                   S_IFREG | 0,
		                                   &new_view_methods,
		                                   sizeof (new_view_extra) );
		
		new_view_extra& extra = *(new_view_extra*) result->extra();
		
		extra.mappings         = mappings;
		extra.destructor       = dtor;
		extra.view_factory     = factory;
		extra.delegate_factory = delegate_factory;
		extra.extra_annex_size = extra_annex_size;
		
		return result;
	}
	
	vfs::node_ptr create_default_delegate_for_new_view( const vfs::node*     that,
	                                                    const vfs::node*     parent,
	                                                    const plus::string&  name )
	{
		new_view_extra& extra = *(new_view_extra*) that->extra();
		
		vfs::node_ptr delegate = fixed_dir( parent, name, extra.mappings, extra.destructor );
		
		return delegate;
	}
	
	static void new_view_hardlink( const vfs::node*  that,
	                               const vfs::node*  target )
	{
		new_view_extra& extra = *(new_view_extra*) that->extra();
		
		const vfs::node* parent = target->owner();
		
		const vfs::node* key = parent;
		
		vfs::node_ptr delegate = extra.delegate_factory( that, parent, "v" );
		
		add_view_parameters( key, *delegate, extra.view_factory );
		
		mkdir( *target, 0 );  // mode is ignored
	}
	
	
	struct view_extra
	{
		ViewGetter  get;
		ViewSetter  set;
	};
	
	
	static
	Pedestal::View* get_view( const vfs::node* that )
	{
		ASSERT( that != NULL );
		
		const view_extra& extra = *(view_extra*) that->extra();
		
		return extra.get( that->owner(), that->name() );
	}
	
	static
	void set_view( const vfs::node* that, Pedestal::View* view )
	{
		ASSERT( that != NULL );
		
		const view_extra& extra = *(view_extra*) that->extra();
		
		extra.set( that->owner(), that->name(), view );
	}
	
	static void unview_mkdir( const vfs::node* that, mode_t mode )
	{
		const vfs::node* parent = that->owner();
		
		const plus::string& name = that->name();
		
		const vfs::node* windowKey = GetViewWindowKey( parent );
		
		if ( windowKey == NULL )
		{
			ASSERT( parent != NULL );
			
			windowKey = GetViewWindowKey( parent->owner() );
		}
		
		if ( windowKey == NULL )
		{
			windowKey = parent;
		}
		
		add_view_port_key( parent, windowKey );
		
		boost::intrusive_ptr< Ped::View > view = make_view( parent );
		
		set_view( that, view.get() );
		
		// Install and invalidate if window exists
		install_view_in_port( *view, windowKey );
	}
	
	static const vfs::dir_method_set unview_dir_methods =
	{
		NULL,
		NULL,
		&unview_mkdir
	};
	
	static const vfs::node_method_set unview_methods =
	{
		NULL,
		NULL,
		NULL,
		&unview_dir_methods
	};
	
	
	static void view_touch( const vfs::node* that )
	{
		InvalidateWindowForView( that );
	}
	
	static void view_remove( const vfs::node* that )
	{
		const view_extra& extra = *(view_extra*) that->extra();
		
		const vfs::node* parent = that->owner();
		
		const plus::string& name = that->name();
		
		const vfs::node* windowKey = GetViewWindowKey( that );
		
		uninstall_view_from_port( *get_view( that ), windowKey );
		
		set_view( that, Ped::EmptyView::Get() );
		
		RemoveAllViewParameters( parent );
	}
	
	static vfs::node_ptr view_lookup( const vfs::node*     that,
	                                  const plus::string&  name,
	                                  const vfs::node*     parent )
	{
		const plus::string& real_name = name.empty() ? plus::string( "." ) : name;
		
		return lookup( *GetViewDelegate( that ), real_name, NULL );
	}
	
	static void view_listdir( const vfs::node*    that,
	                          vfs::dir_contents&  cache )
	{
		listdir( *GetViewDelegate( that ), cache );
	}
	
	static const vfs::item_method_set view_item_methods =
	{
		NULL,
		NULL,
		&view_touch,
		NULL,
		&view_remove,
	};
	
	static const vfs::dir_method_set view_dir_methods =
	{
		&view_lookup,
		&view_listdir
	};
	
	static const vfs::node_method_set view_methods =
	{
		&view_item_methods,
		NULL,
		NULL,
		&view_dir_methods
	};
	
	static const vfs::node_method_set viewdir_methods =
	{
		NULL,
		NULL,
		NULL,
		&view_dir_methods
	};
	
	vfs::node_ptr New_View( const vfs::node*       parent,
	                        const plus::string&    name,
	                        const View_Accessors&  access )
	{
		const bool exists = view_exists( parent );
		
		const mode_t mode = !exists ? 0
		                  : name.size() == 1 ? S_IFDIR | 0700
		                  :                    S_IFREG | 0600;
		
		const vfs::node_method_set& methods = exists ? view_methods
		                                             : unview_methods;
		
		vfs::node* result = new vfs::node( parent,
		                                   name,
		                                   mode,
		                                   &methods,
		                                   sizeof (view_extra) );
		
		view_extra& extra = *(view_extra*) result->extra();
		
		extra.get = access.get;
		extra.set = access.set;
		
		return result;
	}
	
	vfs::node_ptr new_view_dir( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          /* args */ )
	{
		if ( !view_exists( parent ) )
		{
			return vfs::null();
		}
		
		vfs::node* result = new vfs::node( parent,
		                                   name,
		                                   S_IFDIR | 0700,
		                                   &viewdir_methods );
		
		return result;
	}
	
}
