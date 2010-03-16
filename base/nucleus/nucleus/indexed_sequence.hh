// nucleus/indexed_sequence.hh
// ---------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_INDEXEDSEQUENCE_HH
#define NUCLEUS_INDEXEDSEQUENCE_HH

#include <iterator>

namespace nucleus
{
	
	template < class Specifics >
	class indexed_sequence_base
	{
		protected:
			// can only be destroyed (and therefore constructed) by a subclass
			~indexed_sequence_base()  {}
		
		public:
			typedef typename Specifics::size_type       size_type;
			
			class const_iterator
			{
				friend class indexed_sequence_base;
				
				public:
					typedef typename Specifics::size_type       size_type;
					typedef typename Specifics::difference_type difference_type;
					typedef typename Specifics::value_type      value_type;
					typedef typename Specifics::const_reference const_reference;
					typedef typename Specifics::const_pointer   const_pointer;
					
					typedef const_reference       reference;
					typedef const_pointer         pointer;
					
					typedef std::random_access_iterator_tag iterator_category;
				
				private:
					typedef typename Specifics::context_type context_type;
					
					context_type its_context;
					size_type position;
					
					const_reference get_reference() const
					{
						return Specifics::get_reference( its_context, position );
					}
					
					const_pointer get_pointer() const
					{
						return Specifics::get_pointer( its_context, position );
					}
					
					const_iterator( context_type context, size_type p )
					: 
						its_context( context ),
						position( p )
					{
					}
				
				public:
					const_iterator& operator++()                         { ++position;  return *this; }
					const_iterator  operator++(int)                      { const_iterator old = *this; operator++(); return old; }
					
					const_iterator operator+( difference_type d ) const  { return const_iterator( its_context, position + d ); }
					const_iterator operator-( difference_type d ) const  { return const_iterator( its_context, position - d ); }
					
					const_reference operator*()  const                   { return get_reference(); }
					const_pointer   operator->() const                   { return get_pointer(); }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
		protected:
			typedef typename Specifics::context_type context_type;
			
			const_iterator MakeConstIterator( context_type context, size_type position ) const
			{
				return const_iterator( context, position );
			}
	};
	
	// Forward declaration for CW
	template < class Specifics >  class indexed_sequence;
	
	template < class Specifics >
	class const_indexed_sequence : public indexed_sequence_base< Specifics >
	{
		friend class indexed_sequence< Specifics >;
		
		private:
			typedef typename Specifics::context_type  context_type;
			
			context_type its_context;
			
			// not implemented
			const_indexed_sequence& operator=( const const_indexed_sequence& );
		
		public:
			typedef typename Specifics::size_type        size_type;
			typedef typename Specifics::value_type       value_type;
			typedef typename Specifics::const_reference  const_reference;
			
			typedef typename indexed_sequence_base< Specifics >::const_iterator const_iterator;
			
			const_indexed_sequence( context_type context )
			: 
				its_context( context )
			{}
			
			size_type size() const  { return Specifics::Size( its_context ); }
		
			const_iterator begin() const  { return MakeConstIterator( its_context, 0 ); }
			const_iterator end()   const  { return MakeConstIterator( its_context, size() ); }
			
			const_reference operator[]( size_type i ) const
			{
				return *( begin() + i );  // zero-based
			}
			
			const_reference front() const  { return *begin();       }
			const_reference back()  const  { return *( end() - 1 ); }
	};
	
	template < class Specifics >
	class indexed_sequence : public indexed_sequence_base< Specifics >
	{
		private:
			typedef typename Specifics::context_type  context_type;
			
			context_type its_context;
			
			// not implemented
			indexed_sequence& operator=( const indexed_sequence& );
		
		public:
			typedef typename Specifics::size_type        size_type;
			typedef typename Specifics::value_type       value_type;
			typedef typename Specifics::const_reference  const_reference;
			typedef typename Specifics::reference        reference;
			
			typedef typename indexed_sequence_base< Specifics >::const_iterator const_iterator;
			
			class iterator
			{
				friend class indexed_sequence;
				
				public:
					typedef typename Specifics::size_type       size_type;
					typedef typename Specifics::difference_type difference_type;
					typedef typename Specifics::value_type      value_type;
					typedef typename Specifics::const_reference const_reference;
					typedef typename Specifics::const_pointer   const_pointer;
					
					typedef typename Specifics::reference       reference;
					typedef typename Specifics::pointer         pointer;
					
					typedef std::random_access_iterator_tag iterator_category;
					
					typedef typename indexed_sequence::const_iterator const_iterator;
					
				private:
					typedef typename Specifics::context_type context_type;
					
					context_type its_context;
					size_type position;
					
					const_reference get_reference() const
					{
						return Specifics::get_reference( its_context, position );
					}
					
					reference get_reference()
					{
						return Specifics::get_reference( its_context, position );
					}
					
					const_pointer get_pointer() const
					{
						return Specifics::get_pointer( its_context, position );
					}
					
					pointer get_pointer()
					{
						return Specifics::get_pointer( its_context, position );
					}
					
					iterator( context_type context, size_type p )
					: 
						its_context( context ),
						position( p )
					{
					}
				
				public:
					iterator& operator++()                         { ++position;  return *this; }
					iterator  operator++(int)                      { iterator old = *this; operator++(); return old; }
					
					iterator operator+( difference_type d ) const  { return iterator( its_context, position + d ); }
					iterator operator-( difference_type d ) const  { return iterator( its_context, position - d ); }
					
					const_reference operator*()  const             { return get_reference(); }
					reference       operator*()                    { return get_reference(); }
					const_pointer   operator->() const             { return get_pointer(); }
					pointer         operator->()                   { return get_pointer(); }
					
					operator const_iterator() const  { return const_iterator( static_cast< context_type >( its_context ), position ); }
					
					friend bool operator==( const iterator& a, const iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const iterator& a, const iterator& b )    { return !( a == b ); }
			};
			
			indexed_sequence( context_type context )
			: 
				its_context( context )
			{}
			
			size_type size() const  { return Specifics::Size( its_context ); }
			
			const_iterator begin() const  { return const_iterator( its_context, 0 );          }
			iterator       begin()        { return iterator      ( its_context, 0 );          }
			const_iterator end()   const  { return const_iterator( its_context, size() ); }
			iterator       end()          { return iterator      ( its_context, size() ); }
			
			value_type operator[]( size_type i ) const
			{
				return *( begin() + i );  // zero-based
			}
			
			const_reference front() const  { return *begin();       }
			reference       front()        { return *begin();       }
			const_reference back()  const  { return *( end() - 1 ); }
			reference       back()         { return *( end() - 1 ); }
			
			operator const_indexed_sequence< Specifics >() const  { return const_indexed_sequence< Specifics >( static_cast< context_type >( its_context ) ); }
	};
	
}

#endif

