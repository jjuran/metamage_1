// IndexUntilFailureContainer.h

#ifndef NITROGEN_INDEXUNTILFAILURECONTAINER_H
#define NITROGEN_INDEXUNTILFAILURECONTAINER_H

#include <iterator>

namespace Nitrogen
  {
   template < class Specifics >
   class IndexUntilFailureContainer: private Specifics
     {
      private: 
         // not implemented:
            IndexUntilFailureContainer& operator=( const IndexUntilFailureContainer& );

      public:
         typedef typename Specifics::value_type value_type;
         typedef typename Specifics::size_type size_type;
         typedef typename Specifics::difference_type difference_type;

         typedef value_type& reference;
         typedef const value_type& const_reference;
         
         class const_iterator: private Specifics
           {
            friend class IndexUntilFailureContainer;
 
             public:
               typedef IndexUntilFailureContainer::difference_type difference_type;
               typedef IndexUntilFailureContainer::value_type value_type;
               typedef const value_type *pointer;
               typedef const value_type& reference;
               typedef std::forward_iterator_tag iterator_category;
           
            private:
               size_type position;
               value_type value;
               
               void GetValue()
                 {
                  try
                    {
                     value = Specifics::GetValue( position );
                    }
                  catch ( const typename Specifics::EndOfEnumeration& )
                    {
                     position = Specifics::end_position();
                    }
                 }
               
               const_iterator( const Specifics& b, size_type p )
                 : Specifics( b ),
                   position( p )
                 {
                  GetValue();
                 }
               
            public:
               const_iterator()                          : position( Specifics::end_position() )   {}
               
               const_iterator& operator++()              { ++position; GetValue(); return *this; }
               const_iterator operator++(int)            { const_iterator old = *this; operator++(); return old; }
               
               reference operator*() const               { return value; }
               pointer operator->() const                { return &value; }
               
               friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
               friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
           };
      
         IndexUntilFailureContainer( const Specifics& base )
           : Specifics( base )
           {}
         
         const_iterator begin() const                    { return const_iterator( *this, Specifics::begin_position() ); }
         const_iterator end() const                      { return const_iterator( *this, Specifics::end_position() ); }
     };
  }
#endif
