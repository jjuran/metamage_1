/*	=====================
 *	FSTree_new_caption.cc
 *	=====================
 */

#include "Genie/FileSystem/FSTree_new_caption.hh"

// POSIX
#include <fcntl.h>

// Nucleus
#include "Nucleus/Saved.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Pedestal
#include "Pedestal/Caption.hh"
#include "Pedestal/GraphicView.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/Views.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	static void InvalidateCurrentWindow()
	{
		N::InvalRect( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	static void InvalidateWindowRef( N::WindowRef window )
	{
		NN::Saved< N::Port_Value > savePort;
		
		N::SetPortWindowPort( window );
		
		InvalidateCurrentWindow();
	}
	
	
	extern const Functional_Traits< void >::Mapping Caption_view_Mappings[];
	
	typedef std::map< const FSTree*, std::string > CaptionTextMap;
	
	static CaptionTextMap gCaptionTextMap;
	
	
	class Caption : public Ped::Caption
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			Caption( Key key ) : itsKey( key )
			{
			}
			
			std::string Text() const;
	};
	
	std::string Caption::Text() const
	{
		std::string result;
		
		CaptionTextMap::const_iterator it = gCaptionTextMap.find( itsKey );
		
		if ( it != gCaptionTextMap.end() )
		{
			result = it->second;
		}
		
		std::replace( result.begin(), result.end(), '\n', '\r' );
		
		return result;
	}
	
	
	class CaptionFactory : public ViewFactory
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			CaptionFactory( Key key ) : itsKey( key )
			{
			}
			
			void operator()( Ped::UserWindow& window ) const
			{
				Rect bounds = N::GetPortBounds( N::GetQDGlobalsThePort() );
				
				typedef Ped::GraphicView< Caption > View;
				
				std::auto_ptr< Ped::View > view( new View( bounds, itsKey ) );
				
				window.SetView( view );
				
				N::InvalRect( bounds );
			}
	};
	
	namespace
	{
		
		void DestroyDelegate( const FSTree* delegate )
		{
			gCaptionTextMap.erase( delegate );
		}
		
	}
	
	void FSTree_new_caption::HardLink( const FSTreePtr& target ) const
	{
		const FSTreePtr& parent = target->Parent();
		
		const FSTree* key = parent.get();
		
		FSTreePtr delegate = Premapped_Factory< Caption_view_Mappings, &DestroyDelegate >( parent, "view" );
		
		boost::shared_ptr< ViewFactory > factory( new CaptionFactory( delegate.get() ) );
		
		AddViewDelegate( key, delegate );
		
		AddViewFactory( key, factory );
		
		target->CreateDirectory( 0 );  // mode is ignored
	}
	
	
	class CaptionTextFileHandle : public VirtualFileHandle
	{
		public:
			CaptionTextFileHandle( const FSTreePtr& file ) : VirtualFileHandle( file )
			{
			}
			
			const FSTree* ViewKey() const  { return GetFile()->Parent().get(); }
			
			std::string& String() const  { return gCaptionTextMap[ ViewKey() ]; }
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t eof )  { String().resize( eof ); }
	};
	
	ssize_t CaptionTextFileHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		std::string& s = String();
		
		ASSERT( GetFileMark() <= s.size() );
		
		byteCount = std::min( byteCount, s.size() - GetFileMark() );
		
		std::copy( s.begin() + GetFileMark(),
		           s.begin() + GetFileMark() + byteCount,
		           buffer );
		
		return Advance( byteCount );
	}
	
	ssize_t CaptionTextFileHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		std::string& s = String();
		
		if ( GetFileMark() + byteCount > s.size() )
		{
			s.resize( GetFileMark() + byteCount );
		}
		
		std::copy( buffer,
		           buffer + byteCount,
		           s.begin() + GetFileMark() );
		
		InvalidateWindowRef( GetWindowRef( ViewKey()->Parent().get() ) );
		
		return Advance( byteCount );
	}
	
	class FSTree_Caption_text : public FSTree
	{
		public:
			FSTree_Caption_text( const FSTreePtr&    parent,
			                     const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			std::string& String() const  { return gCaptionTextMap[ Parent().get() ]; }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t eof ) const  { String().resize( eof ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_Caption_text::Open( OpenFlags flags ) const
	{
		if ( const bool truncating  = flags & O_TRUNC )
		{
			SetEOF( 0 );
			
			InvalidateWindowRef( GetWindowRef( Parent()->Parent().get() ) );
		}
		
		IOHandle* result = new CaptionTextFileHandle( shared_from_this() );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	template < class FSTree_Type >
	static FSTreePtr Factory( const FSTreePtr&    parent,
	                          const std::string&  name )
	{
		return MakeFSTree( new FSTree_Type( parent, name ) );
	}
	
	const Functional_Traits< void >::Mapping Caption_view_Mappings[] =
	{
		{ "text", &Factory< FSTree_Caption_text > },
		
		{ NULL, NULL }
	};
	
}

