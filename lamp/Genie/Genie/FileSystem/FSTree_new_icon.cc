/*	==================
 *	FSTree_new_icon.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_new_icon.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/Icons.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/Icons.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct PlainIcon_Parameters : Icon_Parameters
	{
		NN::Shared< N::Handle >  data;
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
	
	
	class IconDataFileHandle : public VirtualFileHandle
	{
		private:
			N::Handle itsData;
		
		public:
			IconDataFileHandle( const FSTreePtr&  file,
			                    OpenFlags         flags,
			                    N::Handle         data )
			:
				VirtualFileHandle( file, flags ),
				itsData( data )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			const FSTree* ViewKey() const;
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return N::GetHandleSize( itsData ); }
			
			void SetEOF( off_t length )  {}
	};
	
	boost::shared_ptr< IOHandle > IconDataFileHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new IconDataFileHandle( GetFile(), GetFlags(), itsData ) );
	}
	
	const FSTree* IconDataFileHandle::ViewKey() const
	{
		return GetFile()->ParentRef().get();
	}
	
	ssize_t IconDataFileHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		ASSERT( itsData != NULL );
		
		const std::size_t size = N::GetHandleSize( itsData );
		
		if ( size == 0 )
		{
			p7::throw_errno( EIO );
		}
		
		ASSERT( GetFileMark() <= size );
		
		byteCount = std::min( byteCount, size - GetFileMark() );
		
		char* p = *itsData.Get();
		
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
		
		ASSERT( itsData != NULL );
		
		N::SetHandleSize( itsData, byteCount );
		
		char* p = *itsData.Get();
		
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
		private:
			NN::Shared< N::Handle > itsData;
		
		public:
			FSTree_Icon_data( const FSTreePtr&                parent,
			                  const std::string&              name,
			                  const NN::Shared< N::Handle >&  data )
			:
				FSTree( parent, name ),
				itsData( data )
			{
				ASSERT( data.Get() != NULL );
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const  { return N::GetHandleSize( itsData ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_Icon_data::Open( OpenFlags flags ) const
	{
		IOHandle* result = new IconDataFileHandle( Self(), flags, itsData );
		
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
	
	static FSTreePtr Data_Factory( const FSTreePtr&    parent,
	                               const std::string&  name )
	{
		NN::Shared< N::Handle >& data = gPlainIconMap[ parent.get() ].data;
		
		if ( data.Get() == NULL )
		{
			data = N::NewHandle( 0 );  // handle must be allocated
		}
		
		return FSTreePtr( new FSTree_Icon_data( parent, name, data ) );
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
		{ "data", &Data_Factory },
		
		{ "align", &Property_Factory< View_Property< Integer_Scribe< N::IconAlignmentType >, Alignment > > },
		{ "xform", &Property_Factory< View_Property< Integer_Scribe< N::IconTransformType >, Transform > > },
		
		{ NULL, NULL }
	};
	
}

