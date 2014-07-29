/*
	gui/new/progress.cc
	-------------------
*/

#include "Genie/FS/gui/new/progress.hh"

// plus
#include "plus/serialize.hh"
#include "plus/string.hh"

// vfs
#include "vfs/node.hh"

// Pedestal
#include "Pedestal/ProgressBar.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	struct progress_extra : vfs::fixed_dir_extra
	{
		int value;  // permyriad, i.e. units of 1/10000
	};
	
	const size_t progress_annex_size = sizeof (progress_extra) - sizeof (vfs::fixed_dir_extra);
	
	
	class ProgressBar : public Ped::ProgressBar
	{
		private:
			typedef const vfs::node* Key;
			
			Key  its_key;
		
		public:
			ProgressBar( Key key ) : its_key( key )
			{
			}
			
			int Value() const;
	};
	
	int ProgressBar::Value() const
	{
		progress_extra& extra = *(progress_extra*) its_key->extra();
		
		return extra.value;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		progress_extra& extra = *(progress_extra*) delegate->extra();
		
		extra.value = 0;
		
		return new ProgressBar( delegate );
	}
	
	
	namespace
	{
		
		int& Value( const vfs::node* view )
		{
			progress_extra& extra = *(progress_extra*) view->extra();
			
			return extra.value;
		}
		
	}
	
	typedef View_Property< plus::serialize_int< int >, Value > Value_Property;
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "value", PROPERTY( Value_Property ) },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_new_progress( const vfs::node*     parent,
	                                const plus::string&  name,
	                                const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     NULL,
		                     progress_annex_size );
	}
	
}

