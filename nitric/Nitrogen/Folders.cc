// Nitrogen/Folders.cc
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Folders.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
  {
   
	Mac::FSDirSpec FindFolder( Mac::FSVolumeRefNum  vRefNum,
	                           FolderType           folderType,
	                           bool                 createFolder )
	{
		::FSVolumeRefNum outVRefNum;
		SInt32 dirID;
		
		ThrowOSStatus( ::FindFolder( vRefNum,
		                             folderType,
		                             createFolder,
		                             &outVRefNum,
		                             &dirID ) );
		
		return nucleus::make< Mac::FSDirSpec >( Mac::FSVolumeRefNum( outVRefNum ), Mac::FSDirID( dirID ) );
	}
	
   FSRef FSFindFolder( Mac::FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder )
     {
      FSRef result;
      ThrowOSStatus( ::FSFindFolder( vRefNum, folderType, createFolder, &result ) );
      return result;
     }
     
  }
