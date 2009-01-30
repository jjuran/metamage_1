/*	======================
 *	FSTree_new_textedit.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_new_textedit.hh"

// Nucleus
#include "Nucleus/Saved.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/Clipboard.hh"
#include "Pedestal/Scroller_beta.hh"
#include "Pedestal/TextEdit.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/ScrollerBase.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct TextEditParameters
	{
		std::string         itsText;
		Ped::TextSelection  itsSelection;
		std::size_t         itsValidLength;
		bool                itHasChangedAttributes;
		bool                itIsInterlocked;
		bool                itIsWrapped;
		
		TextEditParameters()
		:
			itsValidLength(),
			itHasChangedAttributes(),
			itIsInterlocked(),
			itIsWrapped( true )
		{
		}
	};
	
	typedef std::map< const FSTree*, TextEditParameters > TextEditParametersMap;
	
	static TextEditParametersMap gTextEditParametersMap;
	
	
	class TextEdit : public Ped::TextEdit
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
			
			NN::Owned< N::TEHandle >  itsTE;
			
			Ped::TextSelection  itsSelectionPriorToSearch;
			
			void On_UserSelect();
			void On_UserEdit();
			
			void On_EnterKey();
			
			void UpdateText();
			
			void UpdateClientHeight();
			void UpdateScrollOffsets();
			
			void ClickInLoop()  { UpdateScrollOffsets(); }
		
		public:
			TextEdit( Key key ) : itsKey( key )
			{
				itsSelectionPriorToSearch.start = -1;
			}
			
			void Install();
			void Uninstall();
			
			TEHandle Get() const  { return itsTE; }
			
			void BeginQuasimode();
			void EndQuasimode();
			
			Ped::TextSelection GetPriorSelection() const;
			
			void SetPriorSelection( const Ped::TextSelection& selection );
			
			bool Wrapped() const;
	};
	
	class TextEdit_Scroller : public ScrollerBase
	{
		private:
			TextEdit  itsSubview;
		
		public:
			typedef const FSTree* Key;
			
			TextEdit_Scroller( Key key ) : ScrollerBase( key ), itsSubview( key )
			{
			}
			
			Ped::View& Subview()  { return itsSubview; }
			
			void Scroll( int dh, int dv );
			
			void Draw( const Rect& bounds );
	};
	
	void TextEdit_Scroller::Scroll( int dh, int dv )
	{
		N::TEPinScroll( dh, dv, itsSubview.Get() );
	}
	
	static short CountLinesForEditing( const TERec& te )
	{
		short teLength = te.teLength;
		
		// An empty buffer counts as one line
		if ( teLength == 0 )
		{
			return 1;
		}
		
		short    nLines = te.nLines;
		::Handle hText  = te.hText;
		
		// Find the last character in the buffer
		char c = ( *hText )[ teLength - 1 ];
		
		// If the last char is a carriage return, add an extra line
		if ( c == '\r' )
		{
			++nLines;
		}
		
		return nLines;
	}
	
	static short GetTextEditingHeight( TEHandle hTE )
	{
		const TERec& te = **hTE;
		
		return CountLinesForEditing( te ) * te.lineHeight;
	}
	
	void TextEdit_Scroller::Draw( const Rect& bounds )
	{
		using Nucleus::operator!=;
		
		ScrollerBase::Draw( bounds );
		
		TEHandle hTE = itsSubview.Get();
		
		ASSERT( hTE != NULL );
		
		bool text_modified = false;
		
		const FSTree* key = GetKey();
		
		ScrollerParameters& params = GetScrollerParams( key );
		
		TextEditParameters& editParams = gTextEditParametersMap[ key ];
		
		if ( editParams.itsValidLength < editParams.itsText.length() )
		{
			text_modified = true;
			
			N::SetHandleSize( hTE[0]->hText, editParams.itsText.length() );
			
			TERec& te = **hTE;
			
			te.teLength = editParams.itsText.length();
			
			std::replace_copy( editParams.itsText.begin() + editParams.itsValidLength,
			                   editParams.itsText.end(),
			                   *te.hText + editParams.itsValidLength,
			                   '\n',
			                   '\r' );
			
			editParams.itsValidLength = te.teLength;
		}
		else if ( editParams.itsValidLength < hTE[0]->teLength )
		{
			// Text was merely truncated
			
			text_modified = true;
			
			TERec& te = **hTE;
			
			te.teLength = editParams.itsValidLength;
			
			N::SetHandleSize( te.hText, editParams.itsValidLength );
		}
		
		const short viewWidth  = bounds.right - bounds.left;
		const short viewHeight = bounds.bottom - bounds.top;
		
		const bool bounds_changed = bounds != hTE[0]->viewRect;
		
		if ( bounds_changed )
		{
			//params.itsLastViewBounds = bounds;
			
			params.itsClientWidth = viewWidth;
			
			TERec& te = **hTE;
			
			te.viewRect = bounds;
			
			te.destRect = N::OffsetRect( te.viewRect,
			                             -params.itsHOffset,
			                             -params.itsVOffset );
		}
		
		if ( bounds_changed || text_modified )
		{
			N::TECalText( hTE );
			
			params.itsClientHeight = GetTextEditingHeight( hTE );
			
			if ( text_modified )
			{
				const short max_voffset = std::max( params.itsClientHeight - viewHeight, 0 );
				
				if ( params.itsVOffset > max_voffset )
				{
					params.itsVOffset = max_voffset;
					
					editParams.itHasChangedAttributes = true;
				}
			}
		}
		
		if ( editParams.itHasChangedAttributes )
		{
			TERec& te = **hTE;
			
			// Propagate changes made to 'x' and 'y' files
			te.destRect = N::OffsetRect( te.viewRect,
			                             -params.itsHOffset,
			                             -params.itsVOffset );
			
			te.selStart = editParams.itsSelection.start;
			te.selEnd   = editParams.itsSelection.end;
			
			editParams.itHasChangedAttributes = false;
		}
		
		Subview().Draw( bounds );
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
	
	
	void TextEdit::On_UserSelect()
	{
		TextEditParameters& params = gTextEditParametersMap[ itsKey ];
		
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
		gTextEditParametersMap[ itsKey ].itIsInterlocked = false;
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
		
		TextEditParameters& params = gTextEditParametersMap[ itsKey ];
		
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
		
		//const TERec& te = **itsTE;
		
		params.itsClientHeight = GetTextEditingHeight( itsTE );
	}
	
	void TextEdit::UpdateScrollOffsets()
	{
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = GetScrollerParams( itsKey );
		
		const TERec& te = **itsTE;
		
		params.itsHOffset = te.viewRect.left - te.destRect.left;
		params.itsVOffset = te.viewRect.top  - te.destRect.top;
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
		TextEditParametersMap::const_iterator it = gTextEditParametersMap.find( itsKey );
		
		if ( it != gTextEditParametersMap.end() )
		{
			return it->second.itIsWrapped;
		}
		
		return true;
	}
	
	boost::shared_ptr< Ped::View > TextEditFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new TextEdit_Scroller( delegate ) );
	}
	
	
	void FSTree_new_textedit::DestroyDelegate( const FSTree* delegate )
	{
		RemoveScrollerParams( delegate );
		
		gTextEditParametersMap.erase( delegate );
	}
	
	
	static void TextEditText_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->ParentRef().get();
		
		TextEditParameters& params = gTextEditParametersMap[ view ];
		
		params.itsValidLength = std::min< size_t >( params.itsText.length(), length );
		
		params.itsText.resize( length );
		
		InvalidateWindowForView( view );
	}
	
	class TextEditTextFileHandle : public VirtualFileHandle
	{
		public:
			TextEditTextFileHandle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			const FSTree* ViewKey() const;
			
			std::string& String() const  { return gTextEditParametersMap[ ViewKey() ].itsText; }
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t length )  { TextEditText_SetEOF( GetFile().get(), length ); }
	};
	
	boost::shared_ptr< IOHandle > TextEditTextFileHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new TextEditTextFileHandle( GetFile(), GetFlags() ) );
	}
	
	const FSTree* TextEditTextFileHandle::ViewKey() const
	{
		return GetFile()->ParentRef().get();
	}
	
	ssize_t TextEditTextFileHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		const FSTree* view = ViewKey();
		
		TextEditParameters& params = gTextEditParametersMap[ view ];
		
		while ( params.itIsInterlocked )
		{
			TryAgainLater();
		}
		
		std::string& s = params.itsText;
		
		ASSERT( GetFileMark() <= s.size() );
		
		byteCount = std::min( byteCount, s.size() - GetFileMark() );
		
		std::copy( s.begin() + GetFileMark(),
		           s.begin() + GetFileMark() + byteCount,
		           buffer );
		
		return Advance( byteCount );
	}
	
	ssize_t TextEditTextFileHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		std::string& s = String();
		
		if ( GetFileMark() + byteCount > s.size() )
		{
			s.resize( GetFileMark() + byteCount );
		}
		
		std::copy( buffer,
		           buffer + byteCount,
		           s.begin() + GetFileMark() );
		
		const FSTree* view = ViewKey();
		
		TextEditParameters& params = gTextEditParametersMap[ view ];
		
		params.itsValidLength = std::min< size_t >( params.itsValidLength, GetFileMark() );
		
		InvalidateWindowForView( view );
		
		return Advance( byteCount );
	}
	
	class FSTree_TextEdit_text : public FSTree
	{
		public:
			FSTree_TextEdit_text( const FSTreePtr&    parent,
			                      const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			std::string& String() const  { return gTextEditParametersMap[ ParentRef().get() ].itsText; }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t length ) const  { TextEditText_SetEOF( this, length ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_TextEdit_text::Open( OpenFlags flags ) const
	{
		IOHandle* result = new TextEditTextFileHandle( shared_from_this(), flags );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	class FSTree_TextEdit_interlock : public FSTree
	{
		public:
			FSTree_TextEdit_interlock( const FSTreePtr&    parent,
			                           const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			void SetTimes() const;
	};
	
	void FSTree_TextEdit_interlock::SetTimes() const
	{
		const FSTree* view = ParentRef().get();
		
		gTextEditParametersMap[ view ].itIsInterlocked = true;
	}
	
	
	struct Selection_Property
	{
		static std::string Get( const FSTree* that, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			const Ped::TextSelection& selection = gTextEditParametersMap[ view ].itsSelection;
			
			std::string result = NN::Convert< std::string >( selection.start );
			
			if ( selection.end != selection.start )
			{
				result += '-';
				
				result += NN::Convert< std::string >( selection.end );
			}
			
			return result;
		}
		
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			TextEditParameters& params = gTextEditParametersMap[ view ];
			
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
	};
	
	
	namespace
	{
		
		bool& Wrapped( const FSTree* view )
		{
			return gTextEditParametersMap[ view ].itIsWrapped;
		}
		
		int& Width( const FSTree* view )
		{
			return GetScrollerParams( view ).itsClientWidth;
		}
		
		int& Height( const FSTree* view )
		{
			return GetScrollerParams( view ).itsClientHeight;
		}
		
		int& HOffset( const FSTree* view )
		{
			return GetScrollerParams( view ).itsHOffset;
		}
		
		int& VOffset( const FSTree* view )
		{
			return GetScrollerParams( view ).itsVOffset;
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
	
	const FSTree_Premapped::Mapping TextEdit_view_Mappings[] =
	{
		{ "text", &Basic_Factory< FSTree_TextEdit_text > },
		
		{ "interlock", &Basic_Factory< FSTree_TextEdit_interlock > },
		
		{ "selection", &Property_Factory< Selection_Property > },
		
		//{ "wrapped", &Property_Factory< View_Property< Boolean_Scribe, Wrapped > > },
		
		// unlocked-text
		
		{ "width",  &Property_Factory< View_Property< Integer_Scribe< int >, Width  > > },
		{ "height", &Property_Factory< View_Property< Integer_Scribe< int >, Height > > },
		
		{ "x", &Property_Factory< View_Property< Integer_Scribe< int >, HOffset > > },
		{ "y", &Property_Factory< View_Property< Integer_Scribe< int >, VOffset > > },
		
		{ NULL, NULL }
	};
	
}

