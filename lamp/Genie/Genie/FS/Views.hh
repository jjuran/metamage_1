/*	========
 *	Views.hh
 *	========
 */

#ifndef GENIE_FILESYSTEM_VIEWS_HH
#define GENIE_FILESYSTEM_VIEWS_HH

// plus
#include "plus/var_string.hh"

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
	
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const vfs::node* ) >
	struct Const_View_Property : vfs::readonly_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			typedef typename Serialize::result_type result_type;
			
			const result_type& value = Access( that );
			
			Serialize::deconstruct::apply( result, value, binary );
		}
	};
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const vfs::node* ) >
	struct View_Property : vfs::readwrite_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			typedef typename Serialize::result_type result_type;
			
			const result_type& value = Access( that );
			
			Serialize::deconstruct::apply( result, value, binary );
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			Access( that ) = Serialize::reconstruct::apply( begin, end, binary );
			
			InvalidateWindowForView( that );
		}
	};
	
	
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
	
	typedef boost::intrusive_ptr< Pedestal::View >&
	//
	(*ViewGetter)( const vfs::node*, const plus::string& name );
	
	typedef void (*ViewPurger)( const vfs::node*, const plus::string& name );
	
	vfs::node_ptr New_View( const vfs::node*     parent,
	                        const plus::string&  name,
	                        ViewGetter           get,
	                        ViewPurger           purge );
	
	vfs::node_ptr new_view_dir( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          /* args */ = NULL );
	
}

#endif
