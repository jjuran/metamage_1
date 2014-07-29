/*	========
 *	Views.hh
 *	========
 */

#ifndef GENIE_FILESYSTEM_VIEWS_HH
#define GENIE_FILESYSTEM_VIEWS_HH

// plus
#include "plus/var_string.hh"

// vfs
#include "vfs/node/types/fixed_dir.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/FS/property.hh"


namespace Pedestal
{
	
	class View;
	
}

namespace Genie
{
	
	void RemoveAllViewParameters( const FSTree* parent );
	
	
	typedef boost::intrusive_ptr< Pedestal::View > (*ViewFactory)( const FSTree* delegate );
	
	typedef FSTreePtr (*DelegateFactory)( const FSTree*,
	                                      const FSTree*,
	                                      const plus::string& );
	
	
	const FSTree* GetViewWindowKey( const FSTree* view );
	
	bool InvalidateWindowForView( const FSTree* view );
	
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const FSTree* ) >
	struct Const_View_Property : readonly_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			typedef typename Serialize::result_type result_type;
			
			const result_type& value = Access( that );
			
			Serialize::deconstruct::apply( result, value, binary );
		}
	};
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const FSTree* ) >
	struct View_Property : readwrite_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			typedef typename Serialize::result_type result_type;
			
			const result_type& value = Access( that );
			
			Serialize::deconstruct::apply( result, value, binary );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			Access( that ) = Serialize::reconstruct::apply( begin, end, binary );
			
			InvalidateWindowForView( that );
		}
	};
	
	
	FSTreePtr create_default_delegate_for_new_view( const FSTree*        node,
	                                                const FSTree*        parent,
	                                                const plus::string&  name );
	
	FSTreePtr New_new_view( const FSTree*              parent,
	                        const plus::string&        name,
	                        ViewFactory                factory,
	                        const vfs::fixed_mapping*  mappings         = vfs::empty_mappings,
	                        vfs::node_destructor       dtor             = NULL,
	                        size_t                     extra_annex_size = 0,
	                        DelegateFactory            delegate_factory = &create_default_delegate_for_new_view );
	
	typedef boost::intrusive_ptr< Pedestal::View >&
	//
	(*ViewGetter)( const FSTree*, const plus::string& name );
	
	typedef void (*ViewPurger)( const FSTree*, const plus::string& name );
	
	FSTreePtr New_View( const FSTree*        parent,
	                    const plus::string&  name,
	                    ViewGetter           get,
	                    ViewPurger           purge );
	
	vfs::node_ptr new_view_dir( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          /* args */ = NULL );
	
}

#endif

