// Nucleus/LinkedListContainer.h
// -----------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_LINKEDLISTCONTAINER_H
#define NUCLEUS_LINKEDLISTCONTAINER_H

#include <iterator>


namespace Nucleus
{
	
	template < class Specifics >
	class LinkedListContainer: private Specifics
	{
		private: 
			// not implemented:
			LinkedListContainer& operator=( const LinkedListContainer& );
		
		public:
			typedef typename Specifics::value_type value_type;
			typedef typename Specifics::size_type size_type;
			typedef typename Specifics::difference_type difference_type;
			
			typedef value_type& reference;
			typedef const value_type& const_reference;
			
			class const_iterator: private Specifics
			{
				friend class LinkedListContainer;
 				
				public:
					typedef typename LinkedListContainer::size_type size_type;
					typedef typename LinkedListContainer::difference_type difference_type;
					typedef typename LinkedListContainer::value_type value_type;
					typedef const value_type *pointer;
					typedef const value_type& reference;
					typedef std::forward_iterator_tag iterator_category;
				
				private:
					value_type value;
					
					void GetNextValue()
					{
						value = Specifics::GetNextValue( value );
					}
					
					const_iterator( const Specifics& b, value_type v ) : Specifics( b ), value( v )
					{
					}
				
				public:
					const_iterator()                          : value( Specifics::end_value() )   {}
					
					const_iterator& operator++()              { GetNextValue();  return *this; }
					const_iterator operator++(int)            { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const               { return value; }
					pointer operator->() const                { return &value; }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.value == b.value; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			LinkedListContainer( const Specifics& base ) : Specifics( base )
			{
			}
			
			const_iterator begin() const                    { return const_iterator( *this, Specifics::begin_value() ); }
			const_iterator end() const                      { return const_iterator( *this, NULL ); }
	};
	
}

#endif

