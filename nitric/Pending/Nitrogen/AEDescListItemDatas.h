// AEDescListItemDataC.h

#include "Nitrogen/AEDataModel.h"
#include "Nitrogen/Pseudoreference.h"


namespace Nitrogen {
	
	template < ::DescType type >
	class ConstAEDescListItemData_Details
	{
		private:
			const AEDescList& list;
			std::size_t index;
			
		public:
			typedef typename DescType_Traits< type >::Result    Value;
			typedef typename DescType_Traits< type >::Result    GetResult;
			
			ConstAEDescListItemData_Details( const AEDescList& list, std::size_t index ) 
			: 
				list( list ), 
				index( index )  {}
			
			GetResult Get() const  { return AEGetNthPtr< type >( list, index ); }
	};
	
	template < ::DescType type >
	class ConstAEDescListItemData : public ConstPseudoreference< ConstAEDescListItemData_Details< type > >
	{
		private:
			typedef ConstPseudoreference< ConstAEDescListItemData_Details< type > > Base;
			
		public:
			typedef typename Base::Details Details;
			typedef typename Base::GetResult GetResult;
			
			typedef ConstAEDescListItemData< type > Reference;
			typedef ConstAEDescListItemData< type > ConstReference;
			
			typedef Pseudopointer< Reference > Pointer;
			typedef Pseudopointer< ConstReference > ConstPointer;
			
			explicit ConstAEDescListItemData( Details theDetails ) : Base( theDetails )  {}
			ConstAEDescListItemData( const AEDescList& list, std::size_t index ) : Base( Details( list, index ) )  {}
			
			Pointer operator&() const  { return Pointer( Base::operator&().Get() ); }
			
			using Base::Get;
			
			operator GetResult() const  { return Get(); }
	};
	
	template < ::DescType type >
	class ConstAEDescListItemData_Container
	{
		public:
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			class const_iterator
			{
				friend class ConstAEDescListItemData_Container;
				
				public:
					typedef ConstAEDescListItemData_Container::size_type size_type;
					typedef ConstAEDescListItemData_Container::difference_type difference_type;
					typedef ConstAEDescListItemData< type > value_type;
					typedef typename value_type::ConstPointer pointer;
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
			
			ConstAEDescListItemData_Container( const AEDescList& list )
			: 
				list( list )
			{}
			
			const_iterator begin() const                    { return const_iterator( list, 1 ); }
			const_iterator end() const                      { return const_iterator( list, size() + 1 ); }
			
			size_type size() const  { return AECountItems( list ); }
			
		private:
			const AEDescList& list;
	};
	
	template < ::DescType type >
	inline ConstAEDescListItemData_Container< type > AEDescList_ItemDatas( const AEDescList& list )
	{
		return ConstAEDescListItemData_Container< type >( list );
	}
	
	template < ::DescType type >
	class AEDescListItemData_Details
	{
		private:
			Owned< AEDescList >& list;
			std::size_t index;
			
		public:
			typedef typename DescType_Traits< type >::Result    Value;
			typedef typename DescType_Traits< type >::Result    GetResult;
			typedef typename DescType_Traits< type >::Parameter SetParameter;
			
			AEDescListItemData_Details( Owned< AEDescList >& list, std::size_t index ) : list( list ), index( index )  {}
			
			GetResult Get() const  { return AEGetNthPtr< type >( list, index ); }
			void Set( SetParameter param ) const  { AEPutPtr< type >( list, index, param ); }
	};
	
	template < ::DescType type >
	class AEDescListItemData : public Pseudoreference< AEDescListItemData_Details< type > >
	{
		private:
			typedef Pseudoreference< AEDescListItemData_Details< type > > Base;
			
		public:
			typedef typename Base::Details Details;
			typedef typename Base::GetResult GetResult;
			typedef typename Base::SetParameter SetParameter;
			
			typedef AEDescListItemData< type > Reference;
			typedef ConstAEDescListItemData< type > ConstReference;
			
			typedef Pseudopointer< Reference > Pointer;
			typedef Pseudopointer< ConstReference > ConstPointer;
			
			explicit AEDescListItemData( Details theDetails ) : Base( theDetails )  {}
			AEDescListItemData( Owned< AEDescList >& list, std::size_t index ) : Base( Details( list, index ) )  {}
			
			Pointer operator&() const  { return Pointer( Base::operator&().Get() ); }
			
			operator ConstReference() const  { return ConstReference( Base::operator&().Get() ); }
			
			using Base::Get;
			using Base::Set;
			
			operator GetResult() const  { return Get(); }
			
			const AEDescListItemData& operator=( SetParameter value ) const  { Set( value ); return *this; }
			const AEDescListItemData& operator=( const AEDescListItemData& rhs ) const  { Set( rhs.Get() ); return *this; }
	};
	
	template < ::DescType type >
	struct ReferenceTraits< AEDescListItemData< type > >
	{
		typedef AEDescListItemData< type > Reference;
		
		typedef typename Reference::Value           Value;
		typedef typename Reference::Pointer         Pointer;
		typedef typename Reference::ConstReference  ConstReference;
		typedef typename Reference::ConstPointer    ConstPointer;
	};
	
	template < ::DescType type >
	class AEDescListItemData_Container
	{
		public:
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			class const_iterator
			{
				friend class iterator;
				
				public:
					typedef AEDescListItemData_Container::size_type size_type;
					typedef AEDescListItemData_Container::difference_type difference_type;
					typedef ConstAEDescListItemData< type > value_type;
					typedef typename value_type::ConstPointer pointer;
					typedef value_type reference;
					typedef std::random_access_iterator_tag iterator_category;
					
				private:
					const Owned< AEDescList >& list;
					size_type position;
					
					value_type GetValue() const
					{
						return value_type( list, position );
					}
					
					const_iterator( const Owned< AEDescList >& list, size_type position )
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
				friend class AEDescListItemData_Container;
				
				public:
					typedef AEDescListItemData_Container::size_type size_type;
					typedef AEDescListItemData_Container::difference_type difference_type;
					typedef AEDescListItemData< type > value_type;
					typedef typename value_type::Pointer pointer;
					typedef value_type reference;
					typedef std::random_access_iterator_tag iterator_category;
					
				private:
					Owned< AEDescList >& list;
					size_type position;
					
					value_type GetValue() const
					{
						return value_type( list, position );
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
					
					operator const_iterator() const  { return const_iterator( list, index ); }
					
					friend bool operator==( const iterator& a, const iterator& b )    { return a.position == b.position; }
					friend bool operator!=( const iterator& a, const iterator& b )    { return !( a == b ); }
			};
			
			AEDescListItemData_Container( Owned< AEDescList >& list )
			: 
				list( list )
			{}
			
			iterator begin() const                    { return iterator( list, 1 ); }
			iterator end() const                      { return iterator( list, size() + 1 ); }
			
			size_type size() const  { return AECountItems( list ); }
			
			operator ConstAEDescListItemData_Container< type >() const  { return ConstAEDescListItemData_Container< type >( list ); }
			
		private:
			Owned< AEDescList >& list;
	};
	
	template < ::DescType type >
	inline AEDescListItemData_Container< type > AEDescList_ItemDatas( Owned< AEDescList >& list )
	{
		return AEDescListItemData_Container< type >( list );
	}
	
};

