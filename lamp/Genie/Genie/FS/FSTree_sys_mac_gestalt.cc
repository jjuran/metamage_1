/*	=========================
 *	FSTree_sys_mac_gestalt.cc
 *	=========================
 */

#include "Genie/FS/FSTree_sys_mac_gestalt.hh"

// Nitrogen
#include "Nitrogen/Gestalt.h"
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/Name_OSType.hh"


namespace Nitrogen
{
	
	struct Gestalt_Record
	{
		Nitrogen::OSType  selector;
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
			
			typedef Nitrogen::OSType value_type;
			
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
	
	
	struct Gestalt_KeyName_Traits : OSType_KeyName_Traits
	{
		static std::string NameFromKey( const Key& key );
	};
	
	struct sys_mac_gestalt_Details : public Gestalt_KeyName_Traits
	{
		typedef Nitrogen::GestaltTable_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::GestaltTable(); }
		
		static Key KeyFromValue( Sequence::const_reference ref )  { return ref; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_gestalt_Details > FSTree_sys_mac_gestalt;
	
	
	std::string Gestalt_KeyName_Traits::NameFromKey( const Key& key )
	{
		std::string result = OSType_KeyName_Traits::NameFromKey( key );
		
		if ( !sys_mac_gestalt_Details::KeyIsValid( key ) )
		{
			result = "." + result;
		}
		
		return result;
	}
	
	
	bool sys_mac_gestalt_Details::KeyIsValid( const Key& key )
	{
		try
		{
			N::Gestalt( N::GestaltSelector( key ) );
		}
		catch ( ... )
		{
			return false;
		}
		
		return true;
	}
	
	FSTreePtr sys_mac_gestalt_Details::GetChildNode( const FSTreePtr&    parent,
		                                             const std::string&  name,
		                                             const Key&          key )
	{
		return FSTreePtr( new FSTree_Premapped( parent, name ) );
	}
	
	FSTreePtr New_FSTree_sys_mac_gestalt( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_sys_mac_gestalt( parent, name ) );
	}
	
}

