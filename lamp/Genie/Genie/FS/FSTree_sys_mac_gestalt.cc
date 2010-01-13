/*	=========================
 *	FSTree_sys_mac_gestalt.cc
 *	=========================
 */

#include "Genie/FS/FSTree_sys_mac_gestalt.hh"

// iota
#include "iota/hexidecimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/Name_OSType.hh"
#include "Genie/Utilities/canonical_32_bit_hex.hh"


namespace Nitrogen
{
	
	struct Gestalt_Record
	{
		Gestalt_Selector  selector;
		UInt32            zero;
	};
	
	typedef Gestalt_Record const *const *Gestalt_Handle;
	
	class GestaltTable_Container
	{
		private:
			Gestalt_Handle handle;
			
			// not implemented:
			GestaltTable_Container& operator=( const GestaltTable_Container& );
		
		public:
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			typedef Gestalt_Selector value_type;
			
			typedef const value_type &reference;
			typedef const value_type &const_reference;
			
			typedef const value_type *pointer;
			typedef const value_type *const_pointer;
			
			class const_iterator
			{
				friend class GestaltTable_Container;
				
				public:
					typedef GestaltTable_Container::size_type  size_type;
					typedef GestaltTable_Container::reference  reference;
					typedef GestaltTable_Container::pointer    pointer;
					
					typedef std::forward_iterator_tag iterator_category;
					
				private:
					Gestalt_Handle  handle;
					size_type       position;
					
					const_reference GetReference() const
					{
						return handle[0][ position ].selector;
					}
					
					void Advance()
					{
						++position;
						
						if ( GetReference() == 0x7fffffff )
						{
							position = 0xffff;
						}
					}
					
					const_iterator( Gestalt_Handle h, size_type pos ) : handle( h ), position( pos )
					{
					}
					
				public:
					const_iterator& operator++()      { Advance();  return *this; }
					const_iterator operator++(int)    { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const       { return GetReference(); }
					pointer operator->() const        { return &GetReference(); }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			GestaltTable_Container( Gestalt_Handle h ) : handle( h )
			{
			}
			
			const_iterator begin() const              { return const_iterator( handle,      0 ); }
			const_iterator end() const                { return const_iterator( handle, 0xffff ); }
	};
	
	static GestaltTable_Container GestaltTable()
	{
		UInt32 tabl = Nitrogen::Gestalt( Nitrogen::GestaltSelector( 'tabl' ) );
		
		Gestalt_Handle h = reinterpret_cast< Gestalt_Handle >( tabl );
		
		if ( h == NULL  ||  *h == NULL )
		{
			ThrowOSStatus( nilHandleErr );
		}
		
		return GestaltTable_Container( h );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static bool is_valid_Gestalt_Selector( N::Gestalt_Selector selector )
	{
		try
		{
			N::Gestalt( selector );
		}
		catch ( ... )
		{
			return false;
		}
		
		return true;
	}
	
	static inline bool is_valid_Gestalt_Selector_name( const std::string& name )
	{
		if ( !canonical_32_bit_hex::applies( name ) )
		{
			return false;
		}
		
		const unsigned decoded = iota::decode_32_bit_hex( name.c_str() );
		
		return is_valid_Gestalt_Selector( N::Gestalt_Selector( decoded ) );
	}
	
	static FSTreePtr gestalt_lookup( const FSTreePtr& parent, const std::string& name )
	{
		if ( !is_valid_Gestalt_Selector_name( name ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return seize_ptr( new FSTree_Premapped( parent, name ) );
	}
	
	class gestalt_IteratorConverter
	{
		public:
			FSNode operator()( N::Gestalt_Selector selector ) const
			{
				const bool valid = is_valid_Gestalt_Selector( selector );
				
				const ino_t inode = 0;
				
				std::string name;
				
				name.resize( !valid + 8 );
				
				if ( !valid )
				{
					name[0] = '.';
				}
				
				iota::encode_32_bit_hex( (unsigned) selector, &name[ !valid ] );
				
				return FSNode( inode, name );
			}
	};
	
	static void gestalt_iterate( FSTreeCache& cache )
	{
		gestalt_IteratorConverter converter;
		
		N::GestaltTable_Container sequence = N::GestaltTable();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	FSTreePtr New_FSTree_sys_mac_gestalt( const FSTreePtr& parent, const std::string& name )
	{
		return new_basic_directory( parent, name, gestalt_lookup, gestalt_iterate );
	}
	
}

