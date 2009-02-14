/*	==================
 *	FSTree_new_list.cc
 *	==================
 */

#include "Genie/FileSystem/FSTree_new_list.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"

// Pedestal
#include "Pedestal/ListView.hh"

// Genie
#include "Genie/IO/PropertyFile.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct ListParameters
	{
		std::vector< std::string >  itsStrings;
		bool                        itHasChanged;
		
		ListParameters() : itHasChanged()
		{
		}
	};
	
	typedef std::map< const FSTree*, ListParameters > ListParameterMap;
	
	static ListParameterMap gListParameterMap;
	
	
	class ListView : public Ped::ListView
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
		
		public:
			typedef Key Initializer;
			
			ListView( Key key ) : itsKey( key )
			{
			}
			
			void Draw( const Rect& bounds );
	};
	
	void ListView::Draw( const Rect& bounds )
	{
		ListParameters& params = gListParameterMap[ itsKey ];
		
		if ( params.itHasChanged )
		{
			params.itHasChanged = false;
			
			DeleteCells();
			
			const std::vector< std::string >& strings = params.itsStrings;
			
			typedef std::vector< std::string >::const_iterator Iter;
			
			for ( Iter it = strings.begin();  it != strings.end();  ++it )
			{
				const std::string& s = *it;
				
				AppendCell( s.data(), s.size() );
			}
		}
		
		Ped::ListView::Draw( bounds );
	}
	
	
	boost::shared_ptr< Ped::View > ListFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new ListView( delegate ) );
	}
	
	
	void FSTree_new_list::DestroyDelegate( const FSTree* delegate )
	{
		gListParameterMap.erase( delegate );
	}
	
	
	class List_data_Handle : public VirtualFileHandle
	{
		public:
			List_data_Handle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return GetFile()->GetEOF(); }
			
			void SetEOF( off_t length )  { return GetFile()->SetEOF( length ); }
	};
	
	boost::shared_ptr< IOHandle > List_data_Handle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new List_data_Handle( GetFile(), GetFlags() ) );
	}
	
	ssize_t List_data_Handle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		if ( byteCount != 0  &&  buffer[ byteCount - 1 ] != '\n' )
		{
			p7::throw_errno( EINVAL );
		}
		
		const FSTree* view = GetFile()->ParentRef().get();
		
		ListParameters& params = gListParameterMap[ view ];
		
		std::vector< std::string >& strings = params.itsStrings;
		
		const char* end = buffer + byteCount;
		
		const char* p = buffer;
		
		while ( p < end )
		{
			const char* q = std::find( p, end, '\n' );
			
			strings.push_back( std::string( p, q ) );
			
			p = q + 1;
			
			params.itHasChanged = true;
		}
		
		// FIXME:  Ignores the file mark
		
		InvalidateWindowForView( view );
		
		return Advance( byteCount );
	}
	
	
	class FSTree_List_data : public FSTree
	{
		public:
			FSTree_List_data( const FSTreePtr&    parent,
			                  const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const;
			
			void SetEOF( off_t length ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	static std::size_t measure_strings( const std::vector< std::string >& strings )
	{
		std::size_t result = 0;
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = strings.begin();  it != strings.end();  ++it )
		{
			result += it->size() + 1;
		}
		
		return result;
	}
	
	off_t FSTree_List_data::GetEOF() const
	{
		return measure_strings( gListParameterMap[ ParentRef().get() ].itsStrings );
	}
	
	void FSTree_List_data::SetEOF( off_t length ) const
	{
		if ( length != 0 )
		{
			p7::throw_errno( EINVAL );
		}
		
		ListParameters& params = gListParameterMap[ ParentRef().get() ];
		
		params.itsStrings.clear();
		
		params.itHasChanged = true;
	}
	
	static std::string join_strings( const std::vector< std::string >& strings )
	{
		std::string result;
		
		typedef std::vector< std::string >::const_iterator Iter;
		
		for ( Iter it = strings.begin();  it != strings.end();  ++it )
		{
			result += *it;
			result += '\n';
		}
		
		return result;
	}
	
	boost::shared_ptr< IOHandle > FSTree_List_data::Open( OpenFlags flags ) const
	{
		IOHandle* result = NULL;
		
		if ( flags == O_RDONLY )
		{
			std::string data = join_strings( gListParameterMap[ ParentRef().get() ].itsStrings );
			
			result = new PropertyReaderFileHandle( shared_from_this(), flags, data );
		}
		else if (    (flags & ~O_CREAT) - O_WRONLY == O_TRUNC
		          || (flags & ~O_CREAT) - O_WRONLY == O_APPEND )
		{
			result = new List_data_Handle( shared_from_this(), flags );
		}
		else
		{
			throw p7::errno_t( EINVAL );
		}
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	const FSTree_Premapped::Mapping List_view_Mappings[] =
	{
		{ "data", &Basic_Factory< FSTree_List_data > },
		
		{ NULL, NULL }
	};
	
}

