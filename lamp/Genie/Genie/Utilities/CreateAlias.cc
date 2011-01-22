/*
	CreateAlias.cc
	--------------
*/

#include "Genie/Utilities/CreateAlias.hh"

// Nitrogen
#include "Mac/Files/Types/FSSignature.hh"

#include "Nitrogen/Aliases.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static Mac::FSSignature GetFileSignatureForAlias( const FSSpec& item )
	{
		if ( io::directory_exists( item ) )
		{
			return Mac::FSSignature( Mac::FSCreator( 'MACS'                    ),
			                         Mac::FSType   ( kContainerFolderAliasType ) );
		}
		
		const FInfo fInfo = N::FSpGetFInfo( item );
		
		return Mac::FSSignature( fInfo );
	}
	
	void CreateAlias( const FSSpec& aliasFile, const FSSpec& target )
	{
		// This throws if the target doesn't exist
		const Mac::FSSignature signature = GetFileSignatureForAlias( target );
		
		// Aliases get special creator and type
		N::FSpCreateResFile( aliasFile, signature );
		
		n::owned< AliasHandle > alias = N::NewAlias( aliasFile, target );
		
		n::owned< N::ResFileRefNum > aliasResFile = N::FSpOpenResFile( aliasFile, Mac::fsRdWrPerm );
		
		(void) N::AddResource< Mac::rAliasType >( alias, Mac::ResID( 0 ), "\p" );
		
		FInfo linkFInfo = N::FSpGetFInfo( aliasFile );
		
		linkFInfo.fdFlags |= kIsAlias;
		
		N::FSpSetFInfo( aliasFile, linkFInfo );
	}
	
}

