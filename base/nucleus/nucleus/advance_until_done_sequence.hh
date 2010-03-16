// nucleus/advance_until_done_sequence.hh
// --------------------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_ADVANCEUNTILDONESEQUENCE_HH
#define NUCLEUS_ADVANCEUNTILDONESEQUENCE_HH

#include <iterator>


namespace nucleus
{
	
	template < class Specifics >
	class advance_until_done_sequence : private Specifics
	{
		private: 
			// not implemented:
			advance_until_done_sequence& operator=( const advance_until_done_sequence& );
		
		public:
			typedef typename Specifics::size_type        size_type;
			typedef typename Specifics::difference_type  difference_type;
			typedef typename Specifics::value_type       value_type;
			typedef typename Specifics::key_type         key_type;
			
			typedef value_type& reference;
			typedef const value_type& const_reference;
			
			class const_iterator : private Specifics
			{
				friend class advance_until_done_sequence;
				
				public:
					typedef typename advance_until_done_sequence::size_type        size_type;
					typedef typename advance_until_done_sequence::difference_type  difference_type;
					typedef typename advance_until_done_sequence::value_type       value_type;
					typedef typename advance_until_done_sequence::key_type         key_type;
					typedef const value_type *pointer;
					typedef const value_type& reference;
					typedef std::forward_iterator_tag iterator_category;
				
				private:
					key_type its_key;
					
					void get_next_key()
					{
						its_key = Specifics::get_next_key( its_key );
					}
					
					const_iterator( const Specifics& b, key_type k ) : Specifics( b ),
					                                                   its_key  ( k )
					{
					}
					
				public:
					const_iterator()                  : its_key( Specifics::end_key() )  {}
					
					const_iterator& operator++()      { get_next_key();  return *this; }
					const_iterator operator++(int)    { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const       { return *Specifics::GetPointer( its_key ); }
					pointer operator->() const        { return  Specifics::GetPointer( its_key ); }
					
					friend bool operator==( const const_iterator& a,
					                        const const_iterator& b )    { return a.its_key == b.its_key; }
					
					friend bool operator!=( const const_iterator& a,
					                        const const_iterator& b )    { return !( a == b ); }
			};
			
			advance_until_done_sequence( const Specifics& base ) : Specifics( base )
			{
			}
			
			const_iterator begin() const            { return const_iterator( *this, Specifics::begin_key() ); }
			const_iterator end() const              { return const_iterator( *this, Specifics::end_key  () ); }
	};
	
}

#endif

