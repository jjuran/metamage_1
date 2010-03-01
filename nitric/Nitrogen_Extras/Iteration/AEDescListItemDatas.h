/*	=====================
 *	AEDescListItemDatas.h
 *	=====================
 */

#ifndef ITERATION_AEDESCLISTITEMDATAS_H
#define ITERATION_AEDESCLISTITEMDATAS_H

// Nucleus
#include "Nucleus/IndexedValueContainer.h"

#ifndef NUCLEUS_PSEUDOREFERENCE_H
#include "Nucleus/Pseudoreference.h"
#endif
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif


namespace Nitrogen
{
	
	template < DescType type >
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
	
	template < DescType type >
	class Const_AEDescList_ItemData : public Nucleus::ConstPseudoreference< Const_AEDescList_ItemData_Details< type > >
	{
		private:
			typedef Nucleus::ConstPseudoreference< Const_AEDescList_ItemData_Details< type > > Base;
			
		public:
			typedef typename Base::Details Details;
			typedef typename Base::GetResult GetResult;
			
			typedef Const_AEDescList_ItemData< type > Reference;
			typedef Const_AEDescList_ItemData< type > ConstReference;
			
			typedef Nucleus::Pseudopointer< Reference > Pointer;
			typedef Nucleus::Pseudopointer< ConstReference > ConstPointer;
			
			explicit Const_AEDescList_ItemData( Details theDetails ) : Base( theDetails )  {}
			Const_AEDescList_ItemData( const AEDescList& list, std::size_t index ) : Base( Details( list, index ) )  {}
			
			Pointer operator&() const  { return Pointer( Base::operator&().Get() ); }
			
			using Base::Get;
			
			operator GetResult() const  { return Get(); }
	};
	
	template < DescType type, class AEDescList_Type >
	class AEDescList_ItemData_Details
	{
		private:
			nucleus::owned< AEDescList_Type >& list;
			std::size_t index;
			
		public:
			typedef typename DescType_Traits< type >::Result    Value;
			typedef typename DescType_Traits< type >::Result    GetResult;
			typedef typename DescType_Traits< type >::Parameter SetParameter;
			
			AEDescList_ItemData_Details( nucleus::owned< AEDescList_Type >& list, std::size_t index ) : list( list ), index( index )  {}
			
			GetResult Get() const  { return AEGetNthPtr< type >( list, index ); }
			void Set( SetParameter param ) const  { AEPutPtr< type >( list, index, param ); }
	};
	
	template < DescType type, class AEDescList_Type >
	class AEDescList_ItemData : public Nucleus::Pseudoreference< AEDescList_ItemData_Details< type, AEDescList_Type > >
	{
		private:
			typedef Nucleus::Pseudoreference< AEDescList_ItemData_Details< type, AEDescList_Type > > Base;
			
		public:
			typedef typename Base::Details Details;
			typedef typename Base::GetResult GetResult;
			typedef typename Base::SetParameter SetParameter;
			
			typedef AEDescList_ItemData< type, AEDescList_Type > Reference;
			typedef Const_AEDescList_ItemData< type > ConstReference;
			
			typedef Nucleus::Pseudopointer< Reference > Pointer;
			typedef Nucleus::Pseudopointer< ConstReference > ConstPointer;
			
			explicit AEDescList_ItemData( Details theDetails ) : Base( theDetails )  {}
			AEDescList_ItemData( nucleus::owned< AEDescList_Type >& list, std::size_t index ) : Base( Details( list, index ) )  {}
			
			Pointer operator&() const  { return Pointer( Base::operator&().Get() ); }
			
			operator ConstReference() const  { return ConstReference( Base::operator&().Get() ); }
			
			using Base::Get;
			using Base::Set;
			
			operator GetResult() const  { return Get(); }
			
			const AEDescList_ItemData& operator=( SetParameter value ) const  { Set( value ); return *this; }
			const AEDescList_ItemData& operator=( const AEDescList_ItemData& rhs ) const  { Set( rhs.Get() ); return *this; }
	};
	
}

namespace Nucleus
{
	
	template < DescType type, class AEDescList_Type >
	struct ReferenceTraits< Nitrogen::AEDescList_ItemData< type, AEDescList_Type > >
	{
		typedef Nitrogen::AEDescList_ItemData< type, AEDescList_Type > Reference;
		
