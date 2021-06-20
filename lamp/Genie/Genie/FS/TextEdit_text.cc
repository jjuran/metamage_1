/*	================
 *	TextEdit_text.cc
 *	================
 */

#include "Genie/FS/TextEdit_text.hh"

// POSIX
#include <sys/stat.h>

// more-libc
#include "more/string.h"

// plus
#include "plus/mac_utf8.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/TextEdit.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
	}
	
	
	static
	const vfs::node* view_key( vfs::filehandle* that );
	
	static void TextEdit_text_SetEOF( const vfs::node* text, off_t length )
	{
		const vfs::node* view = text->owner();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		if ( length < params.its_utf8_text.length() )
		{
			params.its_utf8_text.resize( length );
			
			params.its_mac_text = plus::mac_from_utf8( params.its_utf8_text );
			
			params.itsValidLength = params.its_mac_text.length();
		}
		
		InvalidateWindowForView( view );
	}
	
	
	static
	ssize_t TextEdit_text_pread( vfs::filehandle*  that,
	                             char*             buffer,
	                             size_t            n_bytes,
	                             off_t             offset );
	
	static off_t TextEdit_text_geteof( vfs::filehandle* file )
	{
		return TextEditParameters::Get( view_key( file ) ).its_utf8_text.size();
	}
	
	static
	ssize_t TextEdit_text_pwrite( vfs::filehandle*  that,
	                              const char*       buffer,
	                              size_t            n_bytes,
	                              off_t             offset );
	
	static void TextEdit_text_seteof( vfs::filehandle* file, off_t length )
	{
		TextEdit_text_SetEOF( get_file( *file ).get(), length );
	}
	
	static const vfs::bstore_method_set TextEdit_text_bstore_methods =
	{
		&TextEdit_text_pread,
		&TextEdit_text_geteof,
		&TextEdit_text_pwrite,
		&TextEdit_text_seteof,
	};
	
	static const vfs::filehandle_method_set TextEdit_text_methods =
	{
		&TextEdit_text_bstore_methods,
	};
	
	
	static
	const vfs::node* view_key( vfs::filehandle* that )
	{
		return get_file( *that )->owner();
	}
	
	static
	ssize_t TextEdit_text_pread( vfs::filehandle*  that,
	                             char*             buffer,
	                             size_t            n_bytes,
	                             off_t             offset )
	{
		const vfs::node* view = view_key( that );
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		const plus::string& s = params.its_utf8_text;
		
		if ( offset >= s.size() )
		{
			return 0;
		}
		
		n_bytes = min< size_t >( n_bytes, s.size() - offset );
		
		mempcpy( buffer, &s[ offset ], n_bytes );
		
		return n_bytes;
	}
	
	static
	ssize_t TextEdit_text_pwrite( vfs::filehandle*  that,
	                              const char*       buffer,
	                              size_t            n_bytes,
	                              off_t             offset )
	{
		const vfs::node* view = view_key( that );
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		plus::var_string& s = params.its_utf8_text;
		
		if ( offset + n_bytes > s.size() )
		{
			s.resize( offset + n_bytes );
		}
		
		mempcpy( s.begin() + offset, buffer, n_bytes );
		
		if ( offset < params.itsValidLength )
		{
			params.itsValidLength = 0;  // temporary hack until we can count UTF-8 chars
		}
		
		params.its_mac_text = plus::mac_from_utf8( s );
		
		InvalidateWindowForView( view );
		
		return n_bytes;
	}
	
	
	static off_t textedit_text_geteof( const vfs::node* that )
	{
		return TextEditParameters::Get( that->owner() ).its_utf8_text.size();
	}
	
	static void textedit_text_seteof( const vfs::node* that, off_t length )
	{
		TextEdit_text_SetEOF( that, length );
	}
	
	static vfs::filehandle_ptr textedit_text_open( const vfs::node* that, int flags, mode_t mode )
	{
		return new vfs::filehandle( that, flags, &TextEdit_text_methods );
	}
	
	static const vfs::data_method_set textedit_text_data_methods =
	{
		&textedit_text_open,
		&textedit_text_geteof,
		&textedit_text_seteof
	};
	
	static const vfs::node_method_set textedit_text_methods =
	{
		NULL,
		&textedit_text_data_methods
	};
	
	vfs::node_ptr New_FSTree_TextEdit_text( const vfs::node*     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		return new vfs::node( parent, name, S_IFREG | 0600, &textedit_text_methods );
	}
	
}
