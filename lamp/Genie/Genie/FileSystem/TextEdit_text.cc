/*	================
 *	TextEdit_text.cc
 *	================
 */

#include "Genie/FileSystem/TextEdit_text.hh"

// Genie
#include "Genie/FileSystem/TextEdit.hh"
#include "Genie/FileSystem/Views.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	static void TextEdit_text_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->ParentRef().get();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		params.itsValidLength = std::min< size_t >( params.itsText.length(), length );
		
		params.itsText.resize( length );
		
		InvalidateWindowForView( view );
	}
	
	class TextEdit_text_Handle : public VirtualFileHandle
	{
		public:
			TextEdit_text_Handle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			const FSTree* ViewKey() const;
			
			std::string& String() const  { return TextEditParameters::Get( ViewKey() ).itsText; }
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t length )  { TextEdit_text_SetEOF( GetFile().get(), length ); }
	};
	
	boost::shared_ptr< IOHandle > TextEdit_text_Handle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new TextEdit_text_Handle( GetFile(), GetFlags() ) );
	}
	
	const FSTree* TextEdit_text_Handle::ViewKey() const
	{
		return GetFile()->ParentRef().get();
	}
	
	ssize_t TextEdit_text_Handle::SysRead( char* buffer, std::size_t byteCount )
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
	
	ssize_t TextEdit_text_Handle::SysWrite( const char* buffer, std::size_t byteCount )
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
	
	
	off_t FSTree_TextEdit_text::GetEOF() const
	{
		return TextEditParameters::Get( ParentRef().get() ).itsText.size();
	}
	
	void FSTree_TextEdit_text::SetEOF( off_t length ) const
	{
		TextEdit_text_SetEOF( this, length );
	}
	
	boost::shared_ptr< IOHandle > FSTree_TextEdit_text::Open( OpenFlags flags ) const
	{
		IOHandle* result = new TextEdit_text_Handle( shared_from_this(), flags );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
}

