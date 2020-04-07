/*	========
 *	Views.hh
 *	========
 */

#ifndef GENIE_FILESYSTEM_VIEWS_HH
#define GENIE_FILESYSTEM_VIEWS_HH

// vfs
#include "vfs/node_ptr.hh"
#include "vfs/property.hh"
#include "vfs/node/types/fixed_dir.hh"


namespace Pedestal
{
	
	class View;
	
}

namespace Genie
{
	
	void RemoveAllViewParameters( const vfs::node* parent );
	
	
	typedef boost::intrusive_ptr< Pedestal::View > (*ViewFactory)( const vfs::node* delegate );
	
	typedef vfs::node_ptr (*DelegateFactory)( const vfs::node*,
	                                          const vfs::node*,
	                                          const plus::string& );
	
	
	const vfs::node* GetViewWindowKey( const vfs::node* view );
	
	bool InvalidateWindowForView( const vfs::node* view );
	
	
	vfs::node_ptr create_default_delegate_for_new_view( const vfs::node*     node,
	                                                    const vfs::node*     parent,
	                                                    const plus::string&  name );
	
	vfs::node_ptr New_new_view( const vfs::node*           parent,
	                            const plus::string&        name,
	                            ViewFactory                factory,
	                            const vfs::fixed_mapping*  mappings         = vfs::empty_mappings,
	                            vfs::node_destructor       dtor             = NULL,
	                            size_t                     extra_annex_size = 0,
	                            DelegateFactory            delegate_factory = &create_default_delegate_for_new_view );
	
	typedef Pedestal::View* (*ViewGetter)( const vfs::node*,
	                                       const plus::string&  name );
	
	typedef void (*ViewSetter)( const vfs::node*,
	                            const plus::string&  name,
	                            Pedestal::View*      view );
	
	struct View_Accessors
	{
		ViewGetter  get;
		ViewSetter  set;
	};
	
	vfs::node_ptr New_View( const vfs::node*       parent,
	                        const plus::string&    name,
	                        const View_Accessors&  access );
	
	vfs::node_ptr new_view_dir( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          /* args */ = NULL );
	
}

#endif
