/*
	strpool.hh
	----------
*/

#ifndef VXS_STRPOOL_HH
#define VXS_STRPOOL_HH

// vxo
#include "vxo/ptrvec.hh"

// vxs
#include "vxs/string.hh"


namespace vxo
{
	
	const size_t max_lane_size = 64 * 2014;
	
	class Lane
	{
		private:
			size_t its_size;
			size_t its_mark;
			
			char its_data[ max_lane_size ];
		
		public:
			explicit Lane( size_t size ) : its_size( size ), its_mark()
			{
			}
			
			size_t bytes_free() const  { return its_size - its_mark; }
			
			const char* append( const char* p, size_t n );
	};
	
	class StrPool
	{
		private:
			PtrVec_< Lane > its_lanes;
			
			PtrVec_< const char > its_bone;
			
			// non-copyable
			StrPool           ( const StrPool& );
			StrPool& operator=( const StrPool& );
		
		public:
			typedef const char* Item;
			typedef const char* value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type& reference;
			typedef value_type  const_reference;
			
			StrPool() {}
			
			~StrPool();
			
			size_t size() const  { return its_bone.size(); }
			
			Item const* begin() const  { return its_bone.begin(); }
			Item const* end()   const  { return begin() + size(); }
			
			Item const* cbegin() const  { return begin(); }
			Item const* cend()   const  { return end();   }
			
			Item operator[]( const plus::string& key );
	};
	
}

#endif
