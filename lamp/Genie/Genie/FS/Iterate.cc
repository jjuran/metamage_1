/*
	Iterate.cc
	----------
*/

#include "Genie/FS/Iterate.hh"

// vfs
#include "vfs/dir_contents_impl.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/inode.hh"
#include "Genie/FS/listdir.hh"
#include "Genie/FS/parent_inode.hh"


namespace Genie
{
	
	typedef boost::intrusive_ptr< const vfs::dir_contents > FSTreeCachePtr;
	
	class FSIterator_Cache : public FSIterator
	{
		private:
			FSTreeCachePtr  contents;
			std::size_t     nextOffset;
		
		public:
			FSIterator_Cache( const FSTreeCachePtr& cache ) : contents( cache ), nextOffset( 0 )  {}
			
			vfs::dir_entry Get() const
			{
				const vfs::dir_contents_impl* impl = static_cast< const vfs::dir_contents_impl* >( contents.get() );
				
				return nextOffset < impl->size() ? impl->at( nextOffset )
				                                 : vfs::dir_entry();
			}
			
			void Advance()  { ++nextOffset; }
			
			void Rewind()  { nextOffset = 0; }
			
			void Seek( off_t index )  { nextOffset = index; }
			
			off_t Tell() const  { return nextOffset; }
	};
	
	
	FSIterator::~FSIterator()
	{
	}
	
	FSIteratorPtr Iterate( const FSTreePtr& dir )
	{
		vfs::dir_contents_impl cache;
		
		cache.push_back( vfs::dir_entry( inode       ( dir.get() ), "."  ) );
		cache.push_back( vfs::dir_entry( parent_inode( dir.get() ), ".." ) );
		
		listdir( dir.get(), cache );
		
		vfs::dir_contents_impl* newCache = new vfs::dir_contents_impl();
		
		FSTreeCachePtr cachePtr( newCache );
		
		swap( cache, *newCache );
		
		return FSIteratorPtr( new FSIterator_Cache( cachePtr ) );
	}
	
}

