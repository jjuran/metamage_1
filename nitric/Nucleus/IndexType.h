// IndexType.h

/*
   The IndexType template creates a numeric type with the operations necessary
   for use as an index into a random-access data structure such as an array.
   
   In general, such types shouldn't be necessary -- the underlying integral type
   would serve just as well.  But sometimes ambiguities can arise if integral types
   have been used for non-integral purposes, and this template can be used to
   clarify the ambiguities.  (cf. FSVolumeRefNum/FSVolumeIndex)
   
   The specifics for an index type are given by a structure
   
   struct Specifics
     {
      typedef <integral-type> UnderlyingType;
      static const UnderlyingType defaultValue = <value>;
     };
*/

#ifndef NUCLEUS_INDEXTYPE_H
#define NUCLEUS_INDEXTYPE_H

namespace Nucleus
  {
   template < class Specifics >
   class IndexType
     {
      private:
         typedef typename Specifics::UnderlyingType UnderlyingType;
         static const UnderlyingType defaultValue = Specifics::defaultValue;
         
         UnderlyingType value;
         
      public:
         IndexType()                                                    : value( defaultValue )    {}
         IndexType( UnderlyingType theValue )                           : value( theValue )        {}
         
         operator UnderlyingType() const                                { return value; }

         static IndexType Make( UnderlyingType v )                      { return IndexType( v ); }
         UnderlyingType Get() const                                     { return value; }

         IndexType& operator++()                                        { value++; return *this; }
         IndexType& operator--()                                        { value--; return *this; }
         IndexType  operator++( int )                                   { IndexType old = *this; ++value; return old; }
         IndexType  operator--( int )                                   { IndexType old = *this; --value; return old; }

         IndexType& operator+=( UnderlyingType n )                      { value += n; return *this; }
         IndexType& operator-=( UnderlyingType n )                      { value -= n; return *this; }
         IndexType& operator*=( UnderlyingType n )                      { value *= n; return *this; }
         IndexType& operator/=( UnderlyingType n )                      { value /= n; return *this; }
         IndexType& operator%=( UnderlyingType n )                      { value %= n; return *this; }

         friend IndexType operator+( IndexType i, UnderlyingType n )    { return i += n; }
         friend IndexType operator-( IndexType i, UnderlyingType n )    { return i -= n; }
         friend IndexType operator*( IndexType i, UnderlyingType n )    { return i *= n; }
         friend IndexType operator/( IndexType i, UnderlyingType n )    { return i /= n; }
         friend IndexType operator%( IndexType i, UnderlyingType n )    { return i %= n; }

         friend IndexType operator+( UnderlyingType n, IndexType i )    { return i + n; }
         friend IndexType operator*( UnderlyingType n, IndexType i )    { return i * n; }
         
         friend bool operator==( IndexType a, IndexType b )             { return a.Get() == b.Get(); }
         friend bool operator!=( IndexType a, IndexType b )             { return a.Get() != b.Get(); }
     };
  }

#endif
