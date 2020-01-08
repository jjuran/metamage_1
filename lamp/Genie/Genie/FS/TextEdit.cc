/*	===========
 *	TextEdit.cc
 *	===========
 */

#include "Genie/FS/TextEdit.hh"

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAWTEXT_H
#include "missing/QuickdrawText.h"
#endif
#endif

// Standard C++
#include <algorithm>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/simple_map.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"
#include "Nitrogen/MacWindows.hh"
#include "Nitrogen/Quickdraw.hh"
#include "Nitrogen/TextEdit.hh"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.hh"

// Pedestal
#include "Pedestal/Clipboard.hh"

// Genie
#include "Genie/FS/focusable_views.hh"
#include "Genie/FS/ScrollerBase.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/gui/port/ADDR.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	typedef plus::simple_map< const vfs::node*, TextEditParameters > TextEditParametersMap;
	
	static TextEditParametersMap gTextEditParametersMap;
	
	
	TextEditParameters::TextEditParameters()
	:
		itsTextDimensions( N::SetPt( 0, 0 ) ),
		itsValidLength(),
		itHasChangedAttributes(),
		itIsAtBottom(),
		itIsInterlocked(),
		itIsActive(),
		itIsSecret(),
		itIsSingular(),
		itIsWrapped( true )
	{
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
		
		params.its_utf8_text = plus::utf8_from_mac( params.its_mac_text );
		
		params.itsSelection.start =
		params.itsSelection.end   = start;
	}
	
	void TextEdit::Install( const Rect& bounds )
	{
		Ped::TextEdit::Install( bounds );
		
		ASSERT( itsTE == NULL );
		
		itsTE = N::TENew( bounds );
		
		N::TEAutoView( true, itsTE );  // enable auto-scrolling
		
		InstallCustomTEClickLoop( itsTE );
		
		add_focusable_view( itsKey, this );
	}
	
	void TextEdit::Uninstall()
	{
		remove_focusable_view( itsKey );
		
		itsTE.reset();
		
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
		Update_TE_From_Model( itsTE, itsKey );
		
		return itsKeyDown != NULL ? itsKeyDown( *this, event )
		                          : Ped::TextEdit::KeyDown( event );
	}
	
	bool TextEdit::UserCommand( Ped::CommandCode code )
	{
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
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = ScrollerParameters::Get( itsKey );
		
		params.itsClientHeight = Ped::GetTextEditingHeight( **itsTE );
	}
	
	void TextEdit::UpdateScrollOffsets()
	{
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = ScrollerParameters::Get( itsKey );
		
		const TERec& te = **itsTE;
		
		params.itsHOffset = te.viewRect.left - te.destRect.left;
		params.itsVOffset = te.viewRect.top  - te.destRect.top;
		
		TextEditParameters::Get( itsKey ).itIsAtBottom = IsScrolledToBottom( params );
	}
	
	void TextEdit::BeginQuasimode()
	{
		N::FrameRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	void TextEdit::EndQuasimode()
	{
		N::InvalRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	Ped::TextSelection TextEdit::GetPriorSelection() const
	{
		return itsSelectionPriorToSearch;
	}
	
	void TextEdit::SetPriorSelection( const Ped::TextSelection& selection )
	{
		itsSelectionPriorToSearch = selection;
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
		
		// FIXME:  Optimize UTF-8 conversion
		params.its_utf8_text = plus::utf8_from_mac( params.its_mac_text );
		
		N::TEKey( params.itIsSecret ? 0xA5 : c, hTE );  // bullet
		
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
		
		params.its_utf8_text = plus::utf8_from_mac( params.its_mac_text );
		
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
		
		Ped::Preflight_TEPaste( hTE, scrapLength );
		
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
		
		params.its_utf8_text = plus::utf8_from_mac( params.its_mac_text );
		
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
			memcpy( *scrapHandle, &params.its_mac_text[ start ], scrapLength );
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
		
		te.destRect = N::OffsetRect( te.viewRect,
		                             -params.itsHOffset,
		                             -params.itsVOffset );
		
		const short viewWidth  = bounds.right - bounds.left;
		const short viewHeight = bounds.bottom - bounds.top;
		
		const short rows = viewHeight / te.lineHeight;
		const short cols = viewWidth  / ::CharWidth( 'M' );
		
		editParams.itsTextDimensions = N::SetPt( cols, rows );
		
		N::TECalText( hTE );
		
		params.itsLastViewBounds = bounds;
		
		params.itsClientWidth  = viewWidth;
		params.itsClientHeight = Ped::GetTextEditingHeight( **hTE );
	}
	
	void TextEdit_Scroller::Scroll( int dh, int dv )
	{
		N::TEPinScroll( dh, dv, itsSubview.Get() );
		
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
		
		if ( params.itsValidLength < params.its_mac_text.length() )
		{
			N::SetHandleSize( hTE[0]->hText, params.its_mac_text.length() );
			
			const bool secret = params.itIsSecret;
			
			TERec& te = **hTE;
			
			te.teLength = params.its_mac_text.length();
			
			char* text = *te.hText;
			
			if ( secret )
			{
				// fill with bullets
				
				std::fill( text + params.itsValidLength,
				           text + te.teLength,
				           0xA5 );
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
			
			N::SetHandleSize( te.hText, params.itsValidLength );
		}
		else
		{
			// Text wasn't modified at all
			return;
		}
		
		N::TECalText( hTE );
		
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
			te.destRect = N::OffsetRect( te.viewRect,
			                             -scroller_params.itsHOffset,
			                             -scroller_params.itsVOffset );
			
			te.selStart = params.itsSelection.start;
			te.selEnd   = params.itsSelection.end;
			
			params.itHasChangedAttributes = false;
		}
	}
	
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
