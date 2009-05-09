/*	================
 *	TextEdit_text.cc
 *	================
 */

#include "Genie/FS/TextEdit_text.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/TextEdit.hh"
#include "Genie/FS/Views.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	class FSTree_TextEdit_text : public FSTree
	{
		public:
			FSTree_TextEdit_text( const FSTreePtr&    parent,
			                      const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const;
			
			void SetEOF( off_t length ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	static void TextEdit_text_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->ParentRef().get();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		params.itsValidLength = std::min< size_t >( params.itsText.length(), length );
		
		params.itsText.resize( length );
		
		InvalidateWindowForView( view );
	}
	
	class TextEdit_text_Handle : public VirtualFileHandle< RegularFileHandle >
	{
		public:
			TextEdit_text_Handle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			const FSTree* ViewKey() const;
			
			std::string& String() const  { return TextEditParameters::Get( ViewKey() ).itsText; }
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
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
	
	ssize_t TextEdit_text_Handle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const FSTree* view = ViewKey();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		while ( params.itIsInterlocked )
		{
			TryAgainLater();
		}
		
		std::string& s = params.itsText;
		
		if ( offset >= s.size() )
		{
			return 0;
		}
		
		n_bytes = std::min( n_bytes, s.size() - offset );
		
		memcpy( buffer, s.begin() + offset, n_bytes );
		
		return n_bytes;
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
		IOHandle* result = new TextEdit_text_Handle( Self(), flags );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	FSTreePtr New_FSTree_TextEdit_text( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_TextEdit_text( parent, name ) );
	}
	
}

