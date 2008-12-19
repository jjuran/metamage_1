/*	==================
 *	FSTree_new_icon.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_new_icon.hh"

// POSIX
#include <fcntl.h>

// Nitrogen
#include "Nitrogen/Icons.h"

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/View.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Nitrogen
{
	
	static void PlotIconHandle( const Rect&        area,
	                            IconAlignmentType  align,
	                            IconTransformType  transform,
	                            Handle             icon )
	{
		ThrowOSStatus( ::PlotIconHandle( &area, align, transform, icon ) );
	}
	
}

namespace Pedestal
{
	
	class Icon : public View
	{
		public:
			void Draw( const Rect& bounds );
			
			virtual Nitrogen::Handle Data() const = 0;
	};
	
	namespace N = Nitrogen;
	
	void Icon::Draw( const Rect& bounds )
	{
		if ( N::Handle data = Data() )
		{
			N::EraseRect( bounds );
			
			N::PlotIconHandle( bounds, N::kAlignNone, N::kTransformNone, data );
		}
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	typedef std::map< const FSTree*, NN::Shared< N::Handle > > IconMap;
	
	static IconMap gIconMap;
	
	
	class Icon : public Ped::Icon
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			Icon( Key key ) : itsKey( key )
			{
			}
			
			Nitrogen::Handle Data() const;
	};
	
	N::Handle Icon::Data() const
	{
		IconMap::const_iterator it = gIconMap.find( itsKey );
		
		if ( it != gIconMap.end() )
		{
			return it->second;
		}
		
		return N::Handle();
	}
	
	std::auto_ptr< Ped::View > IconFactory( const FSTree* delegate )
	{
		return std::auto_ptr< Ped::View >( new Icon( delegate ) );
	}
	
	
	void FSTree_new_icon::DestroyDelegate( const FSTree* delegate )
	{
		gIconMap.erase( delegate );
	}
	
	
	static std::size_t GetHandleSize_Checked( N::Handle h )
	{
		return h ? N::GetHandleSize( h ) : 0;
	}
	
	class IconDataFileHandle : public VirtualFileHandle
	{
		public:
			IconDataFileHandle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
			{
			}
			
			const FSTree* ViewKey() const;
			
			NN::Shared< N::Handle >& Data()        { return gIconMap[ ViewKey() ]; }
			N::Handle                Data() const  { return gIconMap[ ViewKey() ]; }
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return GetHandleSize_Checked( Data() ); }
			
			void SetEOF( off_t length )  {}
	};
	
	const FSTree* IconDataFileHandle::ViewKey() const
	{
		return GetFile()->ParentRef().get();
	}
	
	ssize_t IconDataFileHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		N::Handle data = Data();
		
		if ( data == NULL )
		{
			p7::throw_errno( EIO );
		}
		
		const std::size_t size = N::GetHandleSize( data );
		
		ASSERT( GetFileMark() <= size );
		
		byteCount = std::min( byteCount, size - GetFileMark() );
		
		char* p = *data.Get();
		
		std::copy( p + GetFileMark(),
		           p + GetFileMark() + byteCount,
		           buffer );
		
		return Advance( byteCount );
	}
	
	ssize_t IconDataFileHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		if ( byteCount != sizeof (N::PlainIcon)  &&  byteCount != sizeof (N::MaskedIcon) )
		{
			p7::throw_errno( EINVAL );
		}
		
		NN::Shared< N::Handle >& data = Data();
		
		if ( data.Get() == NULL )
		{
			data = N::NewHandle( byteCount );
		}
		else
		{
			N::SetHandleSize( data, byteCount );
		}
		
		char* p = *data.Get().Get();
		
		std::copy( buffer,
		           buffer + byteCount,
		           p );
		
		const FSTree* view = ViewKey();
		
		InvalidateWindowForView( view );
		
		// We ignore the file mark
		
		//return Advance( byteCount );
		return byteCount;
	}
	
	class FSTree_Icon_data : public FSTree
	{
		public:
			FSTree_Icon_data( const FSTreePtr&    parent,
			                  const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			N::Handle Data() const  { return gIconMap[ ParentRef().get() ]; }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const  { return GetHandleSize_Checked( Data() ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_Icon_data::Open( OpenFlags flags ) const
	{
		IOHandle* result = new IconDataFileHandle( shared_from_this(), flags );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	template < class FSTree_Type >
	static FSTreePtr Factory( const FSTreePtr&    parent,
	                          const std::string&  name )
	{
		return FSTreePtr( new FSTree_Type( parent, name ) );
	}
	
	const Functional_Traits< void >::Mapping Icon_view_Mappings[] =
	{
		{ "data", &Factory< FSTree_Icon_data > },
		
		{ NULL, NULL }
	};
	
}

