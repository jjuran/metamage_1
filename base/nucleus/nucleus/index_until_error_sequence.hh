// nucleus/index_until_error_sequence.hh
// -------------------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_INDEXUNTILERRORSEQUENCE_HH
#define NUCLEUS_INDEXUNTILERRORSEQUENCE_HH

// Standard C++
#include <iterator>


namespace nucleus
{
	
	template < class Specifics >
	class index_until_error_sequence : private Specifics
	{
		private: 
			// not implemented:
			index_until_error_sequence& operator=( const index_until_error_sequence& );
		
		public:
			typedef typename Specifics::value_type       value_type;
			typedef typename Specifics::size_type        size_type;
			typedef typename Specifics::difference_type  difference_type;
			
			typedef value_type&        reference;
			typedef const value_type&  const_reference;
			
			class const_iterator : private Specifics
			{
				friend class index_until_error_sequence;
				
				public:
					typedef typename index_until_error_sequence::difference_type  difference_type;
					typedef typename index_until_error_sequence::value_type       value_type;
					typedef const value_type *pointer;
					typedef const value_type& reference;
					typedef std::forward_iterator_tag iterator_category;
				
				private:
					size_type position;
					value_type value;
					
					void get_value()
					{
						try
						{
							value = Specifics::get_value( position );
						}
						catch ( const typename Specifics::end_of_enumeration& end )
						{
							if ( !Specifics::exception_is_end_of_enumeration( end ) )  throw;
							
							position = Specifics::end_position();
						}
					}
					
					const_iterator( const Specifics& b, size_type p )
					:
						Specifics( b ),
						position( p )
					{
						get_value();
					}
				
				public:
					const_iterator()                          : position( Specifics::end_position() )   {}
					
					const_iterator& operator++()              { ++position; get_value(); return *this; }
					const_iterator operator++(int)            { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const               { return value; }
					pointer operator->() const                { return &value; }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			index_until_error_sequence( const Specifics& base )
			:
				Specifics( base )
			{
			}
			
			const_iterator begin() const                    { return const_iterator( *this, Specifics::begin_position() ); }
			const_iterator end() const                      { return const_iterator( *this, Specifics::end_position() ); }
		};
	
}

#endif

