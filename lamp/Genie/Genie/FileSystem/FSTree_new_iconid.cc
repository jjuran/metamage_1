/*	====================
 *	FSTree_new_iconid.cc
 *	====================
 */

#include "Genie/FileSystem/FSTree_new_iconid.hh"

// Nitrogen
#include "Nitrogen/Icons.h"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"


namespace Pedestal
{
	
	class IconID : public View
	{
		public:
			struct Undefined {};
			
			void Draw( const Rect& bounds, bool erasing );
			
			virtual Nitrogen::ResID ID() const = 0;
			
			virtual Nitrogen::IconAlignmentType Alignment() const  { return Nitrogen::kAlignNone; }
			
			virtual Nitrogen::IconTransformType Transform() const  { return Nitrogen::kTransformNone; }
	};
	
	namespace N = Nitrogen;
	
	void IconID::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			N::EraseRect( bounds );
		}
		
		try
		{
			N::PlotIconID( bounds, Alignment(), Transform(), ID() );
		}
		catch ( const Undefined& )
		{
		}
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct IconID_Parameters
	{
		N::ResID              id;
		N::IconAlignmentType  align;
		N::IconTransformType  xform;
		
		IconID_Parameters() : id(), align(), xform()
		{
		}
	};
	
	typedef std::map< const FSTree*, IconID_Parameters > IconIDMap;
	
	static IconIDMap gIconIDMap;
	
	
	class IconID : public Ped::IconID
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			IconID( Key key ) : itsKey( key )
			{
			}
			
			Nitrogen::ResID ID() const;
			
			Nitrogen::IconAlignmentType Alignment() const;
			
			Nitrogen::IconTransformType Transform() const;
	};
	
	N::ResID IconID::ID() const
	{
		IconIDMap::const_iterator it = gIconIDMap.find( itsKey );
		
		if ( it == gIconIDMap.end() )
		{
			throw Undefined();
		}
		
		return it->second.id;
	}
	
	N::IconAlignmentType IconID::Alignment() const
	{
		IconIDMap::const_iterator it = gIconIDMap.find( itsKey );
		
		if ( it == gIconIDMap.end() )
		{
			throw Undefined();
		}
		
		return it->second.align;
	}
	
	N::IconTransformType IconID::Transform() const
	{
		IconIDMap::const_iterator it = gIconIDMap.find( itsKey );
		
		if ( it == gIconIDMap.end() )
		{
			throw Undefined();
		}
		
		return it->second.xform;
	}
	
	boost::shared_ptr< Ped::View > IconIDFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new IconID( delegate ) );
	}
	
	
	void FSTree_new_iconid::DestroyDelegate( const FSTree* delegate )
	{
		gIconIDMap.erase( delegate );
	}
	
	
	namespace
	{
		
		N::ResID& IconID( const FSTree* view )
		{
			return gIconIDMap[ view ].id;
		}
		
		N::IconAlignmentType& Alignment( const FSTree* view )
		{
			return gIconIDMap[ view ].align;
		}
		
		N::IconTransformType& Transform( const FSTree* view )
		{
			return gIconIDMap[ view ].xform;
		}
		
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
	
	const FSTree_Premapped::Mapping IconID_view_Mappings[] =
	{
		{ "id", &Property_Factory< View_Property< Integer_Scribe< N::ResID >, IconID > > },
		
		{ "align", &Property_Factory< View_Property< Integer_Scribe< N::IconAlignmentType >, Alignment > > },
		{ "xform", &Property_Factory< View_Property< Integer_Scribe< N::IconTransformType >, Transform > > },
		
		{ NULL, NULL }
	};
	
}

