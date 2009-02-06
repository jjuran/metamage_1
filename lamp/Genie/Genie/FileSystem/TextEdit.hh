/*	===========
 *	TextEdit.hh
 *	===========
 */

// Pedestal
#include "Pedestal/TextEdit.hh"

// Genie
#include "Genie/FileSystem/ScrollerBase.hh"


namespace Genie
{
	
	class FSTree;
	
	
	struct TextEditParameters
	{
		Point                    itsTextDimensions;
		std::string              itsText;
		Pedestal::TextSelection  itsSelection;
		std::size_t              itsValidLength;
		bool                     itHasChangedAttributes;
		bool                     itIsAtBottom;
		bool                     itIsInterlocked;
		bool                     itIsWrapped;
		
		TextEditParameters();
		
		static TextEditParameters* Find( const FSTree* key );
		static TextEditParameters& Get ( const FSTree* key );
		
		static void Erase( const FSTree* key );
	};
	
	
	struct Selection_Property
	{
		static std::string Get( const FSTree* that, bool binary );
		
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary );
	};
	
	
	class TextEdit : public Pedestal::TextEdit
	{
		private:
			typedef const FSTree* Key;
			
			typedef bool (*KeyDown_Hook)( TextEdit&, const EventRecord& );
			
			Key itsKey;
			
			KeyDown_Hook  itsKeyDown;
			
			Nucleus::Owned< Nitrogen::TEHandle >  itsTE;
			
			Pedestal::TextSelection  itsSelectionPriorToSearch;
			
			void On_UserSelect();
			void On_UserEdit();
			
			void On_EnterKey();
			
			void UpdateText();
			
			void UpdateClientHeight();
			void UpdateScrollOffsets();
			
			void ClickInLoop()  { UpdateScrollOffsets(); }
		
		public:
			TextEdit( Key key, KeyDown_Hook keyDown = NULL )
			:
				itsKey( key ),
				itsKeyDown( keyDown )
			{
				itsSelectionPriorToSearch.start = -1;
			}
			
			void Install();
			void Uninstall();
			
			bool KeyDown( const EventRecord& event );
			
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
			
			TextEdit_Scroller( Key key, KeyDown_Hook keyDown = NULL )
			:
				ScrollerBase( key ),
				itsSubview( key, keyDown )
			{
			}
			
			Pedestal::View& Subview()  { return itsSubview; }
			
			void Scroll( int dh, int dv );
			
			void Draw( const Rect& bounds );
	};
	
	
	bool Update_TE_From_Model( TEHandle hTE, TextEditParameters& params );
	
}

