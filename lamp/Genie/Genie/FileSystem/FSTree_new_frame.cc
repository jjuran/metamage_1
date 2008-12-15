/*	===================
 *	FSTree_new_frame.cc
 *	===================
 */

#include "Genie/FileSystem/FSTree_new_frame.hh"

// POSIX
#include <fcntl.h>

// Nucleus
#include "Nucleus/Saved.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Pedestal
#include "Pedestal/EmptyView.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/Views.hh"
#include "Genie/IO/PseudoFile.hh"
#include "Genie/IO/QueryFile.hh"


namespace Pedestal
{
	
	class Frame : public View
	{
		public:
			Frame( const Rect& bounds )
			{
			}
			
			virtual short Margin() const = 0;
			
			virtual View& Subview() = 0;
			
			void Draw( const Rect& bounds );
	};
	
}

namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void Frame::Draw( const Rect& bounds )
	{
		short margin = Margin();
		
		Rect top    = bounds;
		Rect bottom = bounds;
		
		Rect left  = bounds;
		Rect right = bounds;
		
		top.bottom = top.top   + margin;
		left.right = left.left + margin;
		
		bottom.top = bottom.bottom - margin;
		right.left = right.right   - margin;
		
		N::EraseRect( top    );
		N::EraseRect( left   );
		N::EraseRect( right  );
		N::EraseRect( bottom );
		
		Subview().Draw( N::InsetRect( bounds, margin, margin ) );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	static void InvalidateCurrentWindow()
	{
		N::InvalRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	static void InvalidateWindowRef( N::WindowRef window )
	{
		NN::Saved< N::Port_Value > savePort;
		
		N::SetPortWindowPort( window );
		
		InvalidateCurrentWindow();
	}
	
	
	struct FrameParameters
	{
		short  itsMargin;
		
		boost::shared_ptr< Ped::View >  itsSubview;
		
		FrameParameters() : itsMargin( 0 ),
		                    itsSubview( new Ped::EmptyView )
		{
		}
	};
	
	extern const Functional_Traits< void >::Mapping Frame_view_Mappings[];
	
	typedef std::map< const FSTree*, FrameParameters > FrameParametersMap;
	
	static FrameParametersMap gFrameParametersMap;
	
	
	class Frame : public Ped::Frame
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			Frame( const Rect& bounds, Key key ) : Ped::Frame( bounds ), itsKey( key )
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
	
	
	class FrameFactory : public ViewFactory
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			FrameFactory( Key key ) : itsKey( key )
			{
			}
			
			std::auto_ptr< Ped::View > operator()( const Rect& bounds ) const;
	};
	
	std::auto_ptr< Ped::View > FrameFactory::operator()( const Rect& bounds ) const
	{
		typedef Frame View;
		
		std::auto_ptr< Ped::View > view( new View( bounds, itsKey ) );
		
		return view;
	}
	
	namespace
	{
		
		void DestroyDelegate( const FSTree* delegate )
		{
			gFrameParametersMap.erase( delegate );
		}
		
	}
	
	void FSTree_new_frame::HardLink( const FSTreePtr& target ) const
	{
		const FSTreePtr& parent = target->Parent();
		
		const FSTree* key = parent.get();
		
		const std::string& name = target->Name();
		
		FSTreePtr delegate = Premapped_Factory< Frame_view_Mappings, &DestroyDelegate >( parent, name );
		
		boost::shared_ptr< ViewFactory > factory( new FrameFactory( delegate.get() ) );
		
		AddViewParameters( key, name, factory, delegate );
		
		target->CreateDirectory( 0 );  // mode is ignored
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
				
				const FSTree* parentKey = view->Parent().get();
				
				const FSTree* windowKey = GetViewWindowKey( parentKey, view->Name() );
				
				InvalidateWindowRef( GetWindowRef( windowKey ) );
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
	
	
	class FSTree_Frame_view : public FSTree
	{
		public:
			FSTree_Frame_view( const FSTreePtr&    parent,
			                   const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			const FSTree* ParentKey() const  { return Parent().get(); }
			
			const FSTree* WindowKey() const  { return GetViewWindowKey( ParentKey(), Name() ); }
			
			bool IsDirectory() const  { return Exists(); }
			
			bool Exists() const  { return GetViewDelegate( ParentKey(), Name() ) != NULL; }
			
			void SetTimes() const;
			
			void Delete() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup( const std::string& name ) const;
			
			FSIteratorPtr Iterate() const;
	};
	
	static void InvalidateWindow( const FSTree* key )
	{
		if ( N::WindowRef window = GetWindowRef( key ) )
		{
			InvalidateWindowRef( window );
		}
		else
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	void FSTree_Frame_view::SetTimes() const
	{
		InvalidateWindow( WindowKey() );
	}
	
	void FSTree_Frame_view::Delete() const
	{
		const FSTree* parent = Parent().get();
		
		const std::string& name = Name();
		
		if ( ViewExists( parent, name ) )
		{
			gFrameParametersMap[ parent ].itsSubview.reset();
			
			InvalidateWindow( WindowKey() );
			
			RemoveViewParameters( parent, name );
		}
		else
		{
			p7::throw_errno( ENOENT );
		}
	}
	
	void FSTree_Frame_view::CreateDirectory( mode_t mode ) const
	{
		const FSTree* parent = Parent().get();
		
		const std::string& name = Name();
		
		if ( const boost::shared_ptr< ViewFactory >& factory = GetViewFactory( parent, name ) )
		{
			const FSTree* windowKey = GetViewWindowKey( parent->Parent().get(), parent->Name() );
			
			AddViewWindowKey( parent, name, windowKey );
			
			N::WindowRef windowRef = GetWindowRef( windowKey );
			
			Rect bounds = N::GetPortBounds( N::GetWindowPort( windowRef ) );
			
			FrameParameters& params = gFrameParametersMap[ parent ];
			
			const short margin = params.itsMargin;
			
			::InsetRect( &bounds, margin, margin );
			
			std::auto_ptr< Ped::View > subview( (*factory)( bounds ) );
			
			params.itsSubview = subview;
			
			InvalidateWindow( WindowKey() );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	FSTreePtr FSTree_Frame_view::Lookup( const std::string& name ) const
	{
		const FSTreePtr& delegate = GetViewDelegate( ParentKey(), Name() );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Lookup( name );
	}
	
	FSIteratorPtr FSTree_Frame_view::Iterate() const
	{
		const FSTreePtr& delegate = GetViewDelegate( ParentKey(), Name() );
		
		if ( delegate == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return delegate->Iterate();
	}
	
	
	const Functional_Traits< void >::Mapping Frame_view_Mappings[] =
	{
		{ "margin", &Factory< FSTree_Frame_margin > },
		
		{ "v", &Factory< FSTree_Frame_view >, true },
		
		{ NULL, NULL }
	};
	
}

