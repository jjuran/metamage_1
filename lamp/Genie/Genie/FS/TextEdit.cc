/*	===========
 *	TextEdit.cc
 *	===========
 */

#include "Genie/FS/TextEdit.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// Standard C
#include <string.h>

// Standard C++
#include <algorithm>

// more-libc
#include "more/string.h"

// mac-sys-utils
#include "mac_sys/mem_error.hh"
#include "mac_sys/trap_available.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/serialize.hh"
#include "plus/simple_map.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif

// vfs
#include "vfs/node.hh"

// Pedestal
#include "Pedestal/Clipboard.hh"

// Genie
#include "Genie/FS/focusable_views.hh"
#include "Genie/FS/ScrollerBase.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/gui/port/ADDR.hh"


static inline
bool has_TEPinScroll()
{
	enum { _TEPinScroll = 0xA812 };
	
	return ! TARGET_CPU_68K  ||  mac::sys::trap_available( _TEPinScroll );
}

namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	typedef plus::simple_map< const vfs::node*, TextEditParameters > TextEditParametersMap;
	
	static TextEditParametersMap gTextEditParametersMap;
	
	
	TextEditParameters::TextEditParameters()
	:
		itsValidLength(),
		itHasChangedAttributes(),
		itIsAtBottom( true ),
		itIsInterlocked(),
		itIsActive(),
		itIsSecret(),
		itIsSingular(),
		itIsWrapped( true )
	{
		itsTextDimensions.v = 0;
		itsTextDimensions.h = 0;
	}
	
	TextEditParameters* TextEditParameters::Find( const vfs::node* key )
	{
		return gTextEditParametersMap.find( key );
	}
	
	TextEditParameters& TextEditParameters::Get( const vfs::node* key )
	{
		return gTextEditParametersMap[ key ];
	}
	
	void TextEditParameters::Erase( const vfs::node* key )
	{
		gTextEditParametersMap.erase( key );
	}
	
	
	void Selection_Property::get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		const Ped::TextSelection& selection = TextEditParameters::Get( that ).itsSelection;
		
		result = gear::inscribe_decimal( selection.start );
		
		if ( selection.end != selection.start )
		{
			result += '-';
			
			result += gear::inscribe_decimal( selection.end );
		}
	}
	
	void Selection_Property::set( const vfs::node* that, const char* begin, const char* end, bool binary )
	{
		TextEditParameters& params = TextEditParameters::Get( that );
		
		std::size_t length = params.its_mac_text.length();
		
		int start;
		int s_end;
		
		if ( end - begin == 1  &&  begin[0] == '-' )
		{
			// A single hyphen means to select the end of the text.
			
			start =
			s_end = length;
		}
		else
		{
			start = gear::parse_unsigned_decimal( begin );
			
			const char* hyphen = std::find( begin, end, '-' );
			
			// If no hyphen is present, select at the given offset.
			// If no number follows the hyphen, use the text length.
			// Otherwise, convert the number and use it.
			
			s_end = hyphen     == end ? start
			      : hyphen + 1 == end ? length
			      :                     gear::parse_unsigned_decimal( hyphen + 1 );
			
			// The selection must not be inverted or exceed the text range.
			
			if ( 0 > start  ||  start > s_end  ||  s_end > length )
			{
				p7::throw_errno( EINVAL );
			}
		}
		
		params.itsSelection.start = start;
		params.itsSelection.end   = s_end;
		
		params.itHasChangedAttributes = true;
		
		InvalidateWindowForView( that );
	}
	
	
	static void Update_TE_From_Model( TEHandle hTE, const vfs::node* viewKey );
	
	static void DeleteRange( TextEditParameters& params, short start, short end )
	{
		if ( params.itsValidLength >= end )
		{
			params.itsValidLength -= (end - start);
		}
		else if ( params.itsValidLength > start )
		{
			params.itsValidLength = start;
		}
		
		params.its_mac_text.erase( params.its_mac_text.begin() + start, params.its_mac_text.begin() + end );
		
		params.itsSelection.start =
		params.itsSelection.end   = start;
	}
	
	void TextEdit::TEClickLoop( TextEdit* that )
	{
		that->UpdateScrollOffsets();
	}
	
	TextEdit::TextEdit( Key               key,
	                    KeyDown_Hook      keyDown,
	                    UserCommand_Hook  cmdHook )
	:
		itsKey( key ),
		itsKeyDown( keyDown ),
		itsUserCommand( cmdHook )
	{
		its_TEClickLoop_callback = (TEClickLoop_callback) &TEClickLoop;
	}
	
	void TextEdit::Install( const Rect& bounds )
	{
		Ped::TextEdit::Install( bounds );
		
		add_focusable_view( itsKey, this );
	}
	
	void TextEdit::Uninstall()
	{
		remove_focusable_view( itsKey );
		
		Ped::TextEdit::Uninstall();
	}
	
	
	bool TextEdit::IsSecret() const
	{
		return TextEditParameters::Get( itsKey ).itIsSecret;
	}
	
	bool TextEdit::IsSingular() const
	{
		return TextEditParameters::Get( itsKey ).itIsSingular;
	}
	
	bool TextEdit::KeyDown( const EventRecord& event )
	{
		TEHandle itsTE = Get();
		
		Update_TE_From_Model( itsTE, itsKey );
		
		return itsKeyDown != NULL ? itsKeyDown( *this, event )
		                          : Ped::TextEdit::KeyDown( event );
	}
	
	bool TextEdit::UserCommand( Ped::CommandCode code )
	{
		TEHandle itsTE = Get();
		
		Update_TE_From_Model( itsTE, itsKey );
		
		if ( itsUserCommand != NULL  &&  itsUserCommand( *this, code ) )
		{
			return true;
		}
		
		return Ped::TextEdit::UserCommand( code );
	}
	
	void TextEdit::On_UserSelect()
	{
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		params.itsSelection = GetCurrentSelection();
		
		UpdateScrollOffsets();
	}
	
	void TextEdit::On_UserEdit()
	{
		UpdateClientHeight();
		
		On_UserSelect();
	}
	
	void TextEdit::UpdateClientHeight()
	{
		TEHandle itsTE = Get();
		
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = ScrollerParameters::Get( itsKey );
		
		params.itsClientHeight = Ped::GetTextEditingHeight( **itsTE );
	}
	
	void TextEdit::UpdateScrollOffsets()
	{
		TEHandle itsTE = Get();
		
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = ScrollerParameters::Get( itsKey );
		
		const TERec& te = **itsTE;
		
		params.itsHOffset = te.viewRect.left - te.destRect.left;
		params.itsVOffset = te.viewRect.top  - te.destRect.top;
		
		TextEditParameters::Get( itsKey ).itIsAtBottom = IsScrolledToBottom( params );
	}
	
	
	bool TextEdit::Wrapped() const
	{
		if ( TextEditParameters* params = TextEditParameters::Find( itsKey ) )
		{
			return params->itIsWrapped;
		}
		
		return true;
	}
	
	void TextEdit::SetActive( bool activating )
	{
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		params.itIsActive = activating;
	}
	
	void TextEdit::Focus()
	{
		Ped::TextEdit::Focus();
		
		const vfs::node* windowKey = GetViewWindowKey( itsKey );
		
		set_port_focus( windowKey, itsKey );
	}
	
	void TextEdit::Insert_Key( char c )
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		const TERec& te = **hTE;
		
		ASSERT( te.selStart == te.selEnd );
		
		const short offset = te.selStart;
		
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		if ( c == kBackspaceCharCode )
		{
			DeleteRange( params, offset - 1, offset );
			
			TEKey( c, hTE );
			
			return;
		}
		
		Ped::Preflight_TEKey( c, hTE );
		
		const char unix_char = (c == '\r') ? '\n' : c;
		
		params.its_mac_text.insert( params.its_mac_text.begin() + offset, unix_char );
		
		TEKey( params.itIsSecret ? 0xA5 : c, hTE );  // bullet
		
		params.itsSelection.start =
		params.itsSelection.end   = offset + 1;
		
		if ( params.itsValidLength >= offset )
		{
			++params.itsValidLength;
		}
	}
	
	void TextEdit::Delete()
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		const TERec& te = **hTE;
		
		const short start = te.selStart;
		const short end   = te.selEnd;
		
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		DeleteRange( params, start, end );
		
		::TEDelete( hTE );
	}
	
	void TextEdit::Cut()
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		Ped::TextSelection& selection = params.itsSelection;
		
		params.its_mac_text.erase( params.its_mac_text.begin() + selection.start,
		                           params.its_mac_text.begin() + selection.end );
		
		params.itsValidLength -= selection.end - selection.start;
		
		selection.end = selection.start;
		
		Ped::Clipboard::TECut( hTE );
	}
	
	void TextEdit::Paste()
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		const Handle scrapHandle = TEScrapHandle();
		const UInt16 scrapLength = TEGetScrapLength();
		
		Ped::Preflight_TEPaste( hTE );
		
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		const TERec& te = **hTE;
		
		const short start = te.selStart;
		const short end   = te.selEnd;
		
		const int delta = scrapLength - (end - start);
		
		params.its_mac_text.reserve( params.its_mac_text.size() + delta );
		
		params.its_mac_text.replace( params.its_mac_text.begin() + start,
		                             params.its_mac_text.begin() + end,
		                             *scrapHandle,
		                             scrapLength );
		
		if ( params.itsValidLength > end )
		{
			params.itsValidLength += delta;
		}
		else if ( params.itsValidLength >= start )
		{
			params.itsValidLength = start + scrapLength;
		}
		
		const bool secret = params.itIsSecret;
		
		if ( secret )
		{
			// Fill the TE scrap with bullets temporarily
			memset( *scrapHandle, 0xA5, scrapLength );  // bullet
		}
		
		::TEPaste( hTE );
		
		params.itsSelection.start =
		params.itsSelection.end   = hTE[0]->selStart;
		
		if ( secret )
		{
			// Restore the TE scrap
			mempcpy( *scrapHandle, &params.its_mac_text[ start ], scrapLength );
		}
	}
	
	
	void TextEdit_Scroller::SetBounds( const Rect& bounds )
	{
		const vfs::node* key = GetKey();
		
		ScrollerParameters& params = ScrollerParameters::Get( key );
		
		TextEditParameters& editParams = TextEditParameters::Get( key );
		
		TEHandle hTE = itsSubview.Get();
		
		ASSERT( hTE != NULL );
		
		TERec& te = **hTE;
		
		te.viewRect = bounds;
		te.destRect = bounds;
		
		te.destRect.left  -= params.itsHOffset;
		te.destRect.right -= params.itsHOffset;
		te.destRect.top    -= params.itsVOffset;
		te.destRect.bottom -= params.itsVOffset;
		
		const short viewWidth  = bounds.right - bounds.left;
		const short viewHeight = bounds.bottom - bounds.top;
		
		const short rows = viewHeight / te.lineHeight;
		const short cols = viewWidth  / ::CharWidth( 'M' );
		
		editParams.itsTextDimensions.v = rows;
		editParams.itsTextDimensions.h = cols;
		
		TECalText( hTE );
		
		params.itsLastViewBounds = bounds;
		
		params.itsClientWidth  = viewWidth;
		params.itsClientHeight = Ped::GetTextEditingHeight( **hTE );
	}
	
	void TextEdit_Scroller::Scroll( int dh, int dv )
	{
		TEHandle hTE = itsSubview.Get();
		
		if ( has_TEPinScroll() )
		{
			TEPinScroll( dh, dv, hTE );
		}
		else
		{
			TEScroll( dh, dv, hTE );
		}
		
		const vfs::node* key = GetKey();
		
		TextEditParameters::Get( key ).itIsAtBottom = IsScrolledToBottom( ScrollerParameters::Get( key ) );
	}
	
	void TextEdit_Scroller::Draw( const Rect& bounds, bool erasing )
	{
		TEHandle hTE = itsSubview.Get();
		
		ASSERT( hTE != NULL );
		
		const vfs::node* key = GetKey();
		
		Update_TE_From_Model( hTE, key );
		
		Subview().Draw( bounds, erasing );
	}
	
	
	static void Update_TE_From_Model( TEHandle hTE, const vfs::node *viewKey )
	{
		TextEditParameters& params = TextEditParameters::Get( viewKey );
		
		size_t mac_text_length = params.its_mac_text.length();
		
		if ( params.itsValidLength < mac_text_length )
		{
			SetHandleSize( hTE[0]->hText, mac_text_length );
			
			Mac::ThrowOSStatus( mac::sys::mem_error() );
			
			const bool secret = params.itIsSecret;
			
			TERec& te = **hTE;
			
			te.teLength = mac_text_length;
			
			char* text = *te.hText;
			
			if ( secret )
			{
				// fill with bullets
				
				memset( text + params.itsValidLength,
				        0xA5,
				        te.teLength - params.itsValidLength );
			}
			else
			{
				std::replace_copy( params.its_mac_text.begin() + params.itsValidLength,
				                   params.its_mac_text.end(),
				                   text + params.itsValidLength,
				                   '\n',
				                   '\r' );
			}
			
			params.itsValidLength = te.teLength;
		}
		else if ( params.itsValidLength < hTE[0]->teLength )
		{
			// Text was merely truncated
			
			TERec& te = **hTE;
			
			te.teLength = params.itsValidLength;
			
			SetHandleSize( te.hText, params.itsValidLength );
		}
		else
		{
			// Text wasn't modified at all
			return;
		}
		
		TECalText( hTE );
		
		Rect bounds = hTE[0]->viewRect;
		
		ScrollerParameters& scroller_params = ScrollerParameters::Get( viewKey );
		
		scroller_params.itsClientHeight = Ped::GetTextEditingHeight( **hTE );
		
		const short viewHeight = bounds.bottom - bounds.top;
		
		const short max_voffset = std::max( scroller_params.itsClientHeight - viewHeight, 0 );
		
		if ( scroller_params.itsVOffset == max_voffset )
		{
			// do nothing
		}
		else if ( scroller_params.itsVOffset > max_voffset  ||  params.itIsAtBottom )
		{
			scroller_params.itsVOffset = max_voffset;
			
			params.itHasChangedAttributes = true;
		}
		
		if ( params.itHasChangedAttributes )
		{
			TERec& te = **hTE;
			
			// Propagate changes made to 'x' and 'y' files
			te.destRect = te.viewRect;
			
			te.destRect.left  -= scroller_params.itsHOffset;
			te.destRect.right -= scroller_params.itsHOffset;
			te.destRect.top    -= scroller_params.itsVOffset;
			te.destRect.bottom -= scroller_params.itsVOffset;
			
			te.selStart = params.itsSelection.start;
			te.selEnd   = params.itsSelection.end;
			
			params.itHasChangedAttributes = false;
		}
	}
	
	using plus::serialize_bool;
	
	static
	void flag_get( plus::var_string& result, const vfs::node* view, bool binary, const plus::string& name )
	{
		const TextEditParameters& params = TextEditParameters::Get( view );
		
		const char c = name[ 1 ];
		
		const bool value = c == 'c' ? params.itIsActive
		                 : c == 'e' ? params.itIsSecret
		                 : c == 'i' ? params.itIsSingular
		                 :            0;
		
		serialize_bool::deconstruct::apply( result, value, binary );
	}
	
	static
	void flag_set( const vfs::node* view, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		TextEditParameters& params = TextEditParameters::Get( view );
		
		bool value = serialize_bool::reconstruct::apply( begin, end, binary );
		
		if ( name[ 1 ] == 'e' )
		{
			params.itsValidLength = 0;
			params.itIsSecret = value;
		}
		else
		{
			params.itIsSingular = value;
		}
		
		InvalidateWindowForView( view );
	}
	
	const vfs::property_params textedit_flag_params =
	{
		serialize_bool::fixed_size,
		(vfs::property_get_hook) &flag_get,
		(vfs::property_set_hook) &flag_set,
	};
	
	static
	void scroll_set( const vfs::node* that, const char* begin, const char* end, bool binary )
	{
		TextEditParameters::Get( that ).itHasChangedAttributes = true;
		
		scroller_setting_params.set( that, begin, end, binary );
	}
	
	const vfs::property_params textedit_scroll_params =
	{
		scroller_setting_params.size,
		scroller_setting_params.get,
		&scroll_set,
	};
	
	const vfs::property_params textedit_selection_params =
	{
		vfs::no_fixed_size,
		&Selection_Property::get,
		&Selection_Property::set,
	};
	
}
