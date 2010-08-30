/*	=====================
 *	FSTree_new_caption.cc
 *	=====================
 */

#include "Genie/FS/FSTree_new_caption.hh"

// POSIX
#include <fcntl.h>

// plus
#include "plus/var_string.hh"

// Pedestal
#include "Pedestal/Caption.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/port/ADDR.hh"
#include "Genie/FS/Views.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/VirtualFile.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	struct CaptionParameters
	{
		plus::var_string  itsText;
		bool              itIsWrapped;
		bool              disabling;
		
		CaptionParameters() : itIsWrapped( true ), disabling()
		{
		}
	};
	
	typedef simple_map< const FSTree*, CaptionParameters > CaptionParametersMap;
	
	static CaptionParametersMap gCaptionParametersMap;
	
	
	class Caption : public Ped::Caption
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
			bool  itIsActive;
		
		public:
			Caption( Key key ) : itsKey( key ), itIsActive( true )
			{
			}
			
			plus::string Text() const;
			
			bool Wrapped() const;
			bool Disabled() const;
			
			void Activate( bool activating )  { itIsActive = activating; }
	};
	
	plus::string Caption::Text() const
	{
		plus::var_string result;
		
		if ( CaptionParameters* it = gCaptionParametersMap.find( itsKey ) )
		{
			result = it->itsText;
		}
		
		std::replace( result.begin(), result.end(), '\n', '\r' );
		
		return result;
	}
	
	bool Caption::Wrapped() const
	{
		if ( CaptionParameters* it = gCaptionParametersMap.find( itsKey ) )
		{
			return it->itIsWrapped;
		}
		
		return true;
	}
	
	bool Caption::Disabled() const
	{
		if ( CaptionParameters* it = gCaptionParametersMap.find( itsKey ) )
		{
			return !itIsActive && it->disabling;
		}
		
		return false;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return seize_ptr( new Caption( delegate ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		gCaptionParametersMap.erase( delegate );
	}
	
	
	static void CaptionText_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->ParentRef().get();
		
		gCaptionParametersMap[ view ].itsText.resize( length );
		
		InvalidateWindowForView( view );
	}
	
	class CaptionTextFileHandle : public VirtualFileHandle< RegularFileHandle >
	{
		public:
			CaptionTextFileHandle( const FSTreePtr& file, OpenFlags flags )
			:
				VirtualFileHandle< RegularFileHandle >( file, flags )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			const FSTree* ViewKey();
			
			plus::var_string& String()  { return gCaptionParametersMap[ ViewKey() ].itsText; }
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return String().size(); }
			
			void SetEOF( off_t length )  { CaptionText_SetEOF( GetFile().get(), length ); }
	};
	
	boost::shared_ptr< IOHandle > CaptionTextFileHandle::Clone()
	{
		return seize_ptr( new CaptionTextFileHandle( GetFile(), GetFlags() ) );
	}
	
	const FSTree* CaptionTextFileHandle::ViewKey()
	{
		return GetFile()->ParentRef().get();
	}
	
	ssize_t CaptionTextFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const plus::string& s = String();
		
		if ( offset >= s.size() )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, s.size() - offset );
		
		memcpy( buffer, &s[ offset ], n_bytes );
		
		return n_bytes;
	}
	
	ssize_t CaptionTextFileHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		plus::var_string& s = String();
		
		if ( offset + n_bytes > s.size() )
		{
			s.resize( offset + n_bytes );
		}
		
		std::copy( buffer,
		           buffer + n_bytes,
		           s.begin() + offset );
		
		const FSTree* view = ViewKey();
		
		InvalidateWindowForView( view );
		
		return n_bytes;
	}
	
	class FSTree_Caption_text : public FSTree
	{
		public:
			FSTree_Caption_text( const FSTreePtr&     parent,
			                     const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			plus::var_string& String() const  { return gCaptionParametersMap[ ParentRef().get() ].itsText; }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t length ) const  { CaptionText_SetEOF( this, length ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_Caption_text::Open( OpenFlags flags ) const
	{
		return seize_ptr( new CaptionTextFileHandle( Self(), flags ) );
	}
	
	
	namespace
	{
		
		bool& Wrapped( const FSTree* view )
		{
			return gCaptionParametersMap[ view ].itIsWrapped;
		}
		
		bool& Disabling( const FSTree* view )
		{
			return gCaptionParametersMap[ view ].disabling;
		}
		
	}
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Get,
		                            &Property::Set );
	}
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ "text", &Basic_Factory< FSTree_Caption_text > },
		
		{ "wrapped",   &Property_Factory< View_Property< Boolean_Scribe, Wrapped   > > },
		{ "disabling", &Property_Factory< View_Property< Boolean_Scribe, Disabling > > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_caption( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return seize_ptr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

