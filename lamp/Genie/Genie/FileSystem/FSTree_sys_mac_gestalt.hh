/*	=========================
 *	FSTree_sys_mac_gestalt.hh
 *	=========================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACGESTALT_HH
#define GENIE_FILESYSTEM_FSTREESYSMACGESTALT_HH

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


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
	
	GestaltTable_Container GestaltTable();
	
}

namespace Genie
{
	
	struct OSType_KeyName_Traits
	{
		typedef Nitrogen::OSType Key;
		
		static std::string NameFromKey( const Key& key )
		{
			return Nucleus::Convert< std::string >( key );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			return Nucleus::Convert< Nitrogen::OSType >( name );
		}
	};
	
	struct Gestalt_KeyName_Traits : OSType_KeyName_Traits
	{
	};
	
	struct sys_mac_gestalt_Details : public Gestalt_KeyName_Traits
	{
		typedef Nitrogen::GestaltTable_Container Sequence;
		
		static const Sequence& ItemSequence()  { return Nitrogen::GestaltTable(); }
		
		static Key KeyFromValue( Sequence::const_reference ref )  { return ref; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_gestalt_Details > FSTree_sys_mac_gestalt;
	
}

#endif

