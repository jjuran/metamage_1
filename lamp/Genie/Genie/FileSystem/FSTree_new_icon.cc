/*	==================
 *	FSTree_new_icon.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_new_icon.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/Icons.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/Icons.hh"
#include "Genie/FileSystem/PlainIcon_data.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct PlainIcon_Parameters : Icon_Parameters
	{
		NN::Shared< N::Handle >  data;
	};
	
	typedef std::map< const FSTree*, PlainIcon_Parameters > PlainIconMap;
	
	static PlainIconMap gPlainIconMap;
	
	
	class PlainIcon : public Ped::PlainIcon
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			PlainIcon( Key key ) : itsKey( key )
			{
			}
			
			Nitrogen::Handle Data() const;
			
			Nitrogen::IconAlignmentType Alignment() const;
			
			Nitrogen::IconTransformType Transform() const;
	};
	
	N::Handle PlainIcon::Data() const
	{
		return gPlainIconMap[ itsKey ].data;
	}
	
	N::IconAlignmentType PlainIcon::Alignment() const
	{
		return gPlainIconMap[ itsKey ].align;
	}
	
	N::IconTransformType PlainIcon::Transform() const
	{
		return gPlainIconMap[ itsKey ].xform;
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
		
	}
	
	static FSTreePtr Data_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		NN::Shared< N::Handle >& data = gPlainIconMap[ parent.get() ].data;
		
		if ( data.Get() == NULL )
		{
			data = N::NewHandle( 0 );  // handle must be allocated
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
		
		{ NULL, NULL }
	};
	
}

