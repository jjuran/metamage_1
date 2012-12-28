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
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	struct ProgressParameters
	{
		int value;  // permyriad, i.e. units of 1/10000
		
		ProgressParameters() : value()
		{
		}
	};
	
	typedef simple_map< const vfs::node*, ProgressParameters > ProgressParametersMap;
	
	static ProgressParametersMap gProgressParametersMap;
	
	
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
		if ( ProgressParameters* it = gProgressParametersMap.find( its_key ) )
		{
			return it->value;
		}
		
		return 0;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new ProgressBar( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gProgressParametersMap.erase( delegate );
	}
	
	
	namespace
	{
		
		int& Value( const vfs::node* view )
		{
			return gProgressParametersMap[ view ].value;
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
		                     &DestroyDelegate );
	}
	
}

