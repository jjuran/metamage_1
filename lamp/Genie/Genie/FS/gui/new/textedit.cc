/*
	gui/new/textedit.cc
	-------------------
*/

#include "Genie/FS/gui/new/textedit.hh"

// POSIX
#include <sys/stat.h>

// plus
#include "plus/serialize.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/property_file.hh"
#include "vfs/node/types/trigger.hh"

// relix-kernel
#include "relix/api/try_again.hh"

// Genie
#include "Genie/FS/TextEdit.hh"
#include "Genie/FS/TextEdit_text.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		return new TextEdit_Scroller( delegate );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		ScrollerParameters::Erase( delegate );
		
		TextEditParameters::Erase( delegate );
	}
	
	
	static void textedit_lock_trigger( const vfs::node* that )
	{
		const vfs::node* view = that->owner();
		
		const bool locked = that->name()[0] != 'u';
		
		TextEditParameters::Get( view ).itIsInterlocked = locked;
	}
	
	
	static
	unsigned texteditgate_poll( vfs::filehandle* that );
	
	static
	ssize_t texteditgate_read( vfs::filehandle* that, char* buffer, size_t n );
	
	static const vfs::stream_method_set texteditgate_stream_methods =
	{
		&texteditgate_poll,
		&texteditgate_read,
	};
	
	static const vfs::filehandle_method_set texteditgate_methods =
	{
		NULL,
		NULL,
		&texteditgate_stream_methods,
	};
	
	
	static
	unsigned texteditgate_poll( vfs::filehandle* that )
	{
		const vfs::node* view = get_file( *that )->owner();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		const bool readable = !params.itIsInterlocked;
		
		return readable * vfs::Poll_read | vfs::Poll_write;
	}
	
	static
	ssize_t texteditgate_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		const vfs::node* view = get_file( *that )->owner();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		while ( params.itIsInterlocked )
		{
			relix::try_again( is_nonblocking( *that ) );
		}
		
		return 0;
	}
	
	
	static vfs::filehandle_ptr textedit_gate_open( const vfs::node* that, int flags, mode_t mode )
	{
		return new vfs::filehandle( that, flags, &texteditgate_methods );
	}
	
	static const vfs::data_method_set textedit_gate_data_methods =
	{
		&textedit_gate_open
	};
	
	static const vfs::node_method_set textedit_gate_methods =
	{
		NULL,
		&textedit_gate_data_methods
	};
	
	
	static vfs::node_ptr gate_factory( const vfs::node*     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return new vfs::node( parent, name, S_IFREG | 0400, &textedit_gate_methods );
	}
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const vfs::node* ) >
	struct TextInvalidating_View_Property : public View_Property< Serialize, Access >
	{
		static void Set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			TextEditParameters::Get( that ).itsValidLength = 0;
			
			View_Property< Serialize, Access >::Set( that, begin, end, binary );
		}
	};
	
	static const vfs::trigger_extra textedit_lock_trigger_extra =
	{
		&textedit_lock_trigger
	};
	
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	typedef Const_View_Property< plus::serialize_bool, TextEditParameters::Active >  Active_Property;
	
	typedef TextInvalidating_View_Property< plus::serialize_bool, TextEditParameters::Secret >  Secret_Property;
	
	typedef View_Property< plus::serialize_bool, TextEditParameters::Singular >  Singular_Property;
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "text", &New_FSTree_TextEdit_text },
		
		{ "lock",   &vfs::trigger_factory, &textedit_lock_trigger_extra },
		{ "unlock", &vfs::trigger_factory, &textedit_lock_trigger_extra },
		
		{ "gate", &gate_factory },
		
		{ "selection", &vfs::new_property, &textedit_selection_params },
		
		{ "active", PROPERTY( Active_Property ) },
		
		{ "secret", PROPERTY( Secret_Property ) },
		
		{ "singular", PROPERTY( Singular_Property ) },
		
		{ "width",  &vfs::new_property, &scroller_setting_params },
		{ "height", &vfs::new_property, &scroller_setting_params },
		
		{ "x", &vfs::new_property, &textedit_scroll_params },
		{ "y", &vfs::new_property, &textedit_scroll_params },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_textedit( const vfs::node*     parent,
	                                       const plus::string&  name,
	                                       const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     &DestroyDelegate );
	}
	
}
