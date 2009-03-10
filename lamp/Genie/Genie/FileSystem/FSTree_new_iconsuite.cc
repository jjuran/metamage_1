/*	=======================
 *	FSTree_new_iconsuite.cc
 *	=======================
 */

#include "Genie/FileSystem/FSTree_new_iconsuite.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/Icons.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_IconSuite.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/Icons.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct IconSuite_Parameters : Icon_Parameters
	{
		NN::Shared< N::IconSuiteRef >  ref;
	};
	
	typedef std::map< const FSTree*, IconSuite_Parameters > IconSuiteMap;
	
	static IconSuiteMap gIconSuiteMap;
	
	
	class IconSuite : public Ped::IconSuite
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			IconSuite( Key key ) : itsKey( key )
			{
			}
			
			Nitrogen::IconSuiteRef Ref() const;
			
			Nitrogen::IconAlignmentType Alignment() const;
			
			Nitrogen::IconTransformType Transform() const;
	};
	
	N::IconSuiteRef IconSuite::Ref() const
	{
		return gIconSuiteMap[ itsKey ].ref;
	}
	
	N::IconAlignmentType IconSuite::Alignment() const
	{
		return gIconSuiteMap[ itsKey ].align;
	}
	
	N::IconTransformType IconSuite::Transform() const
	{
		return gIconSuiteMap[ itsKey ].xform;
	}
	
	boost::shared_ptr< Ped::View > IconSuiteFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new IconSuite( delegate ) );
	}
	
	
	void FSTree_new_iconsuite::DestroyDelegate( const FSTree* delegate )
	{
		gIconSuiteMap.erase( delegate );
	}
	
	
	static std::size_t GetHandleSize_Checked( N::Handle h )
	{
		return h ? N::GetHandleSize( h ) : 0;
	}
	
	class FSTree_IconSuite_ref : public FSTree
	{
		public:
			FSTree_IconSuite_ref( const FSTreePtr&    parent,
			                      const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			N::IconSuiteRef Ref() const  { return gIconSuiteMap[ ParentRef().get() ].ref; }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			void Attach( const FSTreePtr& target ) const;
	};
	
	void FSTree_IconSuite_ref::Attach( const FSTreePtr& target ) const
	{
		const FSTree* view = ParentRef().get();
		
		gIconSuiteMap[ view ].ref = Fetch_IconSuite();
		
		InvalidateWindowForView( view );
	}
	
	
	namespace
	{
		
		N::IconAlignmentType& Alignment( const FSTree* view )
		{
			return gIconSuiteMap[ view ].align;
		}
		
		N::IconTransformType& Transform( const FSTree* view )
		{
			return gIconSuiteMap[ view ].xform;
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
	
	const FSTree_Premapped::Mapping IconSuite_view_Mappings[] =
	{
		{ "ref", &Basic_Factory< FSTree_IconSuite_ref > },
		
		{ "align", &Property_Factory< View_Property< Integer_Scribe< N::IconAlignmentType >, Alignment > > },
		{ "xform", &Property_Factory< View_Property< Integer_Scribe< N::IconTransformType >, Transform > > },
		
		{ NULL, NULL }
	};
	
}

