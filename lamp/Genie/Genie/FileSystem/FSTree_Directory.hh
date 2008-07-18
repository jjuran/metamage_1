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
			FSTree_Sequence() : itsDetails()
			{
			}
			
			FSTree_Sequence( const FSTreePtr& parent ) : itsDetails()
			{
			}
			
			FSTree_Sequence( const Details& details ) : itsDetails( details )
			{
			}
			
			FSTree_Sequence( const FSTreePtr&  parent,
			                 const Details&    details ) : itsDetails( details )
			{
			}
			
			static std::string OnlyName()  { return Details::Name(); }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return itsDetails.Parent(); }
			
			FSTreePtr Lookup_Child( const std::string& name ) const
			{
				Key key = itsDetails.KeyFromName( name );
				
				if ( !itsDetails.KeyIsValid( key ) )
				{
					poseven::throw_errno( ENOENT );
				}
				
				return itsDetails.GetChildNode( shared_from_this(), key );
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
				
				FSTreePtr node( itsDetails.GetChildNode( itsParent, key ) );
				
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
			virtual ~FSTree_Mappable();
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void Map( FSTreePtr tree );
			
			template < class FSTree_Type >
			void MapSingleton()
			{
				Map( GetSingleton< FSTree_Type >( shared_from_this() ) );
			}
			
			FSTreePtr Lookup_Mapping( const std::string& name ) const;
			
			virtual FSTreePtr Lookup_Regular( const std::string& name ) const = 0;
	};
	
	
	class FSTree_Virtual : public FSTree_Mappable
	{
		public:
			FSTreePtr Lookup_Regular( const std::string& /*name*/ ) const  { return FSNull(); }
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	template < class FSTree_Type >
	FSTreePtr Singleton_Factory( const FSTreePtr& parent, const std::string& name )
	{
		return GetSingleton< FSTree_Type >( parent );
	}
	
	template < class Details >
	class FSTree_Functional : public FSTree_Directory, public Details
	{
		private:
			typedef typename Details::Function Function;
			
			typedef std::map< std::string, Function > Mappings;
			
			Mappings itsMappings;
		
		public:
			FSTree_Functional()
			{
			}
			
			template < class Key >
			FSTree_Functional( const Key& key ) : Details( key )
			{
			}
			
			void Map( const std::string& name, Function f );
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	template < class Details >
	void FSTree_Functional< Details >::Map( const std::string& name, Function f )
	{
		itsMappings[ name ] = f;
	}
	
	template < class Details >
	FSTreePtr FSTree_Functional< Details >::Lookup_Child( const std::string& name ) const
	{
		Mappings::const_iterator it = itsMappings.find( name );
		
		if ( it == itsMappings.end() )
		{
			return FSNull();
		}
		
		const Function& f = it->second;
		
		return Details::Invoke( f, shared_from_this(), name );
	}
	
	template < class Details >
	void FSTree_Functional< Details >::IterateIntoCache( FSTreeCache& cache ) const
	{
		typedef Mappings::const_iterator Iter;
		
		for ( Iter it = itsMappings.begin();  it != itsMappings.end();  ++it )
		{
			const std::string& name = it->first;
			
			const Function& f = it->second;
			
			FSTreePtr tree( Details::Invoke( f, shared_from_this(), name ) );
			
			cache.push_back( FSNode( name, tree ) );
		}
	}
	
	
	class Singleton_Functional_Details
	{
		public:
			typedef FSTreePtr (*Function)( const FSTreePtr&, const std::string& );
			
			FSTreePtr Invoke( Function f, const FSTreePtr& parent, const std::string& name ) const
			{
				return f( parent, name );
			}
	};
	
	
	UInt32 DecodeHex32( const char* begin, const char* end );
	
}

#endif

