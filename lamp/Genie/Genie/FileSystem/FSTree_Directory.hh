/*	===================
 *	FSTree_Directory.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH
#define GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/FileSystem/FSTree_Null.hh"
#include "Genie/FileSystem/FSTree_Singleton.hh"


namespace Genie
{
	
	typedef std::vector< FSNode > FSTreeCache;
	
	
	class FSTree_Directory : public FSTree
	{
		public:
			virtual ~FSTree_Directory();
			
			bool IsFile     () const  { return false; }
			bool IsDirectory() const  { return true;  }
			
			virtual FSTreePtr Parent() const = 0;
			
			mode_t FileTypeMode() const  { return S_IFDIR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR | S_IXUSR; }
			
			FSTreePtr Lookup( const std::string& name ) const;
			
			virtual FSTreePtr Lookup_Child( const std::string& name ) const = 0;
			
			FSIteratorPtr Iterate() const;
			
			virtual void IterateIntoCache( FSTreeCache& cache ) const = 0;
	};
	
	
	template < class Details >
	class FSTree_Special : public FSTree_Directory
	{
		private:
			Details itsDetails;
		
		public:
			FSTree_Special() : itsDetails()  {}
			
			FSTree_Special( const Details& details ) : itsDetails( details )  {}
			
			static std::string OnlyName()  { return Details::Name(); }
			
			std::string Name() const  { return OnlyName(); }
			
			FSTreePtr Parent() const  { return itsDetails.Parent(); }
			
			FSTreePtr Lookup_Child( const std::string& name ) const  { return itsDetails.Lookup( name ); }
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	template < class Details >
	void FSTree_Special< Details >::IterateIntoCache( FSTreeCache& cache ) const
	{
		std::transform( itsDetails.ItemSequence().begin(),
		                itsDetails.ItemSequence().end(),
		                std::back_inserter( cache ),
		                itsDetails );
	}
	
	template < class Details > struct FSTree_Special_Unique;
	
	template < class Details >
	struct UniqueDetails : public Details
	{
		FSTreePtr Parent() const  { return FSRoot(); }
	};
	
	template < class Details >
	struct FSTree_Special_Unique : public FSTree_Special< UniqueDetails< Details > >
	{
	};
	
	
	class FSTree_Mappable : public FSTree_Directory
	{
		protected:
			typedef std::map< std::string, FSTreePtr > Mappings;
			
			Mappings mappings;
		
		public:
			virtual ~FSTree_Mappable();
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void Map( const std::string& name, FSTreePtr tree );
			
			template < class FSTree_Type >
			void MapSingleton()
			{
				Map( FSTree_Type::OnlyName(), FSTreePtr( GetSingleton< FSTree_Type >() ) );
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
	
}

#endif

