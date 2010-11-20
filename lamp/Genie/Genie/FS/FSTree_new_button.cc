/*	====================
 *	FSTree_new_button.cc
 *	====================
 */

#include "Genie/FS/FSTree_new_button.hh"

// plus
#include "plus/make_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Controls.hh"
#include "Nitrogen/OSUtils.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/TrackControl.hh"
#include "Pedestal/PushButton.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Trigger.hh"
#include "Genie/FS/utf8_text_property.hh"
#include "Genie/FS/Views.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/IO/VirtualFile.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct Button_Parameters
	{
		std::size_t  seed;
		N::Str255    title;
		bool         title_changed;
		bool         installed;
		bool         pseudoclicked;
		
		Button_Parameters()
		:
			seed(),
			title( "\p" "OK" ),
			title_changed(),
			installed(),
			pseudoclicked()
		{
		}
	};
	
	typedef simple_map< const FSTree*, Button_Parameters > ButtonMap;
	
	static ButtonMap gButtonMap;
	
	
	struct Control_UserData : Pedestal::Control_UserData
	{
		const FSTree*  key;
		
		Control_UserData() : key()
		{
		}
	};
	
	static void DebriefAfterTrack( ControlRef control )
	{
		Control_UserData* userData = N::GetControlReference( control );
		
		ASSERT( userData      != NULL );
		ASSERT( userData->key != NULL );
		
		const FSTree* button = userData->key;
		
		if ( Button_Parameters* it = gButtonMap.find( button ) )
		{
			++it->seed;
		}
	}
	
	class PushButton : public Ped::PushButton
	{
		private:
			typedef const FSTree* Key;
			
			Control_UserData itsUserData;
		
		public:
			PushButton( Key key )
			{
				itsUserData.trackHook = &Ped::TrackControl;
				itsUserData.afterHook = &DebriefAfterTrack;
				itsUserData.key = key;
			}
			
			N::Str255 Title() const  { return gButtonMap[ itsUserData.key ].title; }
			
			N::RefCon RefCon() const  { return &itsUserData; }
			
			void Install( const Rect& bounds );
			
			void Uninstall();
			
			void Idle( const EventRecord& event );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	void PushButton::Install( const Rect& bounds )
	{
		Ped::PushButton::Install( bounds );
		
		gButtonMap[ itsUserData.key ].installed = true;
	}
	
	void PushButton::Uninstall()
	{
		Ped::PushButton::Uninstall();
		
		gButtonMap[ itsUserData.key ].installed = false;
	}
	
	static inline void QDFlushPortBuffer()
	{
		if ( TARGET_API_MAC_CARBON )
		{
			::QDFlushPortBuffer( ::GetQDGlobalsThePort(), NULL );
		}
	}
	
	void PushButton::Idle( const EventRecord& event )
	{
		Ped::PushButton::Idle( event );
		
		Button_Parameters& params = gButtonMap[ itsUserData.key ];
		
		if ( params.pseudoclicked )
		{
			params.pseudoclicked = false;
			
			N::HiliteControl( Get(), Mac::kControlButtonPart );
			
			QDFlushPortBuffer();
			
			N::Delay( 8 );
			
			N::HiliteControl( Get(), N::kControlNoPart );
			
			QDFlushPortBuffer();
			
			++params.seed;
		}
	}
	
	void PushButton::Draw( const Rect& bounds, bool erasing )
	{
		Ped::PushButton::Draw( bounds, erasing );
		
		Button_Parameters& params = gButtonMap[ itsUserData.key ];
		
		if ( params.title_changed )
		{
			params.title_changed = false;
			
			N::SetControlTitle( Get(), Title() );
		}
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return seize_ptr( new PushButton( delegate ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		gButtonMap.erase( delegate );
	}
	
	
	struct Button_Title : readwrite_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			result = plus::make_string( gButtonMap[ that ].title );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			Button_Parameters& params = gButtonMap[ that ];
			
			params.title = N::Str255( begin, end - begin );
			
			params.title_changed = true;
			
			InvalidateWindowForView( that );
		}
	};
	
	
	class Button_socket_Handle : public VirtualFileHandle< StreamHandle >
	{
		private:
			std::size_t itsSeed;
		
		public:
			Button_socket_Handle( const FSTreePtr& file, OpenFlags flags );
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
	};
	
	Button_socket_Handle::Button_socket_Handle( const FSTreePtr& file, OpenFlags flags )
	:
		VirtualFileHandle< StreamHandle >( file, flags ),
		itsSeed( gButtonMap[ file->ParentRef().get() ].seed )
	{
	}
	
	unsigned int Button_socket_Handle::SysPoll()
	{
		const FSTree* view = GetFile()->ParentRef().get();
		
		Button_Parameters* it = gButtonMap.find( view );
		
		const bool readable =    it == NULL
		                      || !it->installed
		                      || it->seed != itsSeed;
		
		return readable * kPollRead | kPollWrite;
	}
	
	ssize_t Button_socket_Handle::SysRead( char* buffer, std::size_t byteCount )
	{
		const FSTree* view = GetFile()->ParentRef().get();
		
	retry:
		
		const Button_Parameters* it = gButtonMap.find( view );
		
		if ( it == NULL  ||  !it->installed )
		{
			p7::throw_errno( ECONNRESET );
		}
		
		const Button_Parameters& params = *it;
		
		if ( params.seed == itsSeed )
		{
			TryAgainLater();
			
			goto retry;
		}
		
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		itsSeed = params.seed;
		
		const char c = '\n';
		
		buffer[ 0 ] = c;
		
		return sizeof c;
	}
	
	
	class FSTree_Button_socket : public FSTree
	{
		public:
			FSTree_Button_socket( const FSTreePtr&     parent,
	                              const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_Button_socket::Open( OpenFlags flags ) const
	{
		const FSTree* view = ParentRef().get();
		
		const Button_Parameters* it = gButtonMap.find( view );
		
		if ( it == NULL  ||  !it->installed )
		{
			p7::throw_errno( ECONNREFUSED );
		}
		
		return seize_ptr( new Button_socket_Handle( Self(), flags ) );
	}
	
	
	class Button_click_Trigger : public Trigger_Base
	{
		public:
			Button_click_Trigger( const FSTreePtr&     parent,
			                      const plus::string&  name )
			:
				Trigger_Base( parent, name )
			{
			}
			
			void Invoke() const
			{
				const FSTree* view = ParentRef().get();
				
				gButtonMap[ view ].pseudoclicked = true;
				
				Ped::AdjustSleepForTimer( 1 );
			}
	};
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ ".mac-title", PROPERTY( Button_Title ) },
		
		{ "title", PROPERTY( utf8_text_property< Button_Title > ) },
		
		{ "click", &Basic_Factory< Button_click_Trigger > },
		
		{ "socket", &Basic_Factory< FSTree_Button_socket > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_button( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const void*          args )
	{
		return seize_ptr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

