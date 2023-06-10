/*	===========
 *	TextEdit.hh
 *	===========
 */

#ifndef GENIE_FS_TEXTEDIT_HH
#define GENIE_FS_TEXTEDIT_HH

// plus
#include "plus/var_string.hh"

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_TEXTEDIT_TYPES_TEHANDLE_HH
#include "Mac/TextEdit/Types/TEHandle.hh"
#endif

// vfs
#include "vfs/property.hh"

// Pedestal
#ifndef PEDESTAL_TEXTEDIT_HH
#include "Pedestal/TextEdit.hh"
#endif

// Genie
#include "Genie/FS/ScrollerBase.hh"


namespace Genie
{
	
	struct TextEditParameters
	{
		Point                    itsTextDimensions;
		plus::var_string         its_mac_text;
		Pedestal::TextSelection  itsSelection;
		std::size_t              itsValidLength;
		bool                     itHasChangedAttributes;
		bool                     itIsAtBottom;
		bool                     itIsInterlocked;
		bool                     itIsActive;
		bool                     itIsSecret;
		bool                     itIsSingular;
		bool                     itIsWrapped;
		
		TextEditParameters();
		
		static TextEditParameters* Find( const vfs::node* key );
		static TextEditParameters& Get ( const vfs::node* key );
		
		static void Erase( const vfs::node* key );
		
		static bool& Active  ( const vfs::node* view )  { return Get( view ).itIsActive;   }
		static bool& Secret  ( const vfs::node* view )  { return Get( view ).itIsSecret;   }
		static bool& Singular( const vfs::node* view )  { return Get( view ).itIsSingular; }
		static bool& Wrapped ( const vfs::node* view )  { return Get( view ).itIsWrapped;  }
	};
	
	
	struct Selection_Property : vfs::readwrite_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary );
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary );
	};
	
	
	class TextEdit : public Pedestal::TextEdit
	{
		private:
			typedef const vfs::node* Key;
			
			typedef bool (*KeyDown_Hook)( TextEdit&, const EventRecord& );
			
			typedef bool (*UserCommand_Hook)( TextEdit&, Pedestal::CommandCode );
			
			Key itsKey;
			
			KeyDown_Hook      itsKeyDown;
			UserCommand_Hook  itsUserCommand;
			
			nucleus::owned< TEHandle >  itsTE;
			
			Pedestal::TextSelection  itsSelectionPriorToSearch;
			
			bool IsSecret() const;
			bool IsSingular() const;
			
			void On_UserSelect();
			void On_UserEdit();
			
			void UpdateClientHeight();
			void UpdateScrollOffsets();
			
			static void TEClickLoop( TextEdit* that );
			
			void SetActive( bool activating );
			
			void Focus();
			
			void Insert_Key( char c );
			
			void Delete();
			
			void Cut();
			void Paste();
		
		public:
			TextEdit( Key               key,
			          KeyDown_Hook      keyDown = NULL,
			          UserCommand_Hook  cmdHook = NULL )
			:
				itsKey( key ),
				itsKeyDown( keyDown ),
				itsUserCommand( cmdHook )
			{
				itsSelectionPriorToSearch.undefine();
				
				its_TEClickLoop_callback = (TEClickLoop_callback) &TEClickLoop;
			}
			
			void Install( const Rect& bounds );
			void Uninstall();
			
			bool KeyDown( const EventRecord& event );
			
			bool UserCommand( Pedestal::CommandCode code );
			
			const vfs::node* GetKey() const  { return itsKey; }
			
			TEHandle Get() const  { return itsTE; }
			
			void BeginQuasimode();
			void EndQuasimode();
			
			Pedestal::TextSelection GetPriorSelection() const;
			
			void SetPriorSelection( const Pedestal::TextSelection& selection );
			
			bool Wrapped() const;
	};
	
	
	class TextEdit_Scroller : public ScrollerBase
	{
		private:
			TextEdit  itsSubview;
		
		public:
			typedef const vfs::node* Key;
			
			typedef bool (*KeyDown_Hook)( TextEdit&, const EventRecord& );
			
			typedef bool (*UserCommand_Hook)( TextEdit&, Pedestal::CommandCode );
			
			TextEdit_Scroller( Key               key,
			                   KeyDown_Hook      keyDown = NULL,
			                   UserCommand_Hook  cmdHook = NULL )
			:
				ScrollerBase( key ),
				itsSubview( key, keyDown, cmdHook )
			{
			}
			
			Pedestal::View& Subview()  { return itsSubview; }
			
			void SetBounds( const Rect& bounds );
			
			void Scroll( int dh, int dv );
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
	extern const vfs::property_params textedit_flag_params;
	extern const vfs::property_params textedit_scroll_params;
	extern const vfs::property_params textedit_selection_params;
	
}

#endif
