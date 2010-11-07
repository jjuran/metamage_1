/*	==============
 *	ResFile_Dir.cc
 *	==============
 */

#include "Genie/FS/ResFile_Dir.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/resources.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	class FSTree_ResFileDir : public FSTree_Directory
	{
		private:
			FSSpec  itsFileSpec;
			bool    itIsOnServer;
		
		public:
			FSTree_ResFileDir( const FSSpec& file, bool onServer )
			:
				FSTree_Directory( FSTreeFromFSSpec( file, onServer ), "rsrc" ),
				itsFileSpec( file ),
				itIsOnServer( onServer )
			{
			}
			
			bool Exists() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
	};
	
	
	bool FSTree_ResFileDir::Exists() const
	{
		::ResFileRefNum refNum = ::FSpOpenResFile( &itsFileSpec, fsRdPerm );
		
		const bool exists = refNum >= 0;
		
		if ( exists )
		{
			::CloseResFile( refNum );
		}
		else
		{
			const OSErr err = ::ResError();
			
			if ( err != resFNotFound )
			{
				N::ThrowOSStatus( err );
			}
		}
		
		return exists;
	}
	
	static inline bool contains( const char* s, size_t length, char c )
	{
		const char* end = s + length;
		
		return std::find( s, end, c ) != end;
	}
	
	static inline bool is_rsrc_file( const CInfoPBRec&  cInfo,
	                                 ConstStr255Param   name )
	{
		return cInfo.hFileInfo.ioFlLgLen == 0  &&  !contains( (char*) &name[ 1 ],
		                                                      1 + name[0],
		                                                      '.' );
	}
	
	void FSTree_ResFileDir::CreateDirectory( mode_t mode ) const
	{
		CInfoPBRec cInfo = { 0 };
		
		const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo, false, itsFileSpec );
		
		::OSType creator;
		::OSType type;
		
		if ( !exists || is_rsrc_file( cInfo, itsFileSpec.name ) )
		{
			creator = 'RSED';
			type    = 'rsrc';
		}
		else
		{
			const FInfo& fInfo = cInfo.hFileInfo.ioFlFndrInfo;
			
			creator = fInfo.fdCreator;
			type    = fInfo.fdType;
		}
		
		N::FSpCreateResFile( itsFileSpec,
		                     Mac::FSCreator( creator ),
		                     Mac::FSType   ( type    ),
		                     Mac::smSystemScript );
	}
	
	FSTreePtr FSTree_ResFileDir::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		return Get_RsrcFile_FSTree( (parent ? parent : this)->Self(), name, itsFileSpec );
	}
	
	void FSTree_ResFileDir::IterateIntoCache( FSTreeCache& cache ) const
	{
		iterate_resources( itsFileSpec, cache );
	}
	
	
	FSTreePtr Get_ResFileDir_FSTree( const FSSpec& file, bool onServer )
	{
		return seize_ptr( new FSTree_ResFileDir( file, onServer ) );
	}
	
}

