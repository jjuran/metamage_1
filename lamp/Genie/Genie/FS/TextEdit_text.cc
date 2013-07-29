/*	================
 *	TextEdit_text.cc
 *	================
 */

#include "Genie/FS/TextEdit_text.hh"

// Standard C++
#include <algorithm>

// POSIX
#include <sys/stat.h>

// plus
#include "plus/mac_utf8.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/TextEdit.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	static void TextEdit_text_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->owner();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		if ( length < params.its_utf8_text.length() )
		{
			params.its_utf8_text.resize( length );
			
			params.its_mac_text = plus::mac_from_utf8( params.its_utf8_text );
			
			params.itsValidLength = params.its_mac_text.length();
		}
		
		InvalidateWindowForView( view );
	}
	
	class TextEdit_text_Handle : public RegularFileHandle
	{
		public:
			TextEdit_text_Handle( const FSTreePtr& file, int flags )
			:
				RegularFileHandle( file, flags )
			{
			}
			
			IOPtr Clone();
			
			const FSTree* ViewKey();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return TextEditParameters::Get( ViewKey() ).its_utf8_text.size(); }
			
			void SetEOF( off_t length )  { TextEdit_text_SetEOF( GetFile().get(), length ); }
	};
	
	IOPtr TextEdit_text_Handle::Clone()
	{
		return new TextEdit_text_Handle( GetFile(), GetFlags() );
	}
	
	const FSTree* TextEdit_text_Handle::ViewKey()
	{
		return GetFile()->owner();
	}
	
	ssize_t TextEdit_text_Handle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const FSTree* view = ViewKey();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		const plus::string& s = params.its_utf8_text;
		
		if ( offset >= s.size() )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, s.size() - offset );
		
		memcpy( buffer, &s[ offset ], n_bytes );
		
		return n_bytes;
	}
	
	ssize_t TextEdit_text_Handle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		const FSTree* view = ViewKey();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		plus::var_string& s = params.its_utf8_text;
		
		if ( offset + n_bytes > s.size() )
		{
			s.resize( offset + n_bytes );
		}
		
		std::copy( buffer,
		           buffer + n_bytes,
		           s.begin() + offset );
		
		if ( offset < params.itsValidLength )
		{
			params.itsValidLength = 0;  // temporary hack until we can count UTF-8 chars
		}
		
		params.its_mac_text = plus::mac_from_utf8( s );
		
		InvalidateWindowForView( view );
		
		return n_bytes;
	}
	
	
	static off_t textedit_text_geteof( const FSTree* node )
	{
		return TextEditParameters::Get( node->owner() ).its_utf8_text.size();
	}
	
	static void textedit_text_seteof( const FSTree* node, off_t length )
	{
		TextEdit_text_SetEOF( node, length );
	}
	
	static IOPtr textedit_text_open( const FSTree* node, int flags, mode_t mode )
	{
		return new TextEdit_text_Handle( node, flags );
	}
	
	static const data_method_set textedit_text_data_methods =
	{
		&textedit_text_open,
		&textedit_text_geteof,
		&textedit_text_seteof
	};
	
	static const node_method_set textedit_text_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&textedit_text_data_methods
	};
	
	FSTreePtr New_FSTree_TextEdit_text( const FSTree*        parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		return new FSTree( parent, name, S_IFREG | 0600, &textedit_text_methods );
	}
	
}

