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
	
	Owned< AEObjectSpecifier > AECreateObjectSpecifier( AEObjectClass             objectClass,
	                                                    const AEObjectSpecifier&  container,
	                                                    AEEnumeration             keyForm,
	                                                    const AEDesc&             keyData );
	
	#pragma mark -
	#pragma mark ¥ GetObjectClass ¥
	
	AEObjectClass GetObjectClass( const AEToken&  obj );
	
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
			typedef AEObjectClass (*Callback)( const AEToken& );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			ObjectClassGetter( const ObjectClassGetter& );
			ObjectClassGetter& operator=( const ObjectClassGetter& );
		
		public:
			ObjectClassGetter();
			
			void Register( DescType tokenType, ObjectClassGetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, GetObjectClass_Traits< tokenType >::GetObjectClass );
			}
			
			AEObjectClass GetObjectClass( const AEToken&  obj );
	};
	
	ObjectClassGetter& TheGlobalObjectClassGetter();
	
	inline void RegisterObjectClassGetter( DescType tokenType, ObjectClassGetter::Callback callback )
	{
		TheGlobalObjectClassGetter().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	void RegisterObjectClassGetter()
	{
		TheGlobalObjectClassGetter().template Register< tokenType >();
	}
	
	#pragma mark -
	#pragma mark ¥ GetData ¥
	
	Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType = typeWildCard );
	
	template < ::DescType tokenType > struct GetData_Traits;
	
	template <> struct GetData_Traits< typeNull >
	{
		static Owned< AEDesc > GetData( const AEToken&, DescType )  { return GetRootObjectSpecifier(); }
	};
	
	class DataGetter
	{
		public:
			typedef Owned< AEDesc > (*Callback)( const AEToken&, DescType );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			DataGetter( const DataGetter& );
			DataGetter& operator=( const DataGetter& );
		
		public:
			DataGetter();
			
			void Register( DescType tokenType, DataGetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, GetData_Traits< tokenType >::GetData );
			}
			
			Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType );
	};
	
	DataGetter& TheGlobalDataGetter();
	
	inline void RegisterDataGetter( DescType tokenType, DataGetter::Callback callback )
	{
		TheGlobalDataGetter().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	void RegisterDataGetter()
	{
		TheGlobalDataGetter().template Register< tokenType >();
	}
	
	#pragma mark -
	#pragma mark ¥ SetData ¥
	
	void SetData( const AEToken& obj, const AEDesc& data );
	
	template < ::DescType tokenType > struct SetData_Traits;
	
	class DataSetter
	{
		public:
			typedef void (*Callback)( const AEToken&, const AEDesc& );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			DataSetter( const DataSetter& );
			DataSetter& operator=( const DataSetter& );
		
		public:
			DataSetter();
			
			void Register( DescType tokenType, DataSetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, SetData_Traits< tokenType >::SetData );
			}
			
			void SetData( const AEToken& obj, const AEDesc& data );
	};
	
	DataSetter& TheGlobalDataSetter();
	
	inline void RegisterDataSetter( DescType tokenType, DataSetter::Callback callback )
	{
		TheGlobalDataSetter().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	void RegisterDataSetter()
	{
		TheGlobalDataSetter().template Register< tokenType >();
	}
	
	#pragma mark -
	#pragma mark ¥ Compare ¥
	
	bool Compare( AECompOperator  op,
			      const AEToken&  obj1,
			      const AEToken&  obj2 );
	
	template < ::DescType tokenType > struct Compare_Traits;
	
	class Comparer
	{
		public:
			typedef bool (*Callback)( AECompOperator, const AEToken&, const AEToken& );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			Comparer( const Comparer& );
			Comparer& operator=( const Comparer& );
		
		public:
			Comparer()  {}
			
			void Register( DescType tokenType, Comparer::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, Compare_Traits< tokenType >::Compare );
			}
			
			bool Compare( AECompOperator  op,
			              const AEToken&  obj1,
			              const AEToken&  obj2 );
	};
	
	Comparer& TheGlobalComparer();
	
	inline void RegisterComparer( DescType tokenType, Comparer::Callback callback )
	{
		TheGlobalComparer().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	void RegisterComparer()
	{
		TheGlobalComparer().template Register< tokenType >();
	}
	
	#pragma mark -
	#pragma mark ¥ Count ¥
	
	std::size_t Count( AEObjectClass   desiredClass,
			           AEObjectClass   containerClass,
			           const AEToken&  containerToken );
	
	template < ::DescType desiredClass, ::DescType containerType > struct Count_Traits;
	
	class Counter
	{
		public:
			typedef std::size_t (*Callback)( AEObjectClass, AEObjectClass, const AEToken& );
		
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
			
			typedef std::map< Key, Callback >  Map;
			
			Map map;
			
			// not implemented:
			Counter( const Counter& );
			Counter& operator=( const Counter& );
		
		public:
			Counter()  {}
			
			void Register( AEObjectClass desiredClass, DescType containerType, Counter::Callback callback )
			{
				map[ Key( desiredClass, containerType ) ] = callback;
			}
			
			template < ::DescType desiredClass, ::DescType containerType >
			void Register()
			{
				Register( desiredClass, containerType, Count_Traits< desiredClass, containerType >::Count );
			}
			
			std::size_t Count( AEObjectClass   desiredClass,
			                   AEObjectClass   containerClass,
			                   const AEToken&  containerToken );
	};
	
	Counter& TheGlobalCounter();
	
	inline void RegisterCounter( AEObjectClass desiredClass, DescType containerType, Counter::Callback callback )
	{
		TheGlobalCounter().Register( desiredClass, containerType, callback );
	}
	
	template < ::DescType desiredClass, ::DescType containerType >
	void RegisterCounter()
	{
		TheGlobalCounter().template Register< desiredClass, containerType >();
	}
	
	#pragma mark -
	#pragma mark ¥ DisposeToken ¥
	
	void DisposeToken( Owned< AEToken > token );
	
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
			typedef void (*Callback)( Owned< AEToken > );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			TokenDisposer( const TokenDisposer& );
			TokenDisposer& operator=( const TokenDisposer& );
		
		public:
			TokenDisposer();
			
			void Register( DescType tokenType, TokenDisposer::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, DisposeToken_Traits< tokenType >::DisposeToken );
			}
			
			void DisposeToken( Owned< AEToken > token );
	};
	
	TokenDisposer& TheGlobalTokenDisposer();
	
	inline void RegisterTokenDisposer( DescType tokenType, TokenDisposer::Callback callback )
	{
		TheGlobalTokenDisposer().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	void RegisterTokenDisposer()
	{
		TheGlobalTokenDisposer().template Register< tokenType >();
	}
	
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
