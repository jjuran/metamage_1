/*
	Document.cc
	-----------
*/

#include "TestEdit/Document.hh"

// Standard C++
#include <algorithm>

// mac-file-utils
#include "mac_file/file_traits.hh"
#include "mac_file/open_data_fork.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// mac-app-utils
#include "mac_app/Window_menu.hh"

// plus
#include "plus/string.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// Pedestal
#include "Pedestal/WindowStorage.hh"

// TestEdit
#include "TestEdit/Window.hh"


namespace TestEdit
{
	
	namespace Ped = Pedestal;
	
	
	template < class FileSpec >
	static plus::string ReadFileData( const FileSpec& file )
	{
		using mac::file::FSIORefNum;
		using mac::file::file_traits;
		
		typedef file_traits< FileSpec > traits;
		
		typedef typename traits::file_size_t file_size_t;
		
		plus::string result;
		
		FSIORefNum refnum = mac::file::open_data_fork( file, fsRdPerm );
		
		if ( refnum >= 0 )
		{
			file_size_t size = traits::geteof( refnum );
			
			if ( (file_size_t) -1 > (size_t) -1  &&  size > (size_t) -1 )
			{
				/*
					When we have 64-bit file sizes in 32-bit Mac OS,
					it's possible that a size could overflow size_t.
				*/
				
				size = memFullErr;  // propagate the error
			}
			else if ( size > 0 )
			{
				if ( char* p = result.reset_nothrow( size ) )
				{
					long n_read = traits::read( refnum, p, size );
					
					if ( n_read == size )
					{
						// Allow LF newlines
						std::replace( p,
						              p + n_read,
						              '\n',
						              '\r' );
					}
					else
					{
						size = n_read < 0 ? n_read
						                  : eofErr;  // propagate the error
					}
				}
				else
				{
					size = memFullErr;  // propagate the error
				}
			}
			else
			{
				// empty file -- no need to read or post=process
			}
			
			traits::close( refnum );
			
			if ( size < 0 )
			{
				refnum = size;  // propagate the error
			}
		}
		
		if ( refnum < 0 )
		{
			Mac::ThrowOSStatus( refnum );
		}
		
		return result;
	}
	
	static inline
	void set_window_title_to_filename( WindowRef window, const FSRef& file )
	{
		HFSUniStr255 name;
		
		OSErr err = FSGetCatalogInfo( &file, 0, NULL, &name, NULL, NULL );
		
		if ( err == noErr )
		{
			mac::ui::set_window_title( window, name );
		}
	}
	
	
	Document::Document()
	: 
		itsWindow( NewWindow().release() )
	{
	}
	
	static
	void LoadText( WindowRef window, const plus::string& text )
	{
		Ped::View* view = Ped::get_window_view( window );
		
		View& scrollframe = reinterpret_cast< View& >( *view );
		
		SetText( scrollframe, text );
	}
	
	Document::Document( const FSSpec& file )
	{
		const plus::string& text = ReadFileData( file );
		
		nucleus::owned< WindowRef > window = NewWindow( file.name );
		
		itsWindow = window.get();
		
		LoadText( window, text );
		
		window.release();
	}
	
	Document::Document( const FSRef& file )
	{
		const plus::string& text = ReadFileData( file );
		
		nucleus::owned< WindowRef > window = NewWindow();
		
		itsWindow = window.get();
		
		set_window_title_to_filename( window, file );
		
		mac::app::Window_menu_remove( window );
		mac::app::Window_menu_insert( window );
		
		LoadText( window, text );
		
		window.release();
	}
	
}
