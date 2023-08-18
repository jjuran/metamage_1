/*
	gui/new/button.cc
	-----------------
*/

#include "Genie/FS/gui/new/button.hh"

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// POSIX
#include <sys/stat.h>

// mac-sys-utils
#include "mac_sys/delay.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/simple_map.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Controls.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/TrackControl.hh"
#include "Pedestal/PushButton.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"
#include "vfs/node/types/trigger.hh"

// relix-kernel
#include "relix/api/try_again.hh"

// Genie
#include "Genie/FS/Views.hh"


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
	
	typedef plus::simple_map< const vfs::node*, Button_Parameters > ButtonMap;
	
	static ButtonMap gButtonMap;
	
	
	struct Control_UserData : Pedestal::Control_UserData
	{
		const vfs::node*  key;
		
		Control_UserData() : key()
		{
		}
	};
	
	static void DebriefAfterTrack( ControlRef control )
	{
		Control_UserData* userData = N::GetControlReference( control );
		
		ASSERT( userData      != NULL );
		ASSERT( userData->key != NULL );
		
		const vfs::node* button = userData->key;
		
		if ( Button_Parameters* it = gButtonMap.find( button ) )
		{
			++it->seed;
		}
	}
	
	class PushButton : public Ped::PushButton
	{
		private:
			typedef const vfs::node* Key;
			
			Control_UserData itsUserData;
		
		public:
			PushButton( Key key )
			{
				itsUserData.trackHook = &Ped::TrackControl;
				itsUserData.afterHook = &DebriefAfterTrack;
				itsUserData.key = key;
			}
			
			void Install( const Rect& bounds );
			
			void Uninstall();
			
			void Idle( const EventRecord& event );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	void PushButton::Install( const Rect& bounds )
	{
		Ped::PushButton::Install( bounds );
		
		ControlRef control = Get();
		
		Button_Parameters& params = gButtonMap[ itsUserData.key ];
		
		SetControlTitle( control, params.title );
		
		SetControlReference( control, (long) &itsUserData );
		
		params.installed = true;
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
			
			mac::sys::delay( 8 );
			
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
			
			SetControlTitle( Get(), params.title );
		}
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new PushButton( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		gButtonMap.erase( delegate );
	}
	
	
	struct Button_Title
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name );
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary, const plus::string& name );
	};
	
	void Button_Title::get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		result = gButtonMap[ that ].title;
		
		if ( name[ 0 ] != '.' )
		{
			result = plus::utf8_from_mac( result );
		}
	}
	
	static inline
	void set_button_title( Button_Parameters& params, const char* begin, const char* end )
	{
		params.title = N::Str255( begin, end - begin );
	}
	
	void Button_Title::set( const vfs::node* that, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		Button_Parameters& params = gButtonMap[ that ];
		
		if ( name[ 0 ] == '.' )
		{
			set_button_title( params, begin, end );
		}
		else
		{
			plus::string mac_text = plus::mac_from_utf8( begin, end - begin );
			
			set_button_title( params, mac_text.begin(), mac_text.end() );
		}
		
		params.title_changed = true;
		
		InvalidateWindowForView( that );
	}
	
	static const vfs::property_params Button_Title_params =
	{
		vfs::no_fixed_size,
		(vfs::property_get_hook) &Button_Title::get,
		(vfs::property_set_hook) &Button_Title::set,
	};
	
	struct button_stream_extra
	{
		unsigned long seed;
	};
	
	static
	unsigned buttonstream_poll( vfs::filehandle* that );
	
	static
	ssize_t buttonstream_read( vfs::filehandle* that, char* buffer, size_t n );
	
	static const vfs::stream_method_set buttonstream_stream_methods =
	{
		&buttonstream_poll,
		&buttonstream_read,
	};
	
	static const vfs::filehandle_method_set buttonstream_methods =
	{
		NULL,
		NULL,
		&buttonstream_stream_methods,
	};
	
	
	static
	unsigned buttonstream_poll( vfs::filehandle* that )
	{
		button_stream_extra& extra = *(button_stream_extra*) that->extra();
		
		const vfs::node* view = get_file( *that )->owner();
		
		Button_Parameters* it = gButtonMap.find( view );
		
		const bool readable =    it == NULL
		                      || !it->installed
		                      || it->seed != extra.seed;
		
		return readable * vfs::Poll_read | vfs::Poll_write;
	}
	
	static
	ssize_t buttonstream_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		button_stream_extra& extra = *(button_stream_extra*) that->extra();
		
		const vfs::node* view = get_file( *that )->owner();
		
	retry:
		
		const Button_Parameters* it = gButtonMap.find( view );
		
		if ( it == NULL  ||  !it->installed )
		{
			p7::throw_errno( ECONNRESET );
		}
		
		const Button_Parameters& params = *it;
		
		if ( params.seed == extra.seed )
		{
			relix::try_again( is_nonblocking( *that ) );
			
			goto retry;
		}
		
		if ( n == 0 )
		{
			return 0;
		}
		
		extra.seed = params.seed;
		
		const char c = '\n';
		
		buffer[ 0 ] = c;
		
		return sizeof c;
	}
	
	
	static vfs::filehandle_ptr button_stream_open( const vfs::node* that, int flags, mode_t mode )
	{
		const vfs::node* view = that->owner();
		
		const Button_Parameters* it = gButtonMap.find( view );
		
		if ( it == NULL  ||  !it->installed )
		{
			p7::throw_errno( ECONNREFUSED );
		}
		
		typedef button_stream_extra stream_extra;
		
		vfs::filehandle* result = new vfs::filehandle( that,
		                                               flags,
		                                               &buttonstream_methods,
		                                               sizeof (stream_extra) );
		
		stream_extra& extra = *(stream_extra*) result->extra();
		
		extra.seed = it->seed;
		
		return result;
	}
	
	static const vfs::data_method_set button_stream_data_methods =
	{
		&button_stream_open
	};
	
	static const vfs::node_method_set button_stream_methods =
	{
		NULL,
		&button_stream_data_methods
	};
	
	static vfs::node_ptr button_stream_factory( const vfs::node*     parent,
	                                            const plus::string&  name,
	                                            const void*          args )
	{
		return new vfs::node( parent, name, S_IFCHR | 0400, &button_stream_methods );
	}
	
	
	static void button_click_trigger( const vfs::node* that )
	{
		const vfs::node* view = that->owner();
		
		gButtonMap[ view ].pseudoclicked = true;
		
		Ped::AdjustSleepForTimer( 1 );
	}
	
	static const vfs::trigger_extra button_click_trigger_extra =
	{
		&button_click_trigger
	};
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ ".mac-title", PROPERTY( Button_Title ) },
		{ "title",      PROPERTY( Button_Title ) },
		
		{ "click", &vfs::trigger_factory, (void*) &button_click_trigger_extra },
		
		{ "clicked", &button_stream_factory },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_button( const vfs::node*     parent,
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
