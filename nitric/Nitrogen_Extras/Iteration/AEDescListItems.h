/*	=================
 *	AEDescListItems.h
 *	=================
 */

#ifndef ITERATION_AEDESCLISTITEMS_H
#define ITERATION_AEDESCLISTITEMS_H

#ifndef NUCLEUS_PSEUDOREFERENCE_H
#include "Nucleus/Pseudoreference.h"
#endif

#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif

#ifndef NITROGEN_INDEXEDVALUECONTAINER_H
#include "Iteration/IndexedValueContainer.h"
#endif


namespace Nitrogen
{
	
	template < class AEDesc_Type >
	class Const_AEDescList_Item_Details
	{
		private:
			const AEDesc_Type& list;
			std::size_t index;
			
		public:
			typedef Nucleus::Owned< AEDesc_Type >  Value;
			typedef Value                          GetResult;
			
			Const_AEDescList_Item_Details( const AEDesc_Type& list, std::size_t index ) 
			: 
				list( list ), 
				index( index )  {}
			
			GetResult Get() const  { return AEGetNthDesc( list, index ); }
	};
	
	//typedef Nucleus::ConstPseudoreference< Const_AEDescList_Item_Details > Const_AEDescList_Item;
	
	template < class AEDesc_Type >
	class AEDescList_Item_Details
	{
		private:
			Nucleus::Owned< AEDesc_Type >& list;
			std::size_t index;
			
		public:
			typedef Nucleus::Owned< AEDesc_Type >  Value;
			typedef Value                          GetResult;
			typedef const AEDesc_Type&             SetParameter;
			
			AEDescList_Item_Details( Nucleus::Owned< AEDesc_Type >& list, std::size_t index ) : list( list ), index( index )  {}
			
			GetResult Get() const  { return AEGetNthDesc( list, index ); }
			void Set( SetParameter param ) const  { AEPutDesc( list, index, param ); }
	};
	
	//typedef Nucleus::Pseudoreference< AEDescList_Item_Details > AEDescList_Item;
	
	template < class AEDesc_Type >
	struct AEDescList_Item_Specifics
	{
		typedef UInt32                         size_type;
		typedef SInt32                         difference_type;
		typedef Nucleus::Owned< AEDesc_Type >  value_type;
		
		static std::size_t Size( const AEDesc_Type& list )
		{
			return AECountItems( list );
		}
		
		static value_type GetValue( const AEDesc_Type& list, size_type position )
		{
			return AEGetNthDesc< AEDesc_Type >( list, position + 1 );  // one-based
		}
		
		typedef const AEDesc_Type& ContainerState;
		typedef ContainerState IteratorState;
	};
	
	//typedef IndexedValueContainer< AEDescList_Item_Specifics > AEDescList_ItemValue_Container;
	
	typedef IndexedValueContainer< AEDescList_Item_Specifics< AEDesc_Data  > > AEDescList_Data_ItemValue_Container;
	typedef IndexedValueContainer< AEDescList_Item_Specifics< AEDesc_Token > > AEDescList_Token_ItemValue_Container;
	
	template < class AEDesc_Type >
	inline IndexedValueContainer< AEDescList_Item_Specifics< AEDesc_Type > > AEDescList_ItemValues( const AEDesc_Type& list )
	{
		return IndexedValueContainer< AEDescList_Item_Specifics< AEDesc_Type > >( list );
	}
	
	template < class AEDesc_Type >
	class AEDescList_Item_ValueIterator
	{
		public:
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			typedef Nucleus::Owned< AEDesc_Type > value_type;
		
		private:
			typedef AEDescList_Item_ValueIterator This;
			
			const AEDesc_Type& list;
			size_type position;
		
		public:
			AEDescList_Item_ValueIterator() : list( Nucleus::Make< AEDescList >() )  {}
			
			AEDescList_Item_ValueIterator( const AEDesc_Type& list, size_type pos )
			:
				list( list ),
				position( pos )
			{}
			
			value_type operator*()  { return AEGetNthDesc( list, position + 1 ); }
			
			This& operator++()     { ++position;  return *this; }
			This  operator++(int)  { This old = *this;  (void)operator++();  return old; }
			
			friend bool operator==( const This& a, const This& b )  { return a.position == b.position; }
			friend bool operator!=( const This& a, const This& b )  { return !( a == b ); }
	};
	
	template < class AEDesc_Type >
	inline AEDescList_Item_ValueIterator< AEDesc_Type >
	AEDescList_Item_ValuesBegin( const AEDesc_Type& list )
	{
		return AEDescList_Item_ValueIterator< AEDesc_Type >( list, 0 );
	}
	
	template < class AEDesc_Type >
	inline AEDescList_Item_ValueIterator< AEDesc_Type >
	AEDescList_Item_ValuesEnd( const AEDesc_Type& list )
	{
		return AEDescList_Item_ValueIterator< AEDesc_Type >( list, AECountItems( list ) );
	}
	
	template < class AEDesc_Type >
	class AEDescList_Item_BackInsertionIterator
	{
		private:
			typedef AEDescList_Item_BackInsertionIterator< AEDesc_Type > This;
			typedef This Proxy;
			typedef Nucleus::Owned< AEDesc_Type > List;
			
			List& list;
		
		public:
			AEDescList_Item_BackInsertionIterator( List& list ) : list( list )  {}
			
			Proxy& operator*()  { return *this; }
			
			Proxy& operator=( const AEDesc_Type& param )  { AEPutDesc( list, 0, param );  return *this; }
			
			This& operator++()     { return *this; }
			This& operator++(int)  { return *this; }
	};
	
	template < class AEDesc_Type >
	inline AEDescList_Item_BackInsertionIterator< AEDesc_Type >
	AEDescList_Item_BackInserter( Nucleus::Owned< AEDesc_Type >& list )
	{
		return AEDescList_Item_BackInsertionIterator< AEDesc_Type >( list );
	}
	
}

#endif

