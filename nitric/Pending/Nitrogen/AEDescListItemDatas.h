// AEDescListItemDatas.h

#ifndef NITROGEN_AEDESCLISTITEMDATAS_H
#define NITROGEN_AEDESCLISTITEMDATAS_H

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_PSEUDOREFERENCE_H
#include "Nitrogen/Pseudoreference.h"
#endif
#ifndef NITROGEN_INDEXEDCONTAINER_H
#include "Nitrogen/IndexedContainer.h"
#endif


namespace Nitrogen
{
	
	template < ::DescType type >
	class Const_AEDescList_ItemData_Details
	{
		private:
			const AEDescList& list;
			std::size_t index;
			
		public:
			typedef typename DescType_Traits< type >::Result    Value;
			typedef typename DescType_Traits< type >::Result    GetResult;
			
			Const_AEDescList_ItemData_Details( const AEDescList& list, std::size_t index ) 
			: 
				list( list ), 
				index( index )  {}
			
			GetResult Get() const  { return AEGetNthPtr< type >( list, index ); }
	};
	
	template < ::DescType type >
	class Const_AEDescList_ItemData : public ConstPseudoreference< Const_AEDescList_ItemData_Details< type > >
	{
		private:
			typedef ConstPseudoreference< Const_AEDescList_ItemData_Details< type > > Base;
			
		public:
			typedef typename Base::Details Details;
			typedef typename Base::GetResult GetResult;
			
			typedef Const_AEDescList_ItemData< type > Reference;
			typedef Const_AEDescList_ItemData< type > ConstReference;
			
			typedef Pseudopointer< Reference > Pointer;
			typedef Pseudopointer< ConstReference > ConstPointer;
			
			explicit Const_AEDescList_ItemData( Details theDetails ) : Base( theDetails )  {}
			Const_AEDescList_ItemData( const AEDescList& list, std::size_t index ) : Base( Details( list, index ) )  {}
			
			Pointer operator&() const  { return Pointer( Base::operator&().Get() ); }
			
			using Base::Get;
			
			operator GetResult() const  { return Get(); }
	};
	
	template < ::DescType type >
	class AEDescList_ItemData_Details
	{
		private:
			Owned< AEDescList >& list;
			std::size_t index;
			
		public:
			typedef typename DescType_Traits< type >::Result    Value;
			typedef typename DescType_Traits< type >::Result    GetResult;
			typedef typename DescType_Traits< type >::Parameter SetParameter;
			
			AEDescList_ItemData_Details( Owned< AEDescList >& list, std::size_t index ) : list( list ), index( index )  {}
			
			GetResult Get() const  { return AEGetNthPtr< type >( list, index ); }
			void Set( SetParameter param ) const  { AEPutPtr< type >( list, index, param ); }
	};
	
	template < ::DescType type >
	class AEDescList_ItemData : public Pseudoreference< AEDescList_ItemData_Details< type > >
	{
		private:
			typedef Pseudoreference< AEDescList_ItemData_Details< type > > Base;
			
		public:
			typedef typename Base::Details Details;
			typedef typename Base::GetResult GetResult;
			typedef typename Base::SetParameter SetParameter;
			
			typedef AEDescList_ItemData< type > Reference;
			typedef Const_AEDescList_ItemData< type > ConstReference;
			
			typedef Pseudopointer< Reference > Pointer;
			typedef Pseudopointer< ConstReference > ConstPointer;
			
			explicit AEDescList_ItemData( Details theDetails ) : Base( theDetails )  {}
			AEDescList_ItemData( Owned< AEDescList >& list, std::size_t index ) : Base( Details( list, index ) )  {}
			
			Pointer operator&() const  { return Pointer( Base::operator&().Get() ); }
			
			operator ConstReference() const  { return ConstReference( Base::operator&().Get() ); }
			
			using Base::Get;
			using Base::Set;
			
			operator GetResult() const  { return Get(); }
			
			const AEDescList_ItemData& operator=( SetParameter value ) const  { Set( value ); return *this; }
			const AEDescList_ItemData& operator=( const AEDescList_ItemData& rhs ) const  { Set( rhs.Get() ); return *this; }
	};
	
	template < ::DescType type >
	struct ReferenceTraits< AEDescList_ItemData< type > >
	{
		typedef AEDescList_ItemData< type > Reference;
		
		typedef typename Reference::Value           Value;
		typedef typename Reference::Pointer         Pointer;
		typedef typename Reference::ConstReference  ConstReference;
		typedef typename Reference::ConstPointer    ConstPointer;
	};
	
