// AEDescListItems.h

#ifndef NITROGEN_AEDESCLISTITEMS_H
#define NITROGEN_AEDESCLISTITEMS_H

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
	
	class Const_AEDescList_Item_Details
	{
		private:
			const AEDescList& list;
			std::size_t index;
			
		public:
			typedef Owned< AEDesc > Value;
			typedef Value           GetResult;
			
			Const_AEDescList_Item_Details( const AEDescList& list, std::size_t index ) 
			: 
				list( list ), 
				index( index )  {}
			
			GetResult Get() const  { return AEGetNthDesc( list, index ); }
	};
	
	typedef ConstPseudoreference< Const_AEDescList_Item_Details > Const_AEDescList_Item;
	
	class AEDescList_Item_Details
	{
		private:
			Owned< AEDescList >& list;
			std::size_t index;
			
		public:
			typedef Owned< AEDesc > Value;
			typedef Value           GetResult;
			typedef const AEDesc&   SetParameter;
			
			AEDescList_Item_Details( Owned< AEDescList >& list, std::size_t index ) : list( list ), index( index )  {}
			
			GetResult Get() const  { return AEGetNthDesc( list, index ); }
			void Set( SetParameter param ) const  { AEPutDesc( list, index, param ); }
	};
	
	typedef Pseudoreference< AEDescList_Item_Details > AEDescList_Item;
	
	struct AEDescList_Item_Specifics
	{
		typedef UInt32                   size_type;
		typedef SInt32                   difference_type;
		typedef AEDesc                   value_type;
		typedef Const_AEDescList_Item    const_reference;
		typedef AEDescList_Item          reference;
		typedef reference::ConstPointer  const_pointer;
		typedef reference::Pointer       pointer;
		
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
	
	typedef ConstIndexedContainer< AEDescList_Item_Specifics > Const_AEDescList_Item_Container;
	typedef      IndexedContainer< AEDescList_Item_Specifics >       AEDescList_Item_Container;
	
	inline Const_AEDescList_Item_Container AEDescList_Items( const AEDescList& list )
	{
		return Const_AEDescList_Item_Container( list );
	}
	
	inline AEDescList_Item_Container AEDescList_Items( Owned< AEDescList >& list )
	{
		return AEDescList_Item_Container( list );
	}
	
}

#endif

