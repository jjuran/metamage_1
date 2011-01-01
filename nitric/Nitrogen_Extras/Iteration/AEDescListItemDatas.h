/*	=====================
 *	AEDescListItemDatas.h
 *	=====================
 */

#ifndef ITERATION_AEDESCLISTITEMDATAS_H
#define ITERATION_AEDESCLISTITEMDATAS_H

// nucleus
#ifndef NUCLEUS_INDEXEDVALUESEQUENCE_HH
#include "nucleus/indexed_value_sequence.hh"
#endif

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif


namespace Nitrogen
{
	
	template < Mac::DescType type >
	struct AEDescList_ItemData_Specifics
	{
		typedef UInt32                                  size_type;
		typedef SInt32                                  difference_type;
		typedef typename DescType_result< type >::type  value_type;
		
		static std::size_t size( const AEDescList& list )
		{
			return AECountItems( list );
		}
		
		static value_type get_value( const AEDescList& list, size_type position )
		{
			return AEGetNthPtr< type >( list, position + 1 );  // one-based
		}
		
		typedef const AEDescList& context_type;
	};
	
	template < Mac::DescType type >
	class AEDescList_ItemDataValue_Container : public nucleus::indexed_value_sequence< AEDescList_ItemData_Specifics< type > >
	{
		private:
			typedef typename AEDescList_ItemData_Specifics< type >::context_type context_type;
			
			AEDescList_ItemDataValue_Container( context_type context )
			:
				nucleus::indexed_value_sequence< AEDescList_ItemData_Specifics< type > >( context )
			{}
		
		public:
			static AEDescList_ItemDataValue_Container< type > New( const AEDescList& list )
			{
				return AEDescList_ItemDataValue_Container< type >( list );
			}
	};
	
	template < Mac::DescType type >
	inline AEDescList_ItemDataValue_Container< type > AEDescList_ItemDataValues( const AEDescList& list )
	{
		return AEDescList_ItemDataValue_Container< type >::New( list );
	}
	
	template < Mac::DescType type >
	class AEDescList_ItemData_ValueIterator
	{
		public:
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			typedef typename DescType_result< type >::type Result, value_type;
		
		private:
			typedef AEDescList_ItemData_ValueIterator< type > This;
			
			const AEDescList& list;
			size_type position;
		
		public:
			AEDescList_ItemData_ValueIterator() : list( nucleus::make< AEDescList >() )  {}
			
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
	
	template < Mac::DescType type >
	inline AEDescList_ItemData_ValueIterator< type >
	AEDescList_ItemData_ValuesBegin( const AEDescList& list )
	{
		return AEDescList_ItemData_ValueIterator< type >( list, 0 );
	}
	
	template < Mac::DescType type >
	inline AEDescList_ItemData_ValueIterator< type >
	AEDescList_ItemData_ValuesEnd( const AEDescList& list )
	{
		return AEDescList_ItemData_ValueIterator< type >( list, AECountItems( list ) );
	}
	
	template < Mac::DescType type, class AEDescList_Type >
	class AEDescList_ItemData_BackInsertionIterator
	{
		private:
			typedef AEDescList_ItemData_BackInsertionIterator< type, AEDescList_Type > This;
			typedef This Proxy;
			typedef nucleus::owned< AEDescList_Type > List;
			typedef typename DescType_parameter< type >::type Parameter;
			
			List& list;
		
		public:
			AEDescList_ItemData_BackInsertionIterator( List& list ) : list( list )  {}
			
			Proxy& operator*()  { return *this; }
			
			Proxy& operator=( Parameter param )  { AEPutPtr< type >( list, 0, param );  return *this; }
			
			This& operator++()     { return *this; }
			This& operator++(int)  { return *this; }
	};
	
	template < Mac::DescType type, class AEDescList_Type >
	inline AEDescList_ItemData_BackInsertionIterator< type, AEDescList_Type >
	AEDescList_ItemData_BackInserter( nucleus::owned< AEDescList_Type >& list )
	{
		return AEDescList_ItemData_BackInsertionIterator< type, AEDescList_Type >( list );
	}
	
}

#endif

