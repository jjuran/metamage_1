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

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/ScrollerBase.hh"
#include "Genie/FileSystem/TextEdit.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
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
	
	void TextEdit_Scroller::Draw( const Rect& bounds )
	{
		using Nucleus::operator!=;
		
		ScrollerBase::Draw( bounds );
		
		TEHandle hTE = itsSubview.Get();
		
		ASSERT( hTE != NULL );
		
		const FSTree* key = GetKey();
		
		ScrollerParameters& params = GetScrollerParams( key );
		
		TextEditParameters& editParams = TextEditParameters::Get( key );
		
		const bool text_modified = Update_TE_From_Model( hTE, editParams );
		
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
			
			short rows = (bounds.bottom - bounds.top) / te.lineHeight;
			short cols = (bounds.right - bounds.left) / ::CharWidth( 'M' );
			
			editParams.itsTextDimensions = N::SetPt( cols, rows );
		}
		
		if ( bounds_changed || text_modified )
		{
			N::TECalText( hTE );
			
			params.itsClientHeight = Ped::GetTextEditingHeight( **hTE );
			
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
	
	
	boost::shared_ptr< Ped::View > TextEditFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new TextEdit_Scroller( delegate ) );
	}
	
	
	void FSTree_new_textedit::DestroyDelegate( const FSTree* delegate )
	{
		RemoveScrollerParams( delegate );
		
		TextEditParameters::Erase( delegate );
	}
	
	
	static void TextEditText_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->ParentRef().get();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
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
			
			std::string& String() const  { return TextEditParameters::Get( ViewKey() ).itsText; }
			
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
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
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
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
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
			
			std::string& String() const  { return TextEditParameters::Get( ParentRef().get() ).itsText; }
			
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
		
		TextEditParameters::Get( view ).itIsInterlocked = true;
	}
	
	
	struct Selection_Property
	{
		static std::string Get( const FSTree* that, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			const Ped::TextSelection& selection = TextEditParameters::Get( view ).itsSelection;
			
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
	};
	
	
	namespace
	{
		
		bool& Wrapped( const FSTree* view )
		{
			return TextEditParameters::Get( view ).itIsWrapped;
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
	
	template < class Scribe, typename Scribe::Value& (*Access)( const FSTree* ) >
	struct TE_View_Property : public View_Property< Scribe, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			TextEditParameters::Get( view ).itHasChangedAttributes = true;
			
			View_Property::Set( that, begin, end, binary );
		}
	};
	
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
		
		{ "x", &Property_Factory< TE_View_Property< Integer_Scribe< int >, HOffset > > },
		{ "y", &Property_Factory< TE_View_Property< Integer_Scribe< int >, VOffset > > },
		
		{ NULL, NULL }
	};
	
}

