/*	=====
 *	ic.cc
 *	=====
 */

// Nucleus
#include "Nucleus/AdvanceUntilFailureContainer.h"
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/InternetConfig.h"

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;


namespace Nitrogen
{
	
	class ICMapEntry_ContainerSpecifics
	{
		private:
			const ICMapEntry* itsEntries;
			const void* itsEntriesEnd;
		
		public:
			ICMapEntry_ContainerSpecifics( ICMapEntryHandle handle ) : itsEntries( *handle ),
			                                                           itsEntriesEnd( reinterpret_cast< const char* >( itsEntries ) + N::GetHandleSize( handle ) )  {}
			
			typedef const ICMapEntry* value_type;
			//typedef std::size_t value_type;
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			class EndOfEnumeration {};  // Not used
			
			value_type GetNextValue( const value_type& value )
			{
				const char* next = reinterpret_cast< const char* >( value ) + value->totalLength;
				
				ASSERT( next >  (const void*) itsEntries );
				ASSERT( next <= itsEntriesEnd );
				
				if ( next >= itsEntriesEnd )
				{
					return end_value();
				}
				
				return reinterpret_cast< const ICMapEntry* >( next );
			}
			
			       value_type begin_value() const  { return itsEntries; }
			static value_type end_value()          { return NULL;       }
	};
	
	class ICMapEntry_Container: public Nucleus::AdvanceUntilFailureContainer< ::Nitrogen::ICMapEntry_ContainerSpecifics >
	{
		friend ICMapEntry_Container ICMapEntries( NN::Shared< ICMapEntryHandle > entries );
		
		private:
			NN::Shared< ICMapEntryHandle > itsEntries;
			
			typedef ::Nitrogen::ICMapEntry_ContainerSpecifics Specifics;
			
			ICMapEntry_Container( const NN::Shared< ICMapEntryHandle >& entries ) : Nucleus::AdvanceUntilFailureContainer< Specifics >( Specifics( entries ) ),
			                                                                        itsEntries( entries )
			{
				N::HLock( entries.Get() );
			}
	};
	
	inline ICMapEntry_Container ICMapEntries( NN::Shared< ICMapEntryHandle > entries )
	{
		return ICMapEntry_Container( entries );
	}
	
}


struct ICMapEntryStrings
{
	std::string  extension;
	std::string  creatorAppName;
	std::string  postAppName;
	std::string  MIMEType;
	std::string  entryName;
};

static ICMapEntryStrings GetStringsFromICMapEntry( const ICMapEntry* entry )
{
	ICMapEntryStrings result;
	
	const unsigned char* p = (const unsigned char*) entry + entry->fixedLength;
	
	result.extension = NN::Convert< std::string >( p );
	
	p += 1 + p[0];
	
	result.creatorAppName = NN::Convert< std::string >( p );
	
	p += 1 + p[0];
	
	result.postAppName = NN::Convert< std::string >( p );
	
	p += 1 + p[0];
	
	result.MIMEType = NN::Convert< std::string >( p );
	
	p += 1 + p[0];
	
	result.entryName = NN::Convert< std::string >( p );
	
	return result;
}

static void ReportMapping( const ICMapEntry* entry )
{
	short version = entry->version;
	
	//const unsigned char* ext_ension = (const unsigned char*) entry + entry->fixedLength;
	
	ICMapEntryStrings strings = GetStringsFromICMapEntry( entry );
	
	const char* extension      = strings.extension     .c_str();
	const char* creatorAppName = strings.creatorAppName.c_str();
	const char* postAppName    = strings.postAppName   .c_str();
	const char* MIMEType       = strings.MIMEType      .c_str();
	const char* entryName      = strings.entryName     .c_str();
	
	std::printf( "v%d: %s - %s: %s\n", version, extension, creatorAppName, MIMEType );
}

int O::Main( int argc, argv_t argv )
{
	NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
	
	N::OSType signature = N::OSType( 'Poof' );
	
	NN::Owned< N::ICInstance > ic = N::ICStart( signature );
	
#if !TARGET_API_MAC_CARBON
	
	N::ThrowOSStatus( ::ICFindConfigFile( ic, 0, NULL ) );
	
#endif
	
	N::ThrowOSStatus( ::ICBegin( ic, icReadWritePerm ) );
	
	N::ICMapEntry_Container mappings = N::ICMapEntries( N::Handle_Cast< ICMapEntry >( N::ICFindPrefHandle( ic, kICMapping ) ) );
	
	std::for_each( mappings.begin(),
	               mappings.end(),
	               std::ptr_fun( ReportMapping ) );
	
	::ICEnd( ic );
	
	return 0;
}

