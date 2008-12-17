/*	=====================
 *	FSTree_new_caption.cc
 *	=====================
 */

#include "Genie/FileSystem/FSTree_new_caption.hh"

// POSIX
#include <fcntl.h>

// Pedestal
#include "Pedestal/Caption.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
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
	
	std::auto_ptr< Ped::View > CaptionFactory( const FSTree* delegate )
	{
		return std::auto_ptr< Ped::View >( new Caption( delegate ) );
	}
	
	namespace
	{
		
		void DestroyDelegate( const FSTree* delegate )
		{
			gCaptionTextMap.erase( delegate );
		}
		
	}
	
	FSTreePtr FSTree_new_caption::MakeDelegate( const FSTreePtr& parent, const std::string& name ) const
	{
		return Premapped_Factory< Caption_view_Mappings, &DestroyDelegate >( parent, name );
	}
	
	
	static void CaptionText_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->Parent().get();
		
		gCaptionTextMap[ view ].resize( length );
		
		InvalidateWindowForView( view );
	}
	
	class CaptionTextFileHandle : public VirtualFileHandle
	{
		public:
			CaptionTextFileHandle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
			{
			}
			
			const FSTree* ViewKey() const;
			
			std::string& String() const  { return gCaptionTextMap[ ViewKey() ]; }
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t length )  { CaptionText_SetEOF( GetFile().get(), length ); }
	};
	
	const FSTree* CaptionTextFileHandle::ViewKey() const
	{
		return GetFile()->Parent().get();
	}
	
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
		
		const FSTree* view = ViewKey();
		
		InvalidateWindowForView( view );
		
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
			
			void SetEOF( off_t length ) const  { CaptionText_SetEOF( this, length ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_Caption_text::Open( OpenFlags flags ) const
	{
		IOHandle* result = new CaptionTextFileHandle( shared_from_this(), flags );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	template < class FSTree_Type >
	static FSTreePtr Factory( const FSTreePtr&    parent,
	                          const std::string&  name )
	{
		return FSTreePtr( new FSTree_Type( parent, name ) );
	}
	
	const Functional_Traits< void >::Mapping Caption_view_Mappings[] =
	{
		{ "text", &Factory< FSTree_Caption_text > },
		
		{ NULL, NULL }
	};
	
}

