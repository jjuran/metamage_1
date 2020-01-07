/*
	gui/new/icon.cc
	---------------
*/

#include "Genie/FS/gui/new/icon.hh"

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

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
	
	namespace N = Nitrogen;
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
			N::EraseRect( bounds );
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
			
			N::InvalRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
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
	
	
	namespace
	{
		
		IconAlignmentType& Alignment( const vfs::node* view )
		{
			return gIconMap[ view ].align;
		}
		
		IconTransformType& Transform( const vfs::node* view )
		{
			return gIconMap[ view ].xform;
		}
		
		SInt16& Label( const vfs::node* view )
		{
			return gIconMap[ view ].label;
		}
		
		bool& Selected( const vfs::node* view )
		{
			return gIconMap[ view ].selected;
		}
		
		bool& Disabling( const vfs::node* view )
		{
			return gIconMap[ view ].disabling;
		}
		
	}
	
	static vfs::node_ptr Data_Factory( const vfs::node*     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		boost::intrusive_ptr< IconData >& data = gIconMap[ parent ].data;
		
		if ( data.get() == NULL )
		{
			data = boost::intrusive_ptr< IconData >( new IconData );
		}
		
		return New_FSTree_Icon_data( parent, name, data );
	}
	
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	typedef View_Property< plus::serialize_unsigned< IconAlignmentType >, Alignment >  Alignment_Property;
	typedef View_Property< plus::serialize_unsigned< IconTransformType >, Transform >  Transform_Property;
	
	typedef View_Property< plus::serialize_unsigned< SInt16 >, Label >  Label_Property;
	
	typedef View_Property< plus::serialize_bool, Selected  >  Selected_Property;
	typedef View_Property< plus::serialize_bool, Disabling >  Disabling_Property;
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "data", &Data_Factory },
		
		{ "align",     PROPERTY( Alignment_Property ) },
		{ "xform",     PROPERTY( Transform_Property ) },
		{ "label",     PROPERTY( Label_Property     ) },
		{ "selected",  PROPERTY( Selected_Property  ) },
		{ "disabling", PROPERTY( Disabling_Property ) },
		
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
