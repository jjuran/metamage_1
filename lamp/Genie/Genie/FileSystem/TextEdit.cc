/*	===========
 *	TextEdit.cc
 *	===========
 */

#include "Genie/FileSystem/TextEdit.hh"

// Standard C++
#include <algorithm>
#include <map>

// Nitrogen
#include "Nitrogen/MacMemory.h"
#include "Nitrogen/QuickDraw.h"

// Genie
#include "Genie/FileSystem/ScrollerBase.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
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
	
	
	void TextEdit::Install()
	{
		ASSERT( itsTE == NULL );
		
		N::CGrafPtr thePort = N::GetQDGlobalsThePort();
		
		Rect bounds = N::GetPortBounds( thePort );  // will be fixed in next Draw()
		
		itsTE = N::TENew( bounds );
		
		N::TEAutoView( true, itsTE );  // enable auto-scrolling
		
		InstallCustomTEClickLoop( itsTE );
		
		if ( N::GetWindowPort( N::FrontWindow() ) == thePort )
		{
			N::TEActivate( itsTE );
		}
	}
	
	void TextEdit::Uninstall()
	{
		itsTE.reset();
	}
	
	
	bool TextEdit::KeyDown( const EventRecord& event )
	{
		return itsKeyDown != NULL ? itsKeyDown( *this, event )
		                          : Ped::TextEdit::KeyDown( event );
	}
	
	void TextEdit::On_UserSelect()
	{
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		params.itsSelection = GetCurrentSelection();
		
		UpdateScrollOffsets();
	}
	
	void TextEdit::On_UserEdit()
	{
		UpdateText();
		
		UpdateClientHeight();
		
		On_UserSelect();
	}
	
	void TextEdit::On_EnterKey()
	{
		TextEditParameters::Get( itsKey ).itIsInterlocked = false;
	}
	
	void TextEdit::UpdateText()
	{
		Ped::TextSelection current = GetCurrentSelection();
		
		if ( current.start != current.end )
		{
			// Destructive operations don't leave a selection
			return;
		}
		
		ASSERT( itsTE != NULL );
		
		std::size_t length = itsTE[0]->teLength;
		
		TextEditParameters& params = TextEditParameters::Get( itsKey );
		
		const Ped::TextSelection& previous = params.itsSelection;
		
		unsigned start = std::min( current.start, previous.start );
		
		start = std::min< unsigned >( params.itsText.length(), start );
		
		params.itsText.resize( length );
		
		const TERec& te = **itsTE;
		
		Handle h = te.hText;
		
		std::replace_copy( *h + start,
		                   *h + length,
		                   params.itsText.begin() + start,
		                   '\r',
		                   '\n' );
		
		params.itsValidLength = length;
	}
	
	void TextEdit::UpdateClientHeight()
	{
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = GetScrollerParams( itsKey );
		
		params.itsClientHeight = Ped::GetTextEditingHeight( **itsTE );
	}
	
	void TextEdit::UpdateScrollOffsets()
	{
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = GetScrollerParams( itsKey );
		
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
	
	
	bool Update_TE_From_Model( TEHandle hTE, TextEditParameters& params )
	{
		bool text_modified = false;
		
		if ( params.itsValidLength < params.itsText.length() )
		{
			text_modified = true;
			
			N::SetHandleSize( hTE[0]->hText, params.itsText.length() );
			
			TERec& te = **hTE;
			
			te.teLength = params.itsText.length();
			
			std::replace_copy( params.itsText.begin() + params.itsValidLength,
			                   params.itsText.end(),
			                   *te.hText + params.itsValidLength,
			                   '\n',
			                   '\r' );
			
			params.itsValidLength = te.teLength;
		}
		else if ( params.itsValidLength < hTE[0]->teLength )
		{
			// Text was merely truncated
			
			text_modified = true;
			
			TERec& te = **hTE;
			
			te.teLength = params.itsValidLength;
			
			N::SetHandleSize( te.hText, params.itsValidLength );
		}
		
		return text_modified;
	}
	
}

