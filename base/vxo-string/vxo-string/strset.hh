/*
	strset.hh
	---------
*/

#ifndef VXOSTRING_STRSET_HH
#define VXOSTRING_STRSET_HH

// iota
#include "iota/class.hh"

// vxo
#include "vxo/container.hh"

// vxo-string
#include "vxo-string/string.hh"


namespace vxo
{
	
	class StrSet : public Container
	{
		NO_NEW_DELETE
		
		public:
			typedef plus::string Item, value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type&       reference;
			typedef value_type const& const_reference;
			
			static bool test( const Box& box );  // unimplemented
			
			StrSet() : Container( Box_set )
			{
			}
			
			Item* begin()  { unshare(); return (Item*) u.str.pointer; }
			Item* end()    { return begin() + size(); }
			
			Item const* begin() const  { return (Item const*) u.str.pointer; }
			Item const* end()   const  { return begin() + size(); }
			
			Item const* cbegin() const  { return begin(); }
			Item const* cend()   const  { return end();   }
			
			iterator find( const plus::string& item );
			iterator found( const plus::string& item );
			
			Item const* find( const plus::string& item ) const
			{
				return const_cast< StrSet* >( this )->find( item );
			}
			
			Item const* found( const plus::string& item ) const
			{
				return const_cast< StrSet* >( this )->found( item );
			}
			
			size_t count( const plus::string& item ) const
			{
				return found( item ) != NULL;
			}
			
			void insert( const plus::string& item );
			
			void erase( iterator loc )
			{
				erase_n( (Box*) loc, 1 );
			}
			
			bool erase( const plus::string& item )
			{
				if ( iterator loc = found( item ) )
				{
					erase( loc );
					
					return true;
				}
				
				return false;
			}
	};
	
}

#endif
