/*	=====
 *	ic.cc
 *	=====
 */

// Standard C++
#include <algorithm>
#include <functional>

// Standard C/C++
#include <cstdio>

// Debug
#include "debug/assert.hh"

// plus
#include "plus/make_string.hh"

// nucleus
#include "nucleus/advance_until_done_sequence.hh"
#include "nucleus/shared.hh"

// Nitrogen
#include "Nitrogen/MacTypes.hh"
#include "Nitrogen/InternetConfig.hh"

// Orion
#include "Orion/Main.hh"


namespace Nitrogen
{
	
	class ICMapEntry_ContainerSpecifics
	{
		private:
			const ICMapEntry* itsEntries;
			const void* itsEntriesEnd;
		
		public:
			ICMapEntry_ContainerSpecifics( ICMapEntryHandle handle ) : itsEntries( *handle ),
			                                                           itsEntriesEnd( reinterpret_cast< const char* >( itsEntries ) + Nitrogen::GetHandleSize( handle ) )
			{
			}
			
			typedef ICMapEntry value_type;
			//typedef std::size_t value_type;
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			typedef const value_type* key_type;
			
			key_type get_next_key( const key_type& value ) const
			{
				const char* next = reinterpret_cast< const char* >( value ) + value->totalLength;
				
				ASSERT( next >  (const void*) itsEntries );
				ASSERT( next <= itsEntriesEnd );
				
				if ( next >= itsEntriesEnd )
				{
					return end_key();
				}
				
				return reinterpret_cast< const ICMapEntry* >( next );
			}
			
			static const value_type* GetPointer( key_type ptr )  { return ptr; }
			
			       key_type begin_key() const  { return itsEntries; }
			static key_type end_key()          { return NULL;       }
	};
	
	class ICMapEntry_Container : public nucleus::advance_until_done_sequence< ::Nitrogen::ICMapEntry_ContainerSpecifics >
	{
		friend ICMapEntry_Container ICMapEntries( nucleus::shared< ICMapEntryHandle > entries );
		
		private:
			nucleus::shared< ICMapEntryHandle > itsEntries;
			
			typedef ::Nitrogen::ICMapEntry_ContainerSpecifics Specifics;
			
			ICMapEntry_Container( const nucleus::shared< ICMapEntryHandle >& entries ) : nucleus::advance_until_done_sequence< Specifics >( Specifics( entries ) ),
			                                                                             itsEntries( entries )
			{
				if ( !TARGET_API_MAC_OSX )
				{
					Nitrogen::HLock( entries.get() );
				}
			}
	};
	
	inline ICMapEntry_Container ICMapEntries( nucleus::shared< ICMapEntryHandle > entries )
	{
		return ICMapEntry_Container( entries );
	}
	
}

namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	struct ICMapEntryStrings
	{
		plus::string  extension;
		plus::string  creatorAppName;
		plus::string  postAppName;
		plus::string  MIMEType;
		plus::string  entryName;
	};
	
	static ICMapEntryStrings GetStringsFromICMapEntry( const ICMapEntry& entry )
	{
		ICMapEntryStrings result;
		
		const unsigned char* p = (const unsigned char*) &entry + entry.fixedLength;
		
		result.extension = plus::make_string( p );
		
		p += 1 + p[0];
		
		result.creatorAppName = plus::make_string( p );
		
		p += 1 + p[0];
		
		result.postAppName = plus::make_string( p );
		
		p += 1 + p[0];
		
		result.MIMEType = plus::make_string( p );
		
		p += 1 + p[0];
		
		result.entryName = plus::make_string( p );
		
		return result;
	}
	
	static void ReportMapping( const ICMapEntry& entry )
	{
		short version = entry.version;
		
		//const unsigned char* ext_ension = (const unsigned char*) entry + entry.fixedLength;
		
		ICMapEntryStrings strings = GetStringsFromICMapEntry( entry );
		
		const char* extension      = strings.extension     .c_str();
		const char* creatorAppName = strings.creatorAppName.c_str();
		const char* postAppName    = strings.postAppName   .c_str();
		const char* MIMEType       = strings.MIMEType      .c_str();
		const char* entryName      = strings.entryName     .c_str();
		
		std::printf( "v%d: %s - %s: %s\n", version, extension, creatorAppName, MIMEType );
	}
	
	int Main( int argc, char** argv )
	{
		N::OSType signature = N::OSType( 'Poof' );
		
		n::owned< N::ICInstance > ic = N::ICStart( signature );
		
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

}

