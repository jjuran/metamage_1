/*	=======================
 *	IndexedValueContainer.h
 *	=======================
 */

#ifndef ITERATION_INDEXEDVALUECONTAINER_H
#define ITERATION_INDEXEDVALUECONTAINER_H

// Standard C++
#include <iterator>


namespace Nitrogen
{
	
	template < class Specifics >
	class IndexedValueContainer
	{
		private:
			typedef typename Specifics::ContainerState  State;
			
			State state;
			
			// not implemented
			IndexedValueContainer& operator=( const IndexedValueContainer& );
		
		public:
			typedef typename Specifics::size_type        size_type;
			typedef typename Specifics::value_type       value_type;
			
			class const_iterator
			{
				friend class IndexedValueContainer;
				
				public:
					typedef typename Specifics::size_type       size_type;
					typedef typename Specifics::difference_type difference_type;
					typedef typename Specifics::value_type      value_type;
					
					typedef std::random_access_iterator_tag iterator_category;
				
				private:
					typedef typename Specifics::IteratorState State;
					typedef typename Specifics::ContainerState ContainerState;
					
					State state;
					size_type position;
					
					value_type GetValue() const
					{
						return Specifics::GetValue( state, position );
					}
					
					const_iterator( ContainerState state, size_type p )
					: 
						state( state ),
						position( p )
					{
					}
				
				public:
					const_iterator& operator++()                         { ++position;  return *this; }
					const_iterator  operator++(int)                      { const_iterator old = *this; operator++(); return old; }
					
					const_iterator operator+( difference_type d ) const  { return const_iterator( state, position + d ); }
					const_iterator operator-( difference_type d ) const  { return const_iterator( state, position - d ); }
					
					value_type operator*()  const                        { return GetValue(); }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			IndexedValueContainer( State state )
			: 
				state( state )
			{}
			
			size_type size() const  { return Specifics::Size( state ); }
		
			const_iterator begin() const  { return const_iterator( state, 0 ); }
			const_iterator end()   const  { return const_iterator( state, size() ); }
			
			value_type operator[]( size_type i ) const
			{
				return *( begin() + i );  // zero-based
			}
			
			value_type front() const  { return *begin();       }
			value_type back()  const  { return *( end() - 1 ); }
	};
	
}

#endif

