/*	===========
 *	TextEdit.cc
 *	===========
 */

#include "Genie/FS/TextEdit.hh"

// Standard C++
#include <algorithm>
#include <map>

// Iota
#include "iota/decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/MacMemory.h"
#include "Nitrogen/MacWindows.h"

// Pedestal
#include "Pedestal/Clipboard.hh"

// Genie
#include "Genie/FS/ScrollerBase.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	typedef std::map< const FSTree*, TextEditParameters > TextEditParametersMap;
	
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
	
	TextEditParameters* TextEditParameters::Find( const FSTree* key )
	{
		TextEditParametersMap::iterator it = gTextEditParametersMap.find( key );
		
		if ( it == gTextEditParametersMap.end() )
		{
			return NULL;
		}
		
		return &it->second;
	}
	
	TextEditParameters& TextEditParameters::Get( const FSTree* key )
	{
		return gTextEditParametersMap[ key ];
	}
	
	void TextEditParameters::Erase( const FSTree* key )
	{
		gTextEditParametersMap.erase( key );
	}
	
	
	std::string Selection_Property::Get( const FSTree* that, bool binary )
	{
		const FSTree* view = GetViewKey( that );
		
		const Ped::TextSelection& selection = TextEditParameters::Get( view ).itsSelection;
		
		std::string result = iota::inscribe_decimal( selection.start );
		
		if ( selection.end != selection.start )
		{
			result += '-';
			
			result += iota::inscribe_decimal( selection.end );
		}
		
		return result;
	}
	
	void Selection_Property::Set( const FSTree* that, const char* begin, const char* end, bool binary )
	{
		const FSTree* view = GetViewKey( that );
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		std::size_t length = params.itsText.length();
		
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
			start = std::atoi( begin );
			
			const char* hyphen = std::find( begin, end, '-' );
			
			// If no hyphen is present, select at the given offset.
			// If no number follows the hyphen, use the text length.
			// Otherwise, convert the number and use it.
			
			s_end = hyphen     == end ? start
			      : hyphen + 1 == end ? length
			      :                     std::atoi( hyphen + 1 );
			
			// The selection must not be inverted or exceed the text range.
			
			if ( 0 > start  ||  start > s_end  ||  s_end > length )
			{
				p7::throw_errno( EINVAL );
			}
		}
		
		params.itsSelection.start = start;
		params.itsSelection.end   = s_end;
		
		params.itHasChangedAttributes = true;
		
		InvalidateWindowForView( view );
	}
	
	
	static void Update_TE_From_Model( TEHandle hTE, const FSTree *viewKey );
	
	void TextEdit::Install( const Rect& bounds )
	{
		Ped::TextEdit::Install( bounds );
		
		ASSERT( itsTE == NULL );
		
		N::CGrafPtr thePort = N::GetQDGlobalsThePort();
		
		itsTE = N::TENew( bounds );
		
		N::TEAutoView( true, itsTE );  // enable auto-scrolling
		
		InstallCustomTEClickLoop( itsTE );
	}
	
	void TextEdit::Uninstall()
	{
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
	
	bool TextEdit::UserCommand( Ped::MenuItemCode code )
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
	
	static void DrawQuasimodeFrame( Rect frame )
	{
		N::FrameRect( frame );
	}
	
	static void DrawQuasimodeFrame()
	{
		DrawQuasimodeFrame( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	static const RGBColor gRGBBlack = {     0,     0,     0 };
	static const RGBColor gRGBWhite = { 65535, 65535, 65535 };
	
	void TextEdit::BeginQuasimode()
	{
		DrawQuasimodeFrame();
	}
	
	void TextEdit::EndQuasimode()
	{
		N::RGBForeColor( gRGBWhite );
		
		DrawQuasimodeFrame();
		
		N::RGBForeColor( gRGBBlack );
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
	
	void TextEdit::Insert_Key( char c )
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		const TERec& te = **hTE;
		
		ASSERT( te.selStart == te.selEnd );
		
		const short offset = te.selStart;
		
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		const char unix_char = (c == '\r') ? '\n' : c;
		
		params.itsText.insert( params.itsText.begin() + offset, unix_char );
		
		N::TEKey( params.itIsSecret ? '¥' : c, hTE );
		
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
		
		if ( params.itsValidLength >= end )
		{
			params.itsValidLength -= (end - start);
		}
		else if ( params.itsValidLength > start )
		{
			params.itsValidLength = start;
		}
		
		params.itsText.erase( params.itsText.begin() + start, params.itsText.begin() + end );
		
		::TEDelete( hTE );
		
		params.itsSelection.start =
		params.itsSelection.end   = start;
	}
	
	void TextEdit::Cut()
	{
		TEHandle hTE = Get();
		
		ASSERT( hTE != NULL );
		
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		Ped::TextSelection& selection = params.itsSelection;
		
		params.itsText.erase( params.itsText.begin() + selection.start,
		                      params.itsText.begin() + selection.end );
		
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
		
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		const TERec& te = **hTE;
		
		const short start = te.selStart;
		const short end   = te.selEnd;
		
		const int delta = scrapLength - (end - start);
		
		params.itsText.reserve( params.itsText.size() + delta );
		
		params.itsText.replace( params.itsText.begin() + start,
		                        params.itsText.begin() + end,
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
			memset( *scrapHandle, '¥', scrapLength );
		}
		
		::TEPaste( hTE );
		
		params.itsSelection.start =
		params.itsSelection.end   = hTE[0]->selStart;
		
		if ( secret )
		{
			// Restore the TE scrap
			memcpy( *scrapHandle, &params.itsText[ start ], scrapLength );
		}
	}
	
	
	void TextEdit_Scroller::SetBounds( const Rect& bounds )
	{
		const FSTree* key = GetKey();
		
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
		
		const FSTree* key = GetKey();
		
		TextEditParameters::Get( key ).itIsAtBottom = IsScrolledToBottom( ScrollerParameters::Get( key ) );
	}
	
	void TextEdit_Scroller::Draw( const Rect& bounds, bool erasing )
	{
		TEHandle hTE = itsSubview.Get();
		
		ASSERT( hTE != NULL );
		
		const FSTree* key = GetKey();
		
		Update_TE_From_Model( hTE, key );
		
		Subview().Draw( bounds, erasing );
	}
	
	
	static void Update_TE_From_Model( TEHandle hTE, const FSTree *viewKey )
	{
		TextEditParameters& params = TextEditParameters::Get( viewKey );
		
		if ( params.itsValidLength < params.itsText.length() )
		{
			N::SetHandleSize( hTE[0]->hText, params.itsText.length() );
			
			const bool secret = params.itIsSecret;
			
			TERec& te = **hTE;
			
			te.teLength = params.itsText.length();
			
			char* text = *te.hText;
			
			if ( secret )
			{
				std::fill( text + params.itsValidLength,
				           text + te.teLength,
				           '¥' );
			}
			else
			{
				std::replace_copy( params.itsText.begin() + params.itsValidLength,
				                   params.itsText.end(),
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
	
}

