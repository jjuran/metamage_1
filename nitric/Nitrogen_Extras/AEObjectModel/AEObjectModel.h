/*	===============
 *	AEObjectModel.h
 *	===============
 */

#ifndef AEOBJECTMODEL_AEOBJECTMODEL_H
#define AEOBJECTMODEL_AEOBJECTMODEL_H

#include <map>

#include <AERegistry.h>

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace Nitrogen
{
	
	inline Owned< AEToken, AETokenDisposer > AEInitializeToken()
	{
		return Owned< AEToken, AETokenDisposer >::Seize( Make< AEToken >() );
	}
	
	inline Owned< AEToken, AETokenDisposer > GetRootToken()
	{
		return AEInitializeToken();
	}
	
	inline Owned< AEDesc > GetRootObjectSpecifier()
	{
		return AEInitializeDesc();
	}
	
	#pragma mark -
	#pragma mark ¥ GetObjectClass ¥
	
	template < ::DescType tokenType > struct GetObjectClass_Traits;
	
	template < ::DescType objectClass > struct StaticObjectClass_Traits
	{
		static AEObjectClass GetObjectClass( const AEToken& )
		{
			return objectClass;
		}
	};
	
	template <> struct GetObjectClass_Traits< typeNull > : StaticObjectClass_Traits< cApplication > {};
	
	class ObjectClassGetter
	{
		public:
			typedef AEObjectClass (*ObjectClassGetterType)( const AEToken& );
		
		private:
			typedef std::map< DescType, ObjectClassGetterType >  Map;
			
			Map map;
			
			// not implemented:
			ObjectClassGetter( const ObjectClassGetter& );
			ObjectClassGetter& operator=( const ObjectClassGetter& );
		
		public:
			ObjectClassGetter();
			
			template < ::DescType tokenType >
			void Register()
			{
				map[ tokenType ] = GetObjectClass_Traits< tokenType >::GetObjectClass;
			}
			
			AEObjectClass GetObjectClass( const AEToken&  obj );
	};
	
	ObjectClassGetter& TheGlobalObjectClassGetter();
	
	template < ::DescType tokenType >
	void RegisterObjectClassGetter()
	{
		TheGlobalObjectClassGetter().template Register< tokenType >();
	}
	
	AEObjectClass GetObjectClass( const AEToken&  obj );
	
	#pragma mark -
	#pragma mark ¥ MakeDataDescriptor ¥
	
	template < ::DescType tokenType > struct MakeDataDescriptor_Traits;
	
	template <> struct MakeDataDescriptor_Traits< typeNull >
	{
		static Owned< AEDesc > MakeDataDescriptor( const AEToken& )  { return GetRootObjectSpecifier(); }
	};
	
	class DataDescriptorMaker
	{
		public:
			typedef Owned< AEDesc > (*DataDescriptorMakerType)( const AEToken& );
		
		private:
			typedef std::map< DescType, DataDescriptorMakerType >  Map;
			
			Map map;
			
			// not implemented:
			DataDescriptorMaker( const DataDescriptorMaker& );
			DataDescriptorMaker& operator=( const DataDescriptorMaker& );
		
		public:
			DataDescriptorMaker();
			
			template < ::DescType tokenType >
			void Register()
			{
				map[ tokenType ] = MakeDataDescriptor_Traits< tokenType >::MakeDataDescriptor;
			}
			
			Owned< AEDesc > MakeDataDescriptor( const AEToken& obj );
	};
	
	DataDescriptorMaker& TheGlobalDataDescriptorMaker();
	
	template < ::DescType tokenType >
	void RegisterDataDescriptorMaker()
	{
		TheGlobalDataDescriptorMaker().template Register< tokenType >();
	}
	
	Owned< AEDesc > MakeDataDescriptor( const AEToken& obj );
	
	#pragma mark -
	#pragma mark ¥ Compare ¥
	
	template < ::DescType tokenType > struct Compare_Traits;
	
	class Comparer
	{
		public:
			typedef bool (*ComparerType)( AECompOperator, const AEToken&, const AEToken& );
		
		private:
			typedef std::map< DescType, ComparerType >  Map;
			
			Map map;
			
			// not implemented:
			Comparer( const Comparer& );
			Comparer& operator=( const Comparer& );
		
		public:
			Comparer()  {}
			
			template < ::DescType tokenType >
			void Register()
			{
				map[ tokenType ] = Compare_Traits< tokenType >::Compare;
			}
			
			bool Compare( AECompOperator  op,
			              const AEToken&  obj1,
			              const AEToken&  obj2 );
	};
	
	Comparer& TheGlobalComparer();
	
	template < ::DescType tokenType >
	void RegisterComparer()
	{
		TheGlobalComparer().template Register< tokenType >();
	}
	
	bool Compare( AECompOperator  op,
			      const AEToken&  obj1,
			      const AEToken&  obj2 );
	
	#pragma mark -
	#pragma mark ¥ Count ¥
	
	template < ::DescType desiredClass, ::DescType containerType > struct Count_Traits;
	
	class Counter
	{
		public:
			typedef std::size_t (*CounterType)( AEObjectClass, AEObjectClass, const AEToken& );
		
		private:
			class Key
			{
				private:
					AEObjectClass desiredClass;
					DescType      containerType;
				
				public:
					Key()  {}
					Key( AEObjectClass desiredClass, DescType containerType )
					:
						desiredClass ( desiredClass  ),
						containerType( containerType )
					{}
					
					friend bool operator<( const Key& a, const Key& b )
					{
						return a.desiredClass < b.desiredClass || a.desiredClass == b.desiredClass
						                                       && a.containerType < b.containerType;
					}
			};
			
			typedef std::map< Key, CounterType >  Map;
			
			Map map;
			
			// not implemented:
			Counter( const Counter& );
			Counter& operator=( const Counter& );
		
		public:
			Counter()  {}
			
			template < ::DescType desiredClass, ::DescType containerType >
			void Register()
			{
				map[ Key( desiredClass, containerType ) ] = Count_Traits< desiredClass, containerType >::Count;
			}
			
			std::size_t Count( AEObjectClass   desiredClass,
			                   AEObjectClass   containerClass,
			                   const AEToken&  containerToken );
	};
	
	Counter& TheGlobalCounter();
	
	template < ::DescType desiredClass, ::DescType containerType >
	void RegisterCounter()
	{
		TheGlobalCounter().template Register< desiredClass, containerType >();
	}
	
	std::size_t Count( AEObjectClass   desiredClass,
			           AEObjectClass   containerClass,
			           const AEToken&  containerToken );
	
	#pragma mark -
	#pragma mark ¥ DisposeToken ¥
	
	template < ::DescType tokenType > struct DisposeToken_Traits;
	
	template <> struct DisposeToken_Traits< typeAEList >
	{
		static void DisposeToken( Owned< AETokenList > tokenList )
		{
			std::size_t count = AECountItems( tokenList );
			for ( std::size_t i = 1;  i <= count;  ++i )
			{
				// Get the token from the list (which allocates a new AEDesc),
				// and call AEDisposeToken on it, which disposes both
				// any token-related resources and the newly allocated AEDesc itself.
				// The copy of the token AEDesc remaining in the list descriptor will go 
				// when the list goes.
				// We have to call AEGetNthToken() because AEGetNthDesc() returns 
				// Owned< AEDesc > but AEDisposeToken accepts Owned< AEDesc, AETokenDisposer >.
				// Alternately, we could write AETokenList_Items() or somesuch.
				AEDisposeToken( AEGetNthToken( tokenList, i ) );
			}
			
			// Optional
			AEDisposeDesc( tokenList );
		}
	};
	
	class TokenDisposer
	{
		public:
			typedef void (*TokenDisposerType)( Owned< AEToken > );
		
		private:
			typedef std::map< DescType, TokenDisposerType >  Map;
			
			Map map;
			
			// not implemented:
			TokenDisposer( const TokenDisposer& );
			TokenDisposer& operator=( const TokenDisposer& );
		
		public:
			TokenDisposer();
			
			template < ::DescType tokenType >
			void Register()
			{
				map[ tokenType ] = DisposeToken_Traits< tokenType >::DisposeToken;
			}
			
			void DisposeToken( Owned< AEToken > token );
	};
	
	TokenDisposer& TheGlobalTokenDisposer();
	
	template < ::DescType tokenType >
	void RegisterTokenDisposer()
	{
		TheGlobalTokenDisposer().template Register< tokenType >();
	}
	
	void DisposeToken( Owned< AEToken > token );
	
	#pragma mark -
	#pragma mark ¥ OSL Object Callbacks ¥
	
	pascal OSErr OSLCompare( ::DescType      op,
	                         const AEToken*  obj1,
	                         const AEToken*  obj2,
	                         ::Boolean*      result );
	
	pascal OSErr OSLCount( ::DescType      desiredClass,
	                       ::DescType      containerClass,
	                       const AEToken*  containerToken,
	                       SInt32*         result );
	
	pascal OSErr OSLDisposeToken( AEToken* token );
	
	void AESetObjectCallbacks();
	
	Owned< AEToken, AETokenDisposer > DispatchAccessToList( AEObjectClass   desiredClass,
	                                                        const AEToken&  containerToken,
	                                                        AEObjectClass   containerClass,
	                                                        AEEnumerated    keyForm,
	                                                        const AEDesc&   keyData,
	                                                        RefCon );
	
}

#endif
