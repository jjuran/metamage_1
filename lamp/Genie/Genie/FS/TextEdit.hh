/*	===========
 *	TextEdit.hh
 *	===========
 */

#ifndef GENIE_FS_TEXTEDIT_HH
#define GENIE_FS_TEXTEDIT_HH

// plus
#include "plus/var_string.hh"

// nucleus
#include "nucleus/owned.hh"

// Nitrogen
#include "Mac/TextEdit/Types/TEHandle.hh"

// Pedestal
#include "Pedestal/TextEdit.hh"

// Genie
#include "Genie/FS/ScrollerBase.hh"


namespace Genie
{
	
	class FSTree;
	
	
	struct TextEditParameters
	{
		Point                    itsTextDimensions;
		plus::var_string         itsText;
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
		
		static TextEditParameters* Find( const FSTree* key );
		static TextEditParameters& Get ( const FSTree* key );
		
		static void Erase( const FSTree* key );
		
		static bool& Active  ( const FSTree* view )  { return Get( view ).itIsActive;   }
		static bool& Secret  ( const FSTree* view )  { return Get( view ).itIsSecret;   }
		static bool& Singular( const FSTree* view )  { return Get( view ).itIsSingular; }
		static bool& Wrapped ( const FSTree* view )  { return Get( view ).itIsWrapped;  }
	};
	
	
	struct Selection_Property
	{
		static plus::string Get( const FSTree* that, bool binary );
		
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary );
	};
	
	
	class TextEdit : public Pedestal::TextEdit
	{
		private:
			typedef const FSTree* Key;
			
			typedef bool (*KeyDown_Hook)( TextEdit&, const EventRecord& );
			
			typedef bool (*UserCommand_Hook)( TextEdit&, Pedestal::MenuItemCode );
			
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
			
			void ClickInLoop()  { UpdateScrollOffsets(); }
			
			void SetActive( bool activating );
			
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
			}
			
			void Install( const Rect& bounds );
			void Uninstall();
			
			bool KeyDown( const EventRecord& event );
			
			bool UserCommand( Pedestal::MenuItemCode code );
			
			const FSTree* GetKey() const  { return itsKey; }
			
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
			typedef const FSTree* Key;
			
			typedef bool (*KeyDown_Hook)( TextEdit&, const EventRecord& );
			
			typedef bool (*UserCommand_Hook)( TextEdit&, Pedestal::MenuItemCode );
			
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
	
}

#endif

