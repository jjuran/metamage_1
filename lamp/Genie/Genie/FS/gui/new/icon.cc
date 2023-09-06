/*
	gui/new/icon.cc
	---------------
*/

#include "Genie/FS/gui/new/icon.hh"

// mac-qd-utils
#include "mac_qd/globals/thePort_window.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"

// Pedestal
#include "Pedestal/View.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/Icon_data.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	struct Icon_Parameters
	{
		boost::intrusive_ptr< IconData >  data;
		IconAlignmentType                 align;
		IconTransformType                 xform;
		SInt16                            label;
		bool                              selected;
		bool                              disabling;
		
		Icon_Parameters() : align(), xform(), label(), selected(), disabling()
		{
			xform = kTransformDisabled;
		}
	};
	
	typedef plus::simple_map< const vfs::node*, Icon_Parameters > IconMap;
	
	static IconMap gIconMap;
	
	
	class Icon : public Ped::View
	{
		private:
			typedef const vfs::node* Key;
			
			Key itsKey;
		
		public:
			Icon( Key key ) : itsKey( key )
			{
			}
			
			void Draw( const Rect& bounds, bool erasing );
			
			void Activate( bool activating );
	};
	
	static inline IconTransformType
	//
	CombinedIconTransforms( const Icon_Parameters& params )
	{
		IconTransformType state    = params.xform;
		IconTransformType label    = params.label    * kTransformLabel1;
		IconTransformType selected = params.selected * kTransformSelected;
		
		return state | label | selected;
	}
	
	void Icon::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			EraseRect( &bounds );
		}
		
		Icon_Parameters& params = gIconMap[ itsKey ];
		
		if ( params.data.get() )
		{
			params.data->Plot( bounds,
			                   params.align,
			                   CombinedIconTransforms( params ) );
		}
	}
	
	void Icon::Activate( bool activating )
	{
		Icon_Parameters& params = gIconMap[ itsKey ];
		
		if ( params.disabling )
		{
			params.xform = activating ? kTransformNone
			                          : kTransformDisabled;
			
			mac::ui::invalidate_window( mac::qd::thePort_window() );
		}
	}
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new Icon( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gIconMap.erase( delegate );
	}
	
	
	static
	short& int_field( const vfs::node* view, const plus::string& name )
	{
		Icon_Parameters& params = gIconMap[ view ];
		
		const char c = name[ 0 ];
		
		short& value = c == 'a' ? params.align
		             : c == 'x' ? params.xform
		             : c == 'l' ? params.label
		             :            *(short*) 0;
		
		return value;
	}
	
	static
	bool& flag_field( const vfs::node* view, const plus::string& name )
	{
		Icon_Parameters& params = gIconMap[ view ];
		
		const char c = name[ 0 ];
		
		bool& value = c == 'd' ? params.disabling
		                       : params.selected;
		
		return value;
	}
	
	typedef plus::serialize_unsigned< short > serialize_int;
	
	static
	void int_get( plus::var_string& result, const vfs::node* view, bool binary, const plus::string& name )
	{
		short value = int_field( view, name );
		
		serialize_int::deconstruct::apply( result, value, binary );
	}
	
	static
	void int_set( const vfs::node* view, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		short& value = int_field( view, name );
		
		value = serialize_int::reconstruct::apply( begin, end, binary );
		
		InvalidateWindowForView( view );
	}
	
	static const vfs::property_params icon_int_params =
	{
		serialize_int::fixed_size,
		(vfs::property_get_hook) &int_get,
		(vfs::property_set_hook) &int_set,
	};
	
	static
	void flag_get( plus::var_string& result, const vfs::node* view, bool binary, const plus::string& name )
	{
		bool value = flag_field( view, name );
		
		plus::serialize_bool::deconstruct::apply( result, value, binary );
	}
	
	static
	void flag_set( const vfs::node* view, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		bool& value = flag_field( view, name );
		
		value = plus::serialize_bool::reconstruct::apply( begin, end, binary );
		
		InvalidateWindowForView( view );
	}
	
	static const vfs::property_params icon_flag_params =
	{
		plus::serialize_bool::fixed_size,
		(vfs::property_get_hook) &flag_get,
		(vfs::property_set_hook) &flag_set,
	};
	
	static vfs::node_ptr Data_Factory( const vfs::node*     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		boost::intrusive_ptr< IconData >& data = gIconMap[ parent ].data;
		
		if ( data.get() == NULL )
		{
			data.reset( new IconData );
		}
		
		return New_FSTree_Icon_data( parent, name, data.get() );
	}
	
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "data", &Data_Factory },
		
		{ "align",     PROPERTY( icon_int  ) },
		{ "xform",     PROPERTY( icon_int  ) },
		{ "label",     PROPERTY( icon_int  ) },
		{ "selected",  PROPERTY( icon_flag ) },
		{ "disabling", PROPERTY( icon_flag ) },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_icon( const vfs::node*     parent,
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
