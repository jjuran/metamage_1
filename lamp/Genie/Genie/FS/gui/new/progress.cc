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
#include "vfs/node/types/property_file.hh"

// Pedestal
#include "Pedestal/ProgressBar.hh"

// Genie
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
	
	
	typedef plus::serialize_int< int > serialize;
	
	static
	void get( plus::var_string& result, const vfs::node* view, bool binary )
	{
		progress_extra& extra = *(progress_extra*) view->extra();
		
		serialize::deconstruct::apply( result, extra.value, binary );
	}
	
	static
	void set( const vfs::node* view, const char* begin, const char* end, bool binary )
	{
		progress_extra& extra = *(progress_extra*) view->extra();
		
		extra.value = serialize::reconstruct::apply( begin, end, binary );
		
		InvalidateWindowForView( view );
	}
	
	static const vfs::property_params value_params =
	{
		serialize::fixed_size,
		&get,
		&set,
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "value", PROPERTY( value ) },
		
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
