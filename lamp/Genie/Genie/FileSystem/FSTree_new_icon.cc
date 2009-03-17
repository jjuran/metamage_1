/*	==================
 *	FSTree_new_icon.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_new_icon.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Pedestal
#include "Pedestal/Icons.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/Icons.hh"
#include "Genie/FileSystem/Icon_data.hh"


namespace Nitrogen
{
	
	static const IconTransformType kTransformDisabled = IconTransformType( ::kTransformDisabled );
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct PlainIcon_Parameters : Icon_Parameters
	{
		boost::shared_ptr< IconData >  data;
	};
	
	typedef std::map< const FSTree*, PlainIcon_Parameters > PlainIconMap;
	
	static PlainIconMap gPlainIconMap;
	
	
	class PlainIcon : public Ped::View
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			PlainIcon( Key key ) : itsKey( key )
			{
			}
			
			void Draw( const Rect& bounds, bool erasing );
			
			void Activate( bool activating );
	};
	
	void PlainIcon::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			N::EraseRect( bounds );
		}
		
		PlainIcon_Parameters& params = gPlainIconMap[ itsKey ];
		
		if ( params.data.get() )
		{
			params.data->Plot( bounds,
			                   params.align,
			                   CombinedIconTransforms( params ) );
		}
	}
	
	void PlainIcon::Activate( bool activating )
	{
		Icon_Parameters& params = gPlainIconMap[ itsKey ];
		
		if ( params.disabling )
		{
			params.xform = activating ? N::kTransformNone
			                          : N::kTransformDisabled;
			
			N::InvalRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
		}
	}
	
	
	boost::shared_ptr< Ped::View > IconFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new PlainIcon( delegate ) );
	}
	
	
	void FSTree_new_icon::DestroyDelegate( const FSTree* delegate )
	{
		gPlainIconMap.erase( delegate );
	}
	
	
	namespace
	{
		
		N::IconAlignmentType& Alignment( const FSTree* view )
		{
			return gPlainIconMap[ view ].align;
		}
		
		N::IconTransformType& Transform( const FSTree* view )
		{
			return gPlainIconMap[ view ].xform;
		}
		
		char& Label( const FSTree* view )
		{
			return gPlainIconMap[ view ].label;
		}
		
		bool& Selected( const FSTree* view )
		{
			return gPlainIconMap[ view ].selected;
		}
		
		bool& Disabling( const FSTree* view )
		{
			return gPlainIconMap[ view ].disabling;
		}
		
	}
	
	static FSTreePtr Data_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		boost::shared_ptr< IconData >& data = gPlainIconMap[ parent.get() ].data;
		
		if ( data.get() == NULL )
		{
			data = boost::shared_ptr< IconData >( new IconData );
		}
		
		return FSTreePtr( new FSTree_Icon_data( parent, name, data ) );
	}
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Get,
		                                       &Property::Set ) );
	}
	
	const FSTree_Premapped::Mapping Icon_view_Mappings[] =
	{
		{ "data", &Data_Factory },
		
		{ "align", &Property_Factory< View_Property< Integer_Scribe< N::IconAlignmentType >, Alignment > > },
		{ "xform", &Property_Factory< View_Property< Integer_Scribe< N::IconTransformType >, Transform > > },
		{ "label", &Property_Factory< View_Property< Integer_Scribe< char >, Label > > },
		{ "selected",  &Property_Factory< View_Property< Boolean_Scribe, Selected  > > },
		{ "disabling", &Property_Factory< View_Property< Boolean_Scribe, Disabling > > },
		
		{ NULL, NULL }
	};
	
}

