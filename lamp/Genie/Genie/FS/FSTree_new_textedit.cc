/*	======================
 *	FSTree_new_textedit.cc
 *	======================
 */

#include "Genie/FS/FSTree_new_textedit.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/FSTree_sys_window_REF.hh"
#include "Genie/FS/TextEdit.hh"
#include "Genie/FS/TextEdit_text.hh"
#include "Genie/FS/Trigger.hh"
#include "Genie/FS/Views.hh"
#include "Genie/IO/Stream.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	static boost::shared_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new TextEdit_Scroller( delegate ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		RemoveScrollerParams( delegate );
		
		TextEditParameters::Erase( delegate );
	}
	
	
	class FSTree_TextEdit_interlock : public FSTree
	{
		public:
			FSTree_TextEdit_interlock( const FSTreePtr&    parent,
			                           const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			void SetTimes() const;
	};
	
	void FSTree_TextEdit_interlock::SetTimes() const
	{
		const FSTree* view = ParentRef().get();
		
		TextEditParameters::Get( view ).itIsInterlocked = true;
	}
	
	
	class TextEdit_gate_Handle : public VirtualFileHandle< StreamHandle >
	{
		public:
			TextEdit_gate_Handle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
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
			FSTree_TextEdit_gate( const FSTreePtr&    parent,
			                      const std::string&  name ) : Trigger_Base( parent, name )
			{
			}
			
			void Invoke() const;
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	void FSTree_TextEdit_gate::Invoke() const
	{
		const FSTree* view = ParentRef().get();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		params.itIsInterlocked = false;
	}
	
	boost::shared_ptr< IOHandle > FSTree_TextEdit_gate::Open( OpenFlags flags ) const
	{
		if ( (flags & O_ACCMODE) == O_WRONLY )
		{
			return Trigger_Base::Open( flags );
		}
		
		IOHandle* result = new TextEdit_gate_Handle( Self(), flags );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	namespace
	{
		
		bool& Secret( const FSTree* view )
		{
			return TextEditParameters::Get( view ).itIsSecret;
		}
		
		bool& Singular( const FSTree* view )
		{
			return TextEditParameters::Get( view ).itIsSingular;
		}
		
		bool& Wrapped( const FSTree* view )
		{
			return TextEditParameters::Get( view ).itIsWrapped;
		}
		
		int& Width( const FSTree* view )
		{
			return GetScrollerParams( view ).itsClientWidth;
		}
		
		int& Height( const FSTree* view )
		{
			return GetScrollerParams( view ).itsClientHeight;
		}
		
		int& HOffset( const FSTree* view )
		{
			return GetScrollerParams( view ).itsHOffset;
		}
		
		int& VOffset( const FSTree* view )
		{
			return GetScrollerParams( view ).itsVOffset;
		}
		
	}
	
	template < class Scribe, typename Scribe::Value& (*Access)( const FSTree* ) >
	struct TE_View_Property : public View_Property< Scribe, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			TextEditParameters::Get( view ).itHasChangedAttributes = true;
			
			View_Property::Set( that, begin, end, binary );
		}
	};
	
	template < class Scribe, typename Scribe::Value& (*Access)( const FSTree* ) >
	struct TextInvalidating_View_Property : public View_Property< Scribe, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			TextEditParameters::Get( view ).itsValidLength = 0;
			
			View_Property::Set( that, begin, end, binary );
		}
	};
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &Property::Get,
		                            &Property::Set );
	}
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ "text", &New_FSTree_TextEdit_text },
		
		{ "gate", &Basic_Factory< FSTree_TextEdit_gate > },
		
		{ "interlock", &Basic_Factory< FSTree_TextEdit_interlock > },
		
		{ "selection", &Property_Factory< Selection_Property > },
		
		{ "secret", &Property_Factory< TextInvalidating_View_Property< Boolean_Scribe, Secret > > },
		
		{ "singular", &Property_Factory< View_Property< Boolean_Scribe, Singular > > },
		
		//{ "wrapped", &Property_Factory< View_Property< Boolean_Scribe, Wrapped > > },
		
		// unlocked-text
		
		{ "width",  &Property_Factory< View_Property< Integer_Scribe< int >, Width  > > },
		{ "height", &Property_Factory< View_Property< Integer_Scribe< int >, Height > > },
		
		{ "x", &Property_Factory< TE_View_Property< Integer_Scribe< int >, HOffset > > },
		{ "y", &Property_Factory< TE_View_Property< Integer_Scribe< int >, VOffset > > },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_textedit( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

