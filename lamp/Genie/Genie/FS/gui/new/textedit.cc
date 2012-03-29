/*
	gui/new/textedit.cc
	-------------------
*/

#include "Genie/FS/gui/new/textedit.hh"

// plus
#include "plus/serialize.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/TextEdit.hh"
#include "Genie/FS/TextEdit_text.hh"
#include "Genie/FS/Trigger.hh"
#include "Genie/FS/Views.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return new TextEdit_Scroller( delegate );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		ScrollerParameters::Erase( delegate );
		
		TextEditParameters::Erase( delegate );
	}
	
	
	static void textedit_lock_trigger( const FSTree* node )
	{
		const FSTree* view = node->owner();
		
		const bool locked = node->name()[0] != 'u';
		
		TextEditParameters::Get( view ).itIsInterlocked = locked;
	}
	
	
	class TextEdit_gate_Handle : public VirtualFileHandle< StreamHandle >
	{
		public:
			TextEdit_gate_Handle( const FSTreePtr& file, OpenFlags flags )
			:
				VirtualFileHandle< StreamHandle >( file, flags )
			{
			}
			
			unsigned SysPoll();
			
			ssize_t SysRead( char* buffer, size_t n_bytes );
	};
	
	unsigned TextEdit_gate_Handle::SysPoll()
	{
		const FSTree* view = GetFile()->ParentRef().get();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		const bool readable = !params.itIsInterlocked;
		
		return readable * kPollRead | kPollWrite;
	}
	
	ssize_t TextEdit_gate_Handle::SysRead( char* buffer, size_t n_bytes )
	{
		const FSTree* view = GetFile()->ParentRef().get();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		while ( params.itIsInterlocked )
		{
			TryAgainLater();
		}
		
		return 0;
	}
	
	
	class FSTree_TextEdit_gate : public Trigger_Base
	{
		public:
			FSTree_TextEdit_gate( const FSTreePtr&     parent,
			                      const plus::string&  name )
			:
				Trigger_Base( parent, name, S_IFREG | 0600 )
			{
			}
			
			void Invoke() const;
	};
	
	void FSTree_TextEdit_gate::Invoke() const
	{
		const FSTree* view = ParentRef().get();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		params.itIsInterlocked = false;
	}
	
	static IOPtr textedit_gate_open( const FSTree* node, OpenFlags flags, mode_t mode )
	{
		return new TextEdit_gate_Handle( node, flags );
	}
	
	static const data_method_set textedit_gate_data_methods =
	{
		&textedit_gate_open
	};
	
	static const node_method_set textedit_gate_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&textedit_gate_data_methods
	};
	
	
	static FSTreePtr gate_factory( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		return new FSTree( parent, name, S_IFREG | 0400, &textedit_gate_methods );
	}
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const FSTree* ) >
	struct TE_View_Property : public View_Property< Serialize, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			TextEditParameters::Get( that ).itHasChangedAttributes = true;
			
			View_Property< Serialize, Access >::Set( that, begin, end, binary );
		}
	};
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const FSTree* ) >
	struct TextInvalidating_View_Property : public View_Property< Serialize, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			TextEditParameters::Get( that ).itsValidLength = 0;
			
			View_Property< Serialize, Access >::Set( that, begin, end, binary );
		}
	};
	
	static const trigger_extra textedit_lock_trigger_extra =
	{
		&textedit_lock_trigger
	};
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	typedef Const_View_Property< plus::serialize_bool, TextEditParameters::Active >  Active_Property;
	
	typedef TextInvalidating_View_Property< plus::serialize_bool, TextEditParameters::Secret >  Secret_Property;
	
	typedef View_Property< plus::serialize_bool, TextEditParameters::Singular >  Singular_Property;
	typedef View_Property< plus::serialize_bool, TextEditParameters::Wrapped  >  Wrapped_Property;
	
	typedef View_Property< plus::serialize_int< int >, ScrollerParameters::Width  >  Width_Property;
	typedef View_Property< plus::serialize_int< int >, ScrollerParameters::Height >  Height_Property;
	
	typedef TE_View_Property< plus::serialize_int< int >, ScrollerParameters::HOffset >  HOffset_Property;
	typedef TE_View_Property< plus::serialize_int< int >, ScrollerParameters::VOffset >  VOffset_Property;
	
	static const premapped::mapping local_mappings[] =
	{
		{ "text", &New_FSTree_TextEdit_text },
		
		{ "lock",   &trigger_factory, &textedit_lock_trigger_extra },
		{ "unlock", &trigger_factory, &textedit_lock_trigger_extra },
		
		{ "gate", &gate_factory },
		
		{ "selection", PROPERTY( Selection_Property ) },
		
		{ "active", PROPERTY( Active_Property ) },
		
		{ "secret", PROPERTY( Secret_Property ) },
		
		{ "singular", PROPERTY( Singular_Property ) },
		
		//{ "wrapped", PROPERTY( Wrapped_Property ) },
		
		// unlocked-text
		
		{ "width",  PROPERTY( Width_Property  ) },
		{ "height", PROPERTY( Height_Property ) },
		
		{ "x", PROPERTY( HOffset_Property ) },
		{ "y", PROPERTY( VOffset_Property ) },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_textedit( const FSTreePtr&     parent,
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

