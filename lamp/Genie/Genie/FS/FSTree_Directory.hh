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

// Iota
#include "iota/decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTree_Null.hh"


namespace Genie
{
	
	struct Int_KeyName_Traits
	{
		typedef int Key;
		
		static std::string NameFromKey( const Key& key )
		{
			return iota::inscribe_decimal( key );
		}
		
		static Key KeyFromName( const std::string& name )
		{
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
			
			~FSTree_Directory();
			
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
				
				return itsDetails.GetChildNode( Self(), name, key );
			}
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	template < class Details >
	class IteratorConverter
	{
		private:
			Details itsDetails;
		
		public:
			IteratorConverter( const Details& details ) : itsDetails( details )
			{
			}
			
			FSNode operator()( const typename Details::Sequence::value_type& value ) const
			{
				typename Details::Key key = itsDetails.KeyFromValue( value );
				
				std::string name = itsDetails.NameFromKey( key );
				ino_t inode = 0;
				
				return FSNode( inode, name );
			}
	};
	
	template < class Details >
	void FSTree_Sequence< Details >::IterateIntoCache( FSTreeCache& cache ) const
	{
		IteratorConverter< Details > converter( itsDetails );
		
		typename Details::Sequence sequence = itsDetails.ItemSequence();
		
		std::transform( sequence.begin(),
		                sequence.end(),
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
			
			~FSTree_Mappable();
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void Map( FSTreePtr tree );
			
			FSTreePtr Lookup_Mapping( const std::string& name ) const;
			
			virtual FSTreePtr Lookup_Regular( const std::string& name ) const = 0;
	};
	
	
	class FSTree_Premapped : public FSTree_Directory
	{
		public:
			typedef FSTreePtr (*Function)( const FSTreePtr&, const std::string& );
			
			struct Mapping
			{
				const char*  name;
				Function     f;
				bool         needs_check;
			};
		
		private:
			typedef std::map< std::string, const Mapping* > Mappings;
			
			typedef void (*Destructor)( const FSTree* );
			
			Destructor  itsDestructor;
			Mappings    itsMappings;
		
		public:
			FSTree_Premapped( const FSTreePtr&    parent,
			                  const std::string&  name,
			                  Destructor          dtor = NULL ) : FSTree_Directory( parent, name ),
			                                                      itsDestructor( dtor )
			{
			}
			
			~FSTree_Premapped();
			
			void Map( const Mapping& mapping );
			
			void AddMappings( const Mapping* begin );
			void AddMappings( const Mapping* begin, const Mapping* end );
			
			void Delete() const;
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	template < class FSTree_Type >
	FSTreePtr Basic_Factory( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_Type( parent, name ) );
	}
	
	FSTreePtr Premapped_Factory( const FSTreePtr&                   parent,
	                             const std::string&                 name,
	                             const FSTree_Premapped::Mapping    mappings[],
	                             void                             (*dtor)(const FSTree*) = NULL );
	
	template < const FSTree_Premapped::Mapping mappings[] >
	inline FSTreePtr Premapped_Factory( const FSTreePtr& parent, const std::string& name )
	{
		return Premapped_Factory( parent, name, mappings );
	}
	
	template < const FSTree_Premapped::Mapping mappings[], void (*dtor)(const FSTree*) >
	inline FSTreePtr Premapped_Factory( const FSTreePtr& parent, const std::string& name )
	{
		return Premapped_Factory( parent, name, mappings, dtor );
	}
	
}

#endif

