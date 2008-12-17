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
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/IO/PseudoFile.hh"
#include "Genie/IO/QueryFile.hh"


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
	
	
	template < class FSTree_Type >
	static FSTreePtr Factory( const FSTreePtr&    parent,
	                          const std::string&  name )
	{
		return FSTreePtr( new FSTree_Type( parent, name ) );
	}
	
	
	class Frame_margin_Property
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			Frame_margin_Property( const Key& key ) : itsKey( key )
			{
			}
			
			std::string Get() const
			{
				FrameParametersMap::const_iterator it = gFrameParametersMap.find( itsKey );
				
				if ( it == gFrameParametersMap.end() )
				{
					return "";  // empty file
				}
				
				std::string output = NN::Convert< std::string >( it->second.itsMargin );
				
				output += "\n";
				
				return output;
			}
			
			void Set( const std::string& s )
			{
				gFrameParametersMap[ itsKey ].itsMargin = std::atoi( s.c_str() );
				
				const FSTree* view = itsKey;
				
				InvalidateWindowForView( view );
			}
	};
	
	class FSTree_Generic_REF_Property_Base : public FSTree
	{
		public:
			FSTree_Generic_REF_Property_Base( const FSTreePtr&    parent,
			                                  const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
	};
	
	template < class Property >
	class FSTree_Generic_REF_Property : public FSTree_Generic_REF_Property_Base
	{
		private:
			typedef FSTree_Generic_REF_Property_Base Base;
			
			Property itsProperty;
		
		public:
			FSTree_Generic_REF_Property( const FSTreePtr&    parent,
			                             const std::string&  name ) : Base( parent, name ),
			                                                          itsProperty( parent.get() )
			{
			}
			
			off_t GetEOF() const  { return itsProperty.Get().size(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	template < class Property >
	boost::shared_ptr< IOHandle > FSTree_Generic_REF_Property< Property >::Open( OpenFlags flags ) const
	{
		IOHandle* result = NULL;
		
		if ( flags == O_RDONLY )
		{
			result = new QueryFileHandle( shared_from_this(),
			                              flags,
			                              itsProperty.Get() );
		}
		else if ( (flags & ~O_CREAT) == (O_WRONLY | O_TRUNC) )
		{
			result = new PseudoFileHandle< Property >( shared_from_this(),
			                                           flags,
			                                           itsProperty );
		}
		else
		{
			throw poseven::errno_t( EINVAL );
		}
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	typedef FSTree_Generic_REF_Property< Frame_margin_Property > FSTree_Frame_margin;
	
	
	
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
	
	
	const Functional_Traits< void >::Mapping Frame_view_Mappings[] =
	{
		{ "margin", &Factory< FSTree_Frame_margin > },
		
		{ "v", &Factory< FSTree_Frame_view >, true },
		
		{ NULL, NULL }
	};
	
}

