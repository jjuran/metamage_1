/*	=========================
 *	indexed_value_sequence.hh
 *	=========================
 */

#ifndef NUCLEUS_INDEXEDVALUESEQUENCE_HH
#define NUCLEUS_INDEXEDVALUESEQUENCE_HH

// Standard C++
#include <iterator>


namespace nucleus
{
	
	template < class Specifics >
	class indexed_value_sequence
	{
		private:
			typedef typename Specifics::context_type context_type;
			
			context_type its_context;
			
			// not implemented
			indexed_value_sequence& operator=( const indexed_value_sequence& );
		
		public:
			typedef typename Specifics::size_type        size_type;
			typedef typename Specifics::value_type       value_type;
			
			class const_iterator
			{
				friend class indexed_value_sequence;
				
				public:
					typedef typename Specifics::size_type       size_type;
					typedef typename Specifics::difference_type difference_type;
					typedef typename Specifics::value_type      value_type;
					
					typedef value_type      &  reference;
					typedef value_type const&  const_reference;
					typedef value_type      *  pointer;
					typedef value_type const*  const_pointer;
					
					typedef std::random_access_iterator_tag iterator_category;
				
				private:
					typedef typename Specifics::context_type context_type;
					
					context_type its_context;
					size_type position;
					
					value_type get_value() const
					{
						return Specifics::get_value( its_context, position );
					}
					
					const_iterator( context_type context, size_type p )
					: 
						its_context( context ),
						position( p )
					{
					}
				
				public:
					const_iterator() : its_context(), position() {}
					
					const_iterator& operator++()                         { ++position;  return *this; }
					const_iterator  operator++(int)                      { const_iterator old = *this; operator++(); return old; }
					
					const_iterator operator+( difference_type d ) const  { return const_iterator( its_context, position + d ); }
					const_iterator operator-( difference_type d ) const  { return const_iterator( its_context, position - d ); }
					
					difference_type operator-( const_iterator it ) const  { return position - it.position; }
					
					value_type operator*()  const                        { return get_value(); }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			indexed_value_sequence( context_type context )
			: 
				its_context( context )
			{}
			
			size_type size() const  { return Specifics::size( its_context ); }
		
			const_iterator begin() const  { return const_iterator( its_context, 0 ); }
			const_iterator end()   const  { return const_iterator( its_context, size() ); }
			
			value_type operator[]( size_type i ) const
			{
				return *( begin() + i );  // zero-based
			}
			
			value_type front() const  { return *begin();       }
			value_type back()  const  { return *( end() - 1 ); }
	};
	
}

#endif

