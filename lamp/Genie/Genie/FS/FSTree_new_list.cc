/*	==================
 *	FSTree_new_list.cc
 *	==================
 */

#include "Genie/FS/FSTree_new_list.hh"

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/Errno.hh"

// Pedestal
#include "Pedestal/ListView.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/Views.hh"
#include "Genie/IO/PropertyFile.hh"
#include "Genie/IO/RegularFile.hh"
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
		bool                        itIntersectsGrowBox;
		bool                        bounds_changed;
		bool                        data_changed;
		
		ListParameters() : itIntersectsGrowBox(),
		                   bounds_changed(),
		                   data_changed()
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
			
			bool IntersectsGrowBox() const;
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	bool ListView::IntersectsGrowBox() const
	{
		return gListParameterMap[ itsKey ].itIntersectsGrowBox;
	}
	
	void ListView::Draw( const Rect& bounds, bool erasing )
	{
		ListParameters& params = gListParameterMap[ itsKey ];
		
		if ( params.bounds_changed )
		{
			params.bounds_changed = false;
			
			SetBounds( bounds );
		}
		
		if ( params.data_changed )
		{
			params.data_changed = false;
			
			DeleteCells();
			
			const std::vector< std::string >& strings = params.itsStrings;
			
			typedef std::vector< std::string >::const_iterator Iter;
			
			for ( Iter it = strings.begin();  it != strings.end();  ++it )
			{
				const std::string& s = *it;
				
				AppendCell( s.data(), s.size() );
			}
		}
		
		Ped::ListView::Draw( bounds, erasing );
	}
	
	
	static boost::shared_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new ListView( delegate ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		gListParameterMap.erase( delegate );
	}
	
	
	class List_data_Handle : public VirtualFileHandle< StreamHandle >
	{
		public:
			List_data_Handle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
			{
			}
			
			unsigned int SysPoll()  { return kPollRead | kPollWrite; }
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
	};
	
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
			
			params.data_changed = true;
		}
		
		// FIXME:  Ignores the file mark
		
		InvalidateWindowForView( view );
		
		return byteCount;
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
		
		params.data_changed = true;
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
			
			result = new PropertyReaderFileHandle( Self(), flags, data );
		}
		else if (    (flags & ~O_CREAT) - O_WRONLY == O_TRUNC
		          || (flags & ~O_CREAT) - O_WRONLY == O_APPEND )
		{
			result = new List_data_Handle( Self(), flags );
		}
		else
		{
			throw p7::errno_t( EINVAL );
		}
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	namespace
	{
		
		bool& Overlap( const FSTree* view )
		{
			return gListParameterMap[ view ].itIntersectsGrowBox;
		}
		
	}
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Get,
		                            &Property::Set );
	}
	
	template < class Scribe, typename Scribe::Value& (*Access)( const FSTree* ) >
	struct List_Property : View_Property< Scribe, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			View_Property< Scribe, Access >::Set( that, begin, end, binary );
			
			const FSTree* view = GetViewKey( that );
			
			gListParameterMap[ view ].bounds_changed = true;
		}
	};
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ "data", &Basic_Factory< FSTree_List_data > },
		
		{ "overlap", &Property_Factory< List_Property< Boolean_Scribe, Overlap > > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_list( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

