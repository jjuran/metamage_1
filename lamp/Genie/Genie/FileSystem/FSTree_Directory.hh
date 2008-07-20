/*	===================
 *	FSTree_Directory.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH
#define GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH

// Standard C++
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

// POSIX
#include <sys/stat.h>

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/Errno.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/FileSystem/FSTree_Null.hh"
#include "Genie/FileSystem/FSTree_Singleton.hh"


namespace Genie
{
	
	template < class Pointer > struct Pointer_KeyName_Traits
	{
		typedef Pointer Key;
		
		static std::string NameFromKey( const Key& key )
		{
			return BitsAndBytes::EncodeAsHex( key );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			return reinterpret_cast< Key >( DecodeHex32( &*name.begin(), &*name.end() ) );
		}
	};
	
	struct Int_KeyName_Traits
	{
		typedef int Key;
		
		static std::string NameFromKey( const Key& key )
		{
			return Nucleus::Convert< std::string >( key );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			//return Nucleus::Convert< Integer >( name );
			return Key( std::atoi( name.c_str() ) );
		}
	};
	
	template < class Integer > struct Integer_KeyName_Traits
	{
		typedef Integer Key;
		
		static std::string NameFromKey( const Key& key )
		{
			return Int_KeyName_Traits::NameFromKey( key );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			return Key( Int_KeyName_Traits::KeyFromName( name ) );
		}
	};
	
	
	typedef std::vector< FSNode > FSTreeCache;
	
	
	class FSTree_Directory : public FSTree
	{
		public:
			FSTree_Directory( const FSTreePtr&    parent,
			                  const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			virtual ~FSTree_Directory();
			
			bool IsFile     () const  { return false; }
			bool IsDirectory() const  { return true;  }
			
			mode_t FileTypeMode() const  { return S_IFDIR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR | S_IXUSR; }
			
			FSTreePtr Lookup( const std::string& name ) const;
			
			virtual FSTreePtr Lookup_Child( const std::string& name ) const = 0;
			
			FSIteratorPtr Iterate() const;
			
			virtual void IterateIntoCache( FSTreeCache& cache ) const = 0;
	};
	
	
	template < class Details >
	class FSTree_Sequence : public FSTree_Directory
	{
		protected:
			Details itsDetails;
			
			typedef typename Details::Key Key;
		
		public:
			FSTree_Sequence( const FSTreePtr&    parent,
			                 const std::string&  name ) : FSTree_Directory( parent, name ),
			                                              itsDetails()
			{
			}
			
			FSTree_Sequence( const FSTreePtr&    parent,
			                 const std::string&  name,
			                 const Details&      details ) : FSTree_Directory( parent, name ),
			                                                 itsDetails( details )
			{
			}
			
			FSTreePtr Lookup_Child( const std::string& name ) const
			{
				Key key = itsDetails.KeyFromName( name );
				
				if ( !itsDetails.KeyIsValid( key ) )
				{
					poseven::throw_errno( ENOENT );
				}
				
				return itsDetails.GetChildNode( shared_from_this(), name, key );
			}
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	template < class Details >
	class IteratorConverter
	{
		private:
			FSTreePtr  itsParent;
			Details    itsDetails;
		
		public:
			IteratorConverter( const FSTreePtr&  parent,
			                   const Details&    details ) : itsParent ( parent  ),
			                                                 itsDetails( details )
			{
			}
			
			FSNode operator()( const Details::Sequence::value_type& value ) const
			{
				typename Details::Key key = itsDetails.KeyFromValue( value );
				
				std::string name = itsDetails.NameFromKey( key );
				
				FSTreePtr node( itsDetails.GetChildNode( itsParent, name, key ) );
				
				return FSNode( name, node );
			}
	};
	
	template < class Details >
	void FSTree_Sequence< Details >::IterateIntoCache( FSTreeCache& cache ) const
	{
		IteratorConverter< Details > converter( shared_from_this(), itsDetails );
		
		std::transform( itsDetails.ItemSequence().begin(),
		                itsDetails.ItemSequence().end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	class FSTree_Mappable : public FSTree_Directory
	{
		protected:
			typedef std::map< std::string, FSTreePtr > Mappings;
			
			Mappings mappings;
		
		public:
			FSTree_Mappable( const FSTreePtr&    parent,
			                 const std::string&  name ) : FSTree_Directory( parent, name )
			{
			}
			
			virtual ~FSTree_Mappable();
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void Map( FSTreePtr tree );
			
			FSTreePtr Lookup_Mapping( const std::string& name ) const;
			
			virtual FSTreePtr Lookup_Regular( const std::string& name ) const = 0;
	};
	
	
	template < class Key >
	struct Keyed_Function_Traits
	{
		typedef FSTreePtr (*Function)( const FSTreePtr&, const std::string&, Key key );
	};
	
	template <>
	struct Keyed_Function_Traits< void >
	{
		typedef FSTreePtr (*Function)( const FSTreePtr&, const std::string& );
	};
	
	template < class Key >
	struct Functional_Traits : Keyed_Function_Traits< Key >
	{
		typedef typename Keyed_Function_Traits< Key >::Function Function;
		
		struct Mapping
		{
			const char*  name;
			Function     f;
		};
	};
	
	template < class Key >
	struct Keyed_Invoker_Details : Functional_Traits< Key >
	{
		typedef typename Functional_Traits< Key >::Function Function;
		
		Key itsKey;
		
		Keyed_Invoker_Details( Key key ) : itsKey( key )
		{
		}
		
		FSTreePtr Invoke( Function f, const FSTreePtr& parent, const std::string& name ) const
		{
			return f( parent, name, itsKey );
		}
	};
	
	template <>
	struct Keyed_Invoker_Details< void > : Functional_Traits< void >
	{
		FSTreePtr Invoke( Function f, const FSTreePtr& parent, const std::string& name ) const
		{
			return f( parent, name );
		}
	};
	
	template < class Key >
	class FSTree_Functional : public FSTree_Directory, public Keyed_Invoker_Details< Key >
	{
		private:
			typedef Keyed_Invoker_Details< Key > Details;
			
			typedef typename Details::Function  Function;
			typedef typename Details::Mapping   Mapping;
			
			typedef std::map< std::string, Function > Mappings;
			
			Mappings itsMappings;
		
		public:
			FSTree_Functional( const FSTreePtr&    parent,
			                   const std::string&  name ) : FSTree_Directory( parent, name )
			{
			}
			
			template < class Key >
			FSTree_Functional( const FSTreePtr&    parent,
			                   const std::string&  name,
			                   const Key&          key ) : FSTree_Directory( parent, name ),
			                                               Details( key )
			{
			}
			
			void Map( const std::string& name, Function f );
			
			void AddMappings( const Mapping* begin );
			void AddMappings( const Mapping* begin, const Mapping* end );
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	template < class Key >
	void FSTree_Functional< Key >::Map( const std::string& name, Function f )
	{
		itsMappings[ name ] = f;
	}
	
	template < class Key >
	void FSTree_Functional< Key >::AddMappings( const Mapping* array )
	{
		while ( array->name != NULL )
		{
			Map( array->name, array->f );
			
			++array;
		}
	}
	
	template < class Key >
	void FSTree_Functional< Key >::AddMappings( const Mapping* begin,
	                                            const Mapping* end )
	{
		for ( const Mapping* it = begin;  it != end;  ++it )
		{
			Map( it->name, it->f );
		}
	}
	
	template < class Key >
	FSTreePtr FSTree_Functional< Key >::Lookup_Child( const std::string& name ) const
	{
		Mappings::const_iterator it = itsMappings.find( name );
		
		if ( it == itsMappings.end() )
		{
			return FSNull();
		}
		
		const Function& f = it->second;
		
		return Details::Invoke( f, shared_from_this(), name );
	}
	
	template < class Key >
	void FSTree_Functional< Key >::IterateIntoCache( FSTreeCache& cache ) const
	{
		typedef Mappings::const_iterator Iter;
		
		for ( Iter it = itsMappings.begin();  it != itsMappings.end();  ++it )
		{
			const std::string& name = it->first;
			
			const Function& f = it->second;
			
			try
			{
				FSTreePtr tree( Details::Invoke( f, shared_from_this(), name ) );
				
				cache.push_back( FSNode( name, tree ) );
			}
			catch ( ... )
			{
			}
		}
	}
	
	
	typedef FSTree_Functional< void >::Mapping Singleton_Mapping;
	
	template < class FSTree_Type >
	FSTreePtr Singleton_Factory( const FSTreePtr& parent, const std::string& name )
	{
		return GetSingleton< FSTree_Type >( parent, name );
	}
	
	template < const Singleton_Mapping mappings[] >
	FSTreePtr Premapped_Factory( const FSTreePtr& parent, const std::string& name )
	{
		FSTree_Functional< void >* raw_ptr = new FSTree_Functional< void >( parent, name );
		
		FSTreePtr result( raw_ptr );
		
		raw_ptr->AddMappings( mappings );
		
		return result;
	}
	
	template < class Key, const typename FSTree_Functional< Key >::Mapping mappings[] >
	FSTreePtr Premapped_Factory( const FSTreePtr& parent, const std::string& name, Key key )
	{
		FSTree_Functional< Key >* raw_ptr = new FSTree_Functional< Key >( parent, name, key );
		
		FSTreePtr result( raw_ptr );
		
		raw_ptr->AddMappings( mappings );
		
		return result;
	}
	
	
	UInt32 DecodeHex32( const char* begin, const char* end );
	
}

#endif

