/*	===================
 *	FSTree_new_frame.cc
 *	===================
 */

#include "Genie/FileSystem/FSTree_new_frame.hh"

// POSIX
#include <fcntl.h>

// Pedestal
#include "Pedestal/EmptyView.hh"
#include "Pedestal/Frame.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct FrameParameters
	{
		short  itsMargin;
		
		boost::shared_ptr< Ped::View >  itsSubview;
		
		FrameParameters() : itsMargin( 0 ),
		                    itsSubview( new Ped::EmptyView )
		{
		}
	};
	
	typedef std::map< const FSTree*, FrameParameters > FrameParametersMap;
	
	static FrameParametersMap gFrameParametersMap;
	
	
	class Frame : public Ped::Frame
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			Frame( Key key ) : itsKey( key )
			{
			}
			
			short Margin() const;
			
			Ped::View& Subview();
	};
	
	short Frame::Margin() const
	{
		FrameParametersMap::const_iterator it = gFrameParametersMap.find( itsKey );
		
		if ( it != gFrameParametersMap.end() )
		{
			return it->second.itsMargin;
		}
		
		return 0;
	}
	
	Ped::View& Frame::Subview()
	{
		boost::shared_ptr< Ped::View >& subview = gFrameParametersMap[ itsKey ].itsSubview;
		
		if ( subview.get() == NULL )
		{
			subview.reset( new Ped::EmptyView );
		}
		
		return *subview;
	}
	
	std::auto_ptr< Ped::View > FrameFactory( const FSTree* delegate )
	{
		return std::auto_ptr< Ped::View >( new Frame( delegate ) );
	}
	
	
	void FSTree_new_frame::DestroyDelegate( const FSTree* delegate )
	{
		gFrameParametersMap.erase( delegate );
	}
	
	
	class FSTree_Frame_view : public FSTree_View
	{
		public:
			FSTree_Frame_view( const FSTreePtr&    parent,
			                   const std::string&  name ) : FSTree_View( parent, name )
			{
			}
			
			void DeleteCustomParameters() const;
			
			void AddCustomParameters( std::auto_ptr< Ped::View > view ) const;
	};
	
	void FSTree_Frame_view::DeleteCustomParameters() const
	{
		gFrameParametersMap[ ParentKey() ].itsSubview.reset();
	}
	
	void FSTree_Frame_view::AddCustomParameters( std::auto_ptr< Ped::View > view ) const
	{
		gFrameParametersMap[ ParentKey() ].itsSubview = view;
	}
	
	
	static std::string ReadMargin( const FSTree* that )
	{
		const FSTree* view = GetViewKey( that );
		
		return NN::Convert< std::string >( gFrameParametersMap[ view ].itsMargin );
	}
	
	static void WriteMargin( const FSTree* that, const char* begin, const char* end )
	{
		const FSTree* view = GetViewKey( that );
		
		// *end == '\n'
		
		gFrameParametersMap[ view ].itsMargin = N::ResID( std::atoi( begin ) );
		
		InvalidateWindowForView( view );
	}
	
	
	static FSTreePtr MarginFactory( const FSTreePtr&    parent,
	                                const std::string&  name )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &ReadMargin,
		                                       &WriteMargin ) );
	}
	
	const FSTree_Premapped::Mapping Frame_view_Mappings[] =
	{
		{ "margin", &MarginFactory },
		
		{ "v", &Basic_Factory< FSTree_Frame_view >, true },
		
		{ NULL, NULL }
	};
	
}

