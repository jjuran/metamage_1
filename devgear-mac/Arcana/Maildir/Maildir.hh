/*	==========
 *	Maildir.hh
 *	==========
 */

#pragma once

#include <string>
#include <vector>

// Nitrogen
#include "Nitrogen/Files.hh"


namespace Artifact
{
	
	namespace N = Nitrogen;
	
	bool IsMaildir( const N::FSDirSpec& dir );
	bool IsMaildir( const FSSpec&       dir );
	
	N::FSDirSpec CreateMaildir( const FSSpec& dir );
	
	void SaveMessageToMaildir( const N::FSDirSpec& maildir,
	                           const std::string& filename,
	                           const std::vector< std::string >& lines );
	
	void MarkNewMessageAsRead( const N::FSDirSpec& maildir,
	                           const std::string& filename );
	
}  // namespace Artifact