		typedef typename Reference::Value           Value;
		typedef typename Reference::Pointer         Pointer;
		typedef typename Reference::ConstReference  ConstReference;
		typedef typename Reference::ConstPointer    ConstPointer;
	};
	
}

namespace Nitrogen
{
	
	template < DescType type >
	struct AEDescList_ItemData_Specifics
	{
		typedef UInt32                                    size_type;
		typedef SInt32                                    difference_type;
		typedef typename DescType_Traits< type >::Result  value_type;
		
		static std::size_t Size( const AEDescList& list )
		{
			return AECountItems( list );
		}
		
		static value_type GetValue( const AEDescList& list, size_type position )
		{
			return AEGetNthPtr< type >( list, position + 1 );  // one-based
		}
		
		typedef const AEDescList& ContainerState;
		typedef ContainerState IteratorState;
	};
	
	template < DescType type >
	class AEDescList_ItemDataValue_Container : public Nucleus::IndexedValueContainer< AEDescList_ItemData_Specifics< type > >
	{
		private:
			typedef typename AEDescList_ItemData_Specifics< type >::ContainerState State;
			
			AEDescList_ItemDataValue_Container( State state )
			:
				Nucleus::IndexedValueContainer< AEDescList_ItemData_Specifics< type > >( state )
			{}
		
		public:
			static AEDescList_ItemDataValue_Container< type > New( const AEDescList& list )
			{
				return AEDescList_ItemDataValue_Container< type >( State( list ) );
			}
	};
	
	template < DescType type >
	inline AEDescList_ItemDataValue_Container< type > AEDescList_ItemDataValues( const AEDescList& list )
	{
		return AEDescList_ItemDataValue_Container< type >::New( list );
	}
	
	template < DescType type >
	class AEDescList_ItemData_ValueIterator
	{
		public:
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			typedef typename DescType_Traits< type >::Result Result, value_type;
		
		private:
			typedef AEDescList_ItemData_ValueIterator< type > This;
			
			const AEDescList& list;
			size_type position;
		
		public:
			AEDescList_ItemData_ValueIterator() : list( Nucleus::Make< AEDescList >() )  {}
			
			AEDescList_ItemData_ValueIterator( const AEDescList& list, size_type pos )
			:
				list( list ),
				position( pos )
			{}
			
			value_type operator*()  { return AEGetNthPtr< type >( list, position + 1 ); }
			
			This& operator++()     { ++position;  return *this; }
			This  operator++(int)  { This old = *this;  (void)operator++();  return old; }
			
			friend bool operator==( const This& a, const This& b )  { return a.position == b.position; }
			friend bool operator!=( const This& a, const This& b )  { return !( a == b ); }
	};
	
	template < DescType type >
	inline AEDescList_ItemData_ValueIterator< type >
	AEDescList_ItemData_ValuesBegin( const AEDescList& list )
	{
		return AEDescList_ItemData_ValueIterator< type >( list, 0 );
	}
	
	template < DescType type >
	inline AEDescList_ItemData_ValueIterator< type >
	AEDescList_ItemData_ValuesEnd( const AEDescList& list )
	{
		return AEDescList_ItemData_ValueIterator< type >( list, AECountItems( list ) );
	}
	
	template < DescType type, class AEDescList_Type >
	class AEDescList_ItemData_BackInsertionIterator
	{
		private:
			typedef AEDescList_ItemData_BackInsertionIterator< type, AEDescList_Type > This;
			typedef This Proxy;
			typedef nucleus::owned< AEDescList_Type > List;
			typedef typename DescType_Traits< type >::Parameter Parameter;
			
			List& list;
		
		public:
			AEDescList_ItemData_BackInsertionIterator( List& list ) : list( list )  {}
			
			Proxy& operator*()  { return *this; }
			
			Proxy& operator=( Parameter param )  { AEPutPtr< type >( list, 0, param );  return *this; }
			
			This& operator++()     { return *this; }
			This& operator++(int)  { return *this; }
	};
	
	template < DescType type, class AEDescList_Type >
	inline AEDescList_ItemData_BackInsertionIterator< type, AEDescList_Type >
	AEDescList_ItemData_BackInserter( nucleus::owned< AEDescList_Type >& list )
	{
		return AEDescList_ItemData_BackInsertionIterator< type, AEDescList_Type >( list );
	}
	
}

#endif