	template < ::DescType type >
	struct AEDescList_ItemData_Specifics
	{
		typedef UInt32                                    size_type;
		typedef SInt32                                    difference_type;
		typedef typename DescType_Traits< type >::Result  value_type;
		typedef Const_AEDescList_ItemData< type >         const_reference;
		typedef AEDescList_ItemData< type >               reference;
		typedef typename reference::ConstPointer          const_pointer;
		typedef typename reference::Pointer               pointer;
		
		static std::size_t Size( const AEDescList& list )
		{
			return AECountItems( list );
		}
		
		static const_reference GetReference( const AEDescList& list, size_type position )
		{
			return const_reference( list, position + 1 );  // one-based
		}
		
		static reference GetReference( Owned< AEDescList >& list, size_type position )
		{
			return reference( list, position + 1 );  // one-based
		}
		
		typedef const AEDescList& ConstContainerState;
		typedef ConstContainerState ConstIteratorState;
		
		typedef Owned< AEDescList >& ContainerState;
		typedef ContainerState IteratorState;
	};
	
	template < ::DescType type >
	class Const_AEDescList_ItemData_Container : public ConstIndexedContainer< AEDescList_ItemData_Specifics< type > >
	{
		private:
			typedef typename AEDescList_ItemData_Specifics< type >::ConstContainerState State;
			
			Const_AEDescList_ItemData_Container( State state )
			:
				ConstIndexedContainer< AEDescList_ItemData_Specifics< type > >( state )
			{}
		
		public:
			static Const_AEDescList_ItemData_Container< type > New( const AEDescList& list )
			{
				return Const_AEDescList_ItemData_Container< type >( State( list ) );
			}
			
			// Construct from base class.  Needed for non-const -> const conversion.
			// If we had typedef templates, then this class would be one.
			// We'd just use the non-const -> const conversion defined in the base, 
			// and this would amount to a copy constuctor.
			Const_AEDescList_ItemData_Container( const ConstIndexedContainer< AEDescList_ItemData_Specifics< type > >& base )
			:
				ConstIndexedContainer< AEDescList_ItemData_Specifics< type > >( base )
			{}
	};
	
	template < ::DescType type >
	class AEDescList_ItemData_Container : public IndexedContainer< AEDescList_ItemData_Specifics< type > >
	{
		private:
			typedef typename AEDescList_ItemData_Specifics< type >::ContainerState State;
			
			AEDescList_ItemData_Container( State state )
			:
				IndexedContainer< AEDescList_ItemData_Specifics< type > >( state )
			{}
		
		public:
			static AEDescList_ItemData_Container< type > New( Owned< AEDescList >& list )
			{
				return AEDescList_ItemData_Container< type >( State( list ) );
			}
		
		#ifndef __MWERKS__	
			
			operator Const_AEDescList_ItemData_Container< type >() const
			{
				return ConstIndexedContainer< AEDescList_ItemData_Specifics< type > >( *this );
			}
			
		#endif
	};
	
	template < ::DescType type >
	inline Const_AEDescList_ItemData_Container< type > AEDescList_ItemDatas( const AEDescList& list )
	{
		return Const_AEDescList_ItemData_Container< type >::New( list );
	}
	
	template < ::DescType type >
	inline AEDescList_ItemData_Container< type > AEDescList_ItemDatas( Owned< AEDescList >& list )
	{
		return AEDescList_ItemData_Container< type >::New( list );
	}
	
	template < ::DescType type, class Disposer >
	class AEDescList_ItemData_BackInsertionIterator
	{
		private:
			typedef AEDescList_ItemData_BackInsertionIterator< type, Disposer > This;
			typedef This Proxy;
			typedef Owned< AEDescList, Disposer > List;
			typedef typename DescType_Traits< type >::Parameter Parameter;
			
			List& list;
		
		public:
			AEDescList_ItemData_BackInsertionIterator( List& list ) : list( list )  {}
			
			Proxy& operator*()  { return *this; }
			
			Proxy& operator=( Parameter param )  { AEPutPtr< type >( list, 0, param );  return *this; }
			
			This& operator++()     { return *this; }
			This& operator++(int)  { return *this; }
	};
	
	template < ::DescType type, class Disposer >
	AEDescList_ItemData_BackInsertionIterator< type, Disposer >
	AEDescList_ItemData_BackInserter( Owned< AEDescList, Disposer >& list )
	{
		return AEDescList_ItemData_BackInsertionIterator< type, Disposer >( list );
	}
	
}

#endif

