/*	================
 *	TextEdit_text.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_TEXTEDIT_TEXT_HH
#define GENIE_FILESYSTEM_TEXTEDIT_TEXT_HH

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	class FSTree_TextEdit_text : public FSTree
	{
		public:
			FSTree_TextEdit_text( const FSTreePtr&    parent,
			                      const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const;
			
			void SetEOF( off_t length ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
}

#endif

