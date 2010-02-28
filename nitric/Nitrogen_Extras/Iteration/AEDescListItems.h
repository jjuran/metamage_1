/*	=================
 *	AEDescListItems.h
 *	=================
 */

#ifndef ITERATION_AEDESCLISTITEMS_H
#define ITERATION_AEDESCLISTITEMS_H

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
	
	class Const_AEDescList_Item_Details
	{
		private:
			const AEDescList& list;
			std::size_t index;
			
		public:
			typedef Nucleus::Owned< AEDesc_Data >  Value;
			typedef Value                          GetResult;
			
			Const_AEDescList_Item_Details( const AEDescList& list, std::size_t index ) 
			: 
				list( list ), 
				index( index )  {}
			
			GetResult Get() const  { return AEGetNthDesc( list, index ); }
	};
	
	typedef Nucleus::ConstPseudoreference< Const_AEDescList_Item_Details > Const_AEDescList_Item;
	
	class AEDescList_Item_Details
	{
		private:
			Nucleus::Owned< AEDescList_Data >&  list;
			std::size_t                         index;
			
		public:
			typedef Nucleus::Owned< AEDesc_Data >  Value;
			typedef Value                          GetResult;
			typedef const AEDesc_Data&             SetParameter;
			
			AEDescList_Item_Details( Nucleus::Owned< AEDescList_Data >& list, std::size_t index ) : list( list ), index( index )  {}
			
			GetResult Get() const  { return AEGetNthDesc( list, index ); }
			void Set( SetParameter param ) const  { AEPutDesc( list, index, param ); }
	};
	
	typedef Nucleus::Pseudoreference< AEDescList_Item_Details > AEDescList_Item;
	
	struct AEDescList_Item_Specifics
	{
		typedef UInt32                         size_type;
		typedef SInt32                         difference_type;
		typedef Nucleus::Owned< AEDesc_Data >  value_type;
		
		static std::size_t Size( const AEDescList& list )
		{
			return AECountItems( list );
		}
		
		static value_type GetValue( const AEDescList& list, size_type position )
		{
			return AEGetNthDesc( list, position + 1 );  // one-based
		}
		
		typedef const AEDescList& ContainerState;
		typedef ContainerState IteratorState;
	};
	
	typedef Nucleus::IndexedValueContainer< AEDescList_Item_Specifics > AEDescList_ItemValue_Container;
	
	inline AEDescList_ItemValue_Container AEDescList_ItemValues( const AEDescList& list )
	{
		return AEDescList_ItemValue_Container( list );
	}
	
	class AEDescList_Item_ValueIterator
	{
		public:
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			typedef Nucleus::Owned< AEDesc_Data > value_type;
		
		private:
			typedef AEDescList_Item_ValueIterator This;
			
			const AEDescList& list;
			size_type position;
		
		public:
			AEDescList_Item_ValueIterator() : list( Nucleus::Make< AEDescList >() )  {}
			
			AEDescList_Item_ValueIterator( const AEDescList& list, size_type pos )
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
	
	inline AEDescList_Item_ValueIterator
	AEDescList_Item_ValuesBegin( const AEDescList& list )
	{
		return AEDescList_Item_ValueIterator( list, 0 );
	}
	
	inline AEDescList_Item_ValueIterator
	AEDescList_Item_ValuesEnd( const AEDescList& list )
	{
		return AEDescList_Item_ValueIterator( list, AECountItems( list ) );
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
			
			// For normal use, i.e. AEDesc_Data
			Proxy& operator=(           const AEDesc_Type&  param )  { AEPutDesc( list, 0, param );  return *this; }
			
			// For AEDesc_Token, where param must be Owned
			Proxy& operator=( Nucleus::Owned< AEDesc_Type > param )  { AEPutDesc( list, 0, param );  return *this; }
			
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

