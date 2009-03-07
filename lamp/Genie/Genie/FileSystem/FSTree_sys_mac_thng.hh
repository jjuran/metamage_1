/*	======================
 *	FSTree_sys_mac_thng.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACTHNG_HH
#define GENIE_FILESYSTEM_FSTREESYSMACTHNG_HH

// Nitrogen
#include "Nitrogen/Components.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/Name_OSType.hh"


namespace Nitrogen
{
	
	const ComponentDescription& AnyComponentDescription();
	
	inline Component FindNextComponent( Component                    component   = NULL,
	                                    const ComponentDescription&  description = AnyComponentDescription() )
	{
		ComponentDescription desc_copy = description;
		
		return ::FindNextComponent( component, &desc_copy );
	}
	
	
	class Component_Container
	{
		private:
			// not implemented:
			Component_Container& operator=( const Component_Container& );
		
		public:
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			typedef Component value_type;
			
			typedef const value_type &reference;
			typedef const value_type &const_reference;
			
			typedef const value_type *pointer;
			typedef const value_type *const_pointer;
			
			class const_iterator
			{
				friend class Component_Container;
				
				public:
					typedef Component_Container::size_type  size_type;
					typedef Component_Container::reference  reference;
					typedef Component_Container::pointer    pointer;
					
					typedef std::forward_iterator_tag iterator_category;
					
				private:
					Component component;
					
					const_reference GetReference() const
					{
						return component;
					}
					
					void Advance()
					{
						component = FindNextComponent( component );
					}
					
					const_iterator( Component c = NULL ) : component( c )
					{
					}
					
				public:
					const_iterator& operator++()      { Advance();  return *this; }
					const_iterator operator++(int)    { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const       { return GetReference(); }
					pointer operator->() const        { return &GetReference(); }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.component == b.component; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			const_iterator begin() const              { return const_iterator( FindNextComponent() ); }
			const_iterator end() const                { return const_iterator(); }
	};
	
	inline Component_Container Components()
	{
		return Component_Container();
	}
	
}

namespace Genie
{
	
	struct sys_mac_thng_Details : public Pointer_KeyName_Traits< Component >
	{
		typedef Nitrogen::Component_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::Components(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_thng_Details > FSTree_sys_mac_thng;
	
}

#endif

