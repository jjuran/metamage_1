// AEDescListItemC.h

#include "Nitrogen/AEDataModel.h"
#include "Nitrogen/Pseudoreference.h"


namespace Nitrogen {
	
	class ConstAEDescListItem_Details
	{
		private:
			const AEDescList& list;
			std::size_t index;
			
		public:
			typedef Owned< AEDesc > Value;
			typedef Value           GetResult;
			
			ConstAEDescListItem_Details( const AEDescList& list, std::size_t index ) 
			: 
				list( list ), 
				index( index )  {}
			
			GetResult Get() const  { return AEGetNthDesc( list, index ); }
	};
	
	typedef ConstPseudoreference< ConstAEDescListItem_Details > ConstAEDescListItem;
	
	class ConstAEDescListItem_Container
	{
		public:
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			class const_iterator
			{
				friend class ConstAEDescListItem_Container;
				
				public:
					typedef ConstAEDescListItem_Container::size_type size_type;
					typedef ConstAEDescListItem_Container::difference_type difference_type;
					typedef ConstAEDescListItem value_type;
					typedef ConstAEDescListItem::ConstPointer pointer;
					typedef value_type reference;
					typedef std::random_access_iterator_tag iterator_category;
					
				private:
					const AEDescList& list;
					size_type position;
					
					value_type GetValue() const
					{
						return value_type( list, position );
					}
					
					const_iterator( const AEDescList& list, size_type position )
					: 
						list( list ),
						position( position )
					{
					}
				
				public:
					const_iterator& operator++()              { ++position;  return *this; }
					const_iterator operator++(int)            { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const               { return GetValue(); }
					//pointer operator->() const                { return &value; }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			ConstAEDescListItem_Container( const AEDescList& list )
			: 
				list( list )
			{}
			
			const_iterator begin() const                    { return const_iterator( list, 1 ); }
			const_iterator end() const                      { return const_iterator( list, size() + 1 ); }
			
			size_type size() const  { return AECountItems( list ); }
			
		private:
			const AEDescList& list;
	};
	
	inline ConstAEDescListItem_Container AEDescList_Items( const AEDescList& list )
	{
		return ConstAEDescListItem_Container( list );
	}
	
	class AEDescListItem_Details
	{
		private:
			Owned< AEDescList >& list;
			std::size_t index;
			
		public:
			typedef Owned< AEDesc > Value;
			typedef Value           GetResult;
			typedef const AEDesc&   SetParameter;
			
			AEDescListItem_Details( Owned< AEDescList >& list, std::size_t index ) : list( list ), index( index )  {}
			
			GetResult Get() const  { return AEGetNthDesc( list, index ); }
			void Set( SetParameter param ) const  { AEPutDesc( list, index, param ); }
	};
	
	typedef Pseudoreference< AEDescListItem_Details > AEDescListItem;
	
	class AEDescListItem_Container
	{
		public:
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			class iterator;
			
			class const_iterator
			{
				friend class iterator;
				
				public:
					typedef AEDescListItem_Container::size_type size_type;
					typedef AEDescListItem_Container::difference_type difference_type;
					typedef ConstAEDescListItem value_type;
					typedef value_type::ConstPointer pointer;
					typedef value_type reference;
					typedef std::random_access_iterator_tag iterator_category;
					
				private:
					Owned< AEDescList >& list;
					size_type position;
					
					value_type GetValue() const
					{
						return value_type( ConstAEDescListItem_Details( list, position ) );
					}
					
					const_iterator( Owned< AEDescList >& list, size_type position )
					: 
						list( list ),
						position( position )
					{
					}
				
				public:
					const_iterator& operator++()              { ++position;  return *this; }
					const_iterator operator++(int)            { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const               { return GetValue(); }
					//pointer operator->() const                { return &value; }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			class iterator
			{
				friend class AEDescListItem_Container;
				
				public:
					typedef AEDescListItem_Container::size_type size_type;
					typedef AEDescListItem_Container::difference_type difference_type;
					typedef AEDescListItem value_type;
					typedef value_type::Pointer pointer;
					typedef value_type reference;
					typedef std::random_access_iterator_tag iterator_category;
					
				private:
					Owned< AEDescList >& list;
					size_type position;
					
					value_type GetValue() const
					{
						return value_type( AEDescListItem_Details( list, position ) );
					}
					
					iterator( Owned< AEDescList >& list, size_type position )
					: 
						list( list ),
						position( position )
					{
					}
				
				public:
					iterator& operator++()              { ++position;  return *this; }
					iterator operator++(int)            { iterator old = *this; operator++(); return old; }
					
					reference operator*() const               { return GetValue(); }
					//pointer operator->() const                { return &value; }
					
					operator const_iterator() const  { return const_iterator( list, position ); }
					
					friend bool operator==( const iterator& a, const iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const iterator& a, const iterator& b )    { return !( a == b ); }
			};
			
			AEDescListItem_Container( Owned< AEDescList >& list )
			: 
				list( list )
			{}
			
			iterator begin() const                    { return iterator( list, 1 ); }
			iterator end() const                      { return iterator( list, size() + 1 ); }
			
			size_type size() const  { return AECountItems( list ); }
			
			operator ConstAEDescListItem_Container() const  { return ConstAEDescListItem_Container( list ); }
			
		private:
			Owned< AEDescList >& list;
	};
	
	inline AEDescListItem_Container AEDescList_Items( Owned< AEDescList >& list )
	{
		return AEDescListItem_Container( list );
	}
	
};

