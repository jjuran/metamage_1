// IndexedContainer.h

#ifndef NITROGEN_INDEXEDCONTAINER_H
#define NITROGEN_INDEXEDCONTAINER_H

#include <iterator>

namespace Nitrogen
{
	
	template < class Specifics >
	class IndexedContainer_Base
	{
		protected:
			// can only be destroyed (and therefore constructed) by a subclass
			~IndexedContainer_Base()  {}
		
		public:
			typedef typename Specifics::size_type       size_type;
			
			class const_iterator
			{
				friend class IndexedContainer_Base;
				
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
					typedef typename Specifics::ConstIteratorState State;
					typedef typename Specifics::ConstContainerState ConstContainerState;
					
					State state;
					size_type position;
					
					const_reference GetReference() const
					{
						return Specifics::GetReference( state, position );
					}
					
					const_pointer GetPointer() const
					{
						return Specifics::GetPointer( state, position );
					}
					
					const_iterator( ConstContainerState state, size_type p )
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
					
					const_reference operator*()  const                   { return GetReference(); }
					const_pointer   operator->() const                   { return GetPointer(); }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
		protected:
			typedef typename Specifics::ConstContainerState ConstContainerState;
			
			const_iterator MakeConstIterator( ConstContainerState state, size_type position ) const
			{
				return const_iterator( state, position );
			}
	};
	
	// Forward declaration for CW
	template < class Specifics >  class IndexedContainer;
	
	template < class Specifics >
	class ConstIndexedContainer : public IndexedContainer_Base< Specifics >
	{
		friend class IndexedContainer< Specifics >;
		
		private:
			typedef typename Specifics::ConstContainerState  State;
			
			State state;
			
			// not implemented
			ConstIndexedContainer& operator=( const ConstIndexedContainer& );
		
		public:
			typedef typename Specifics::size_type        size_type;
			typedef typename Specifics::value_type       value_type;
			typedef typename Specifics::const_reference  const_reference;
			
			typedef typename IndexedContainer_Base< Specifics >::const_iterator const_iterator;
			
			ConstIndexedContainer( State state )
			: 
				state( state )
			{}
			
			size_type size() const  { return Specifics::Size( state ); }
		
			const_iterator begin() const  { return MakeConstIterator( state, 0 ); }
			const_iterator end()   const  { return MakeConstIterator( state, size() ); }
			
			const_reference operator[]( size_type i ) const
			{
				return *( begin() + i );  // zero-based
			}
			
			const_reference front() const  { return *begin();       }
			const_reference back()  const  { return *( end() - 1 ); }
	};
	
	template < class Specifics >
	class IndexedContainer : public IndexedContainer_Base< Specifics >
	{
		private:
			typedef typename Specifics::ContainerState       State;
			typedef typename Specifics::ConstContainerState  ConstContainerState;
			
			State state;
			
			// not implemented
			IndexedContainer& operator=( const IndexedContainer& );
		
		public:
			typedef typename Specifics::size_type        size_type;
			typedef typename Specifics::value_type       value_type;
			typedef typename Specifics::const_reference  const_reference;
			typedef typename Specifics::reference        reference;
			
			typedef typename IndexedContainer_Base< Specifics >::const_iterator const_iterator;
			
			class iterator
			{
				friend class IndexedContainer;
				
				public:
					typedef typename Specifics::size_type       size_type;
					typedef typename Specifics::difference_type difference_type;
					typedef typename Specifics::value_type      value_type;
					typedef typename Specifics::const_reference const_reference;
					typedef typename Specifics::const_pointer   const_pointer;
					
					typedef typename Specifics::reference       reference;
					typedef typename Specifics::pointer         pointer;
					
					typedef std::random_access_iterator_tag iterator_category;
					
					typedef typename IndexedContainer::const_iterator const_iterator;
					
				private:
					typedef typename Specifics::IteratorState State;
					typedef typename Specifics::ContainerState ContainerState;
					
					State state;
					size_type position;
					
					const_reference GetReference() const
					{
						return Specifics::GetReference( state, position );
					}
					
					reference GetReference()
					{
						return Specifics::GetReference( state, position );
					}
					
					const_pointer GetPointer() const
					{
						return Specifics::GetPointer( state, position );
					}
					
					pointer GetPointer()
					{
						return Specifics::GetPointer( state, position );
					}
					
					iterator( ContainerState state, size_type p )
					: 
						state( state ),
						position( p )
					{
					}
				
				public:
					iterator& operator++()                         { ++position;  return *this; }
					iterator  operator++(int)                      { iterator old = *this; operator++(); return old; }
					
					iterator operator+( difference_type d ) const  { return iterator( state, position + d ); }
					iterator operator-( difference_type d ) const  { return iterator( state, position - d ); }
					
					const_reference operator*()  const             { return GetReference(); }
					reference       operator*()                    { return GetReference(); }
					const_pointer   operator->() const             { return GetPointer(); }
					pointer         operator->()                   { return GetPointer(); }
					
					operator const_iterator() const  { return const_iterator( static_cast< ConstContainerState >( state ), position ); }
					
					friend bool operator==( const iterator& a, const iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const iterator& a, const iterator& b )    { return !( a == b ); }
			};
			
			IndexedContainer( State state )
			: 
				state( state )
			{}
			
			size_type size() const  { return Specifics::Size( state ); }
			
			const_iterator begin() const  { return const_iterator( state, 0 );          }
			iterator       begin()        { return iterator      ( state, 0 );          }
			const_iterator end()   const  { return const_iterator( state, size() ); }
			iterator       end()          { return iterator      ( state, size() ); }
			
			value_type operator[]( size_type i ) const
			{
				return *( begin() + i );  // zero-based
			}
			
			const_reference front() const  { return *begin();       }
			reference       front()        { return *begin();       }
			const_reference back()  const  { return *( end() - 1 ); }
			reference       back()         { return *( end() - 1 ); }
			
			operator ConstIndexedContainer< Specifics >() const  { return ConstIndexedContainer< Specifics >( static_cast< ConstContainerState >( state ) ); }
	};
	
}

#endif

