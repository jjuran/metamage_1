/*	================
 *	RunToolServer.hh
 *	================
 */

// C++ Standard Library
#include <string>


namespace tool
{
	
	/*
	template <>
	struct N::Disposer<FSSpec> {
		void operator()(const FSSpec& item) const {
			// Rationale:
			// When an Owned<FSSpec> is default-constructed, the FSSpec
			// member is initialized to FSSpec().
			// Because struct FSSpec has no defined constructors,
			// FSSpec() will fill the member with zero bytes.
			// This results in a value which, although valid
			// (namely, the default directory on the default volume),
			// does not represent an item that you would ever want to delete,
			// at least not through this mechanism.  Therefore, it's okay
			// to use the zero value as a null-resource indicator.
			// Corrolary:
			// Anything to be deleted as a temporary item should be specified
			// explicitly, and specifying an all-zero value does *not*
			// delete the current directory.  Don't do that.
			if (item == FSSpec())  return;
			Veneer::DeleteItem(item);
		}
	};
	*/
	
	int RunCommandInToolServer( const std::string& command, bool switch_layers );
	
}

