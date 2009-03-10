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
#include "Genie/FileSystem/FSTree_Property.hh"
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
	
	class PlainIcon : public View
	{
		public:
			void Draw( const Rect& bounds, bool erasing );
			
			virtual Nitrogen::Handle Data() const = 0;
			
			virtual Nitrogen::IconAlignmentType Alignment() const  { return Nitrogen::kAlignNone; }
			
			virtual Nitrogen::IconTransformType Transform() const  { return Nitrogen::kTransformNone; }
	};
	
	namespace N = Nitrogen;
	
	void PlainIcon::Draw( const Rect& bounds, bool erasing )
	{
		if ( N::Handle data = Data() )
		{
			if ( erasing )
			{
				N::EraseRect( bounds );
			}
			
			N::PlotIconHandle( bounds, Alignment(), Transform(), data );
		}
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct PlainIcon_Parameters
	{
		NN::Shared< N::Handle >  data;
		N::IconAlignmentType     align;
		N::IconTransformType     xform;
		
		PlainIcon_Parameters() : align(), xform()
		{
		}
	};
	
	typedef std::map< const FSTree*, PlainIcon_Parameters > PlainIconMap;
	
	static PlainIconMap gPlainIconMap;
	
	
	class PlainIcon : public Ped::PlainIcon
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			PlainIcon( Key key ) : itsKey( key )
			{
			}
			
			Nitrogen::Handle Data() const;
			
			Nitrogen::IconAlignmentType Alignment() const;
			
			Nitrogen::IconTransformType Transform() const;
	};
	
	N::Handle PlainIcon::Data() const
	{
		return gPlainIconMap[ itsKey ].data;
	}
	
	N::IconAlignmentType PlainIcon::Alignment() const
	{
		return gPlainIconMap[ itsKey ].align;
	}
	
	N::IconTransformType PlainIcon::Transform() const
	{
		return gPlainIconMap[ itsKey ].xform;
	}
	
	boost::shared_ptr< Ped::View > IconFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new PlainIcon( delegate ) );
	}
	
	
	void FSTree_new_icon::DestroyDelegate( const FSTree* delegate )
	{
		gPlainIconMap.erase( delegate );
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
			
			boost::shared_ptr< IOHandle > Clone();
			
			const FSTree* ViewKey() const;
			
			NN::Shared< N::Handle >& Data()        { return gPlainIconMap[ ViewKey() ].data; }
			N::Handle                Data() const  { return gPlainIconMap[ ViewKey() ].data; }
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return GetHandleSize_Checked( Data() ); }
			
			void SetEOF( off_t length )  {}
	};
	
	boost::shared_ptr< IOHandle > IconDataFileHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new IconDataFileHandle( GetFile(), GetFlags() ) );
	}
	
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
			
			N::Handle Data() const  { return gPlainIconMap[ ParentRef().get() ].data; }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const  { return GetHandleSize_Checked( Data() ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_Icon_data::Open( OpenFlags flags ) const
	{
		IOHandle* result = new IconDataFileHandle( Self(), flags );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	namespace
	{
		
		N::IconAlignmentType& Alignment( const FSTree* view )
		{
			return gPlainIconMap[ view ].align;
		}
		
		N::IconTransformType& Transform( const FSTree* view )
		{
			return gPlainIconMap[ view ].xform;
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
	
	const FSTree_Premapped::Mapping Icon_view_Mappings[] =
	{
		{ "data", &Basic_Factory< FSTree_Icon_data > },
		
		{ "align", &Property_Factory< View_Property< Integer_Scribe< N::IconAlignmentType >, Alignment > > },
		{ "xform", &Property_Factory< View_Property< Integer_Scribe< N::IconTransformType >, Transform > > },
		
		{ NULL, NULL }
	};
	
}

