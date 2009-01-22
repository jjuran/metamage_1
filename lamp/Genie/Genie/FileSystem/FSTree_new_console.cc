/*	======================
 *	FSTree_new_console.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_new_console.hh"

// POSIX
#include <sys/ttycom.h>

// Nucleus
#include "Nucleus/Saved.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/Clipboard.hh"
#include "Pedestal/Scroller_beta.hh"
#include "Pedestal/TextEdit.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/Process.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/ScrollerBase.hh"
#include "Genie/IO/DynamicGroup.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/IO/TTY.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	static void RunShellCommand( const std::string& command )
	{
		const char* argv[] = { "-sh", "-c", "", NULL };
		
		const char* envp[] = { "PATH=/bin:/sbin:/usr/bin:/usr/sbin", NULL };
		
		argv[2] = command.c_str();
		
		Process& parent = GetProcess( 1 );
		
		const boost::shared_ptr< Process >& process = NewProcess( parent );
		
		FileDescriptorMap& files = process->FileDescriptors();
		
		files[ 0 ] =
		files[ 1 ] =
		files[ 2 ] = GetSimpleDeviceHandle( "console" );
		
		try
		{
			process->Exec( "/bin/sh", argv, envp );
		}
		catch ( ... )
		{
			process->Exit( 127 );
		}
	}
	
	
	struct ConsoleParameters
	{
		boost::shared_ptr< IOHandle >  itsTerminal;
		Point               itsTextDimensions;
		std::string         itsText;
		Ped::TextSelection  itsSelection;
		std::size_t         itsValidLength;
		std::size_t         itsStartOfInput;
		bool                itHasChangedAttributes;
		bool                itIsWrapped;
		bool                itIsAtBottom;
		bool                itHasReceivedEOF;
		
		ConsoleParameters()
		:
			itsTextDimensions( N::SetPt( 0, 0 ) ),
			itsValidLength(),
			itsStartOfInput(),
			itHasChangedAttributes(),
			itIsWrapped( true ),
			itIsAtBottom(),
			itHasReceivedEOF()
		{
		}
	};
	
	typedef std::map< const FSTree*, ConsoleParameters > ConsoleParametersMap;
	
	static ConsoleParametersMap gConsoleParametersMap;
	
	
	class Console : public Ped::TextEdit
	{
		private:
			typedef const FSTree* Key;
			
			Key itsKey;
			
			NN::Owned< N::TEHandle >  itsTE;
			
			Ped::TextSelection  itsSelectionPriorToSearch;
			
			void On_UserSelect();
			void On_UserEdit();
			
			void On_EnterKey();
			
			void UpdateText();
			
			void UpdateClientHeight();
			void UpdateScrollOffsets();
			
			void ClickInLoop()  { UpdateScrollOffsets(); }
		
		public:
			Console( Key key ) : itsKey( key )
			{
				itsSelectionPriorToSearch.start = -1;
			}
			
			void Install();
			void Uninstall();
			
			TEHandle Get() const  { return itsTE; }
			
			bool KeyDown  ( const EventRecord& event );
			
			boost::shared_ptr< Ped::Quasimode >
			//
			EnterShiftSpaceQuasimode( const EventRecord& event );
			
			void BeginQuasimode();
			void EndQuasimode();
			
			Ped::TextSelection GetPriorSelection() const;
			
			void SetPriorSelection( const Ped::TextSelection& selection );
			
			bool Wrapped() const;
	};
	
	class Console_Scroller : public ScrollerBase
	{
		private:
			Console  itsSubview;
		
		public:
			typedef const FSTree* Key;
			
			Console_Scroller( Key key ) : ScrollerBase( key ), itsSubview( key )
			{
			}
			
			Ped::View& Subview()  { return itsSubview; }
			
			void Scroll( int dh, int dv );
			
			void Draw( const Rect& bounds );
			
			bool UserCommand( Ped::MenuItemCode code );
			
			bool SetCursor( const EventRecord&  event,
			                RgnHandle           mouseRgn );
	};
	
	static bool IsAtBottom( const ScrollerParameters& params )
	{
		const Rect& bounds = params.itsLastViewBounds;
		
		// Not pinned at zero; a negative value works fine below
		short max_voffset = params.itsClientHeight - (bounds.bottom - bounds.top);
		
		return params.itsVOffset >= max_voffset;
	}
	
	void Console_Scroller::Scroll( int dh, int dv )
	{
		N::TEPinScroll( dh, dv, itsSubview.Get() );
		
		const FSTree* key = GetKey();
		
		gConsoleParametersMap[ key ].itIsAtBottom = IsAtBottom( GetScrollerParams( key ) );
	}
	
	static short CountLinesForEditing( const TERec& te )
	{
		short teLength = te.teLength;
		
		// An empty buffer counts as one line
		if ( teLength == 0 )
		{
			return 1;
		}
		
		short    nLines = te.nLines;
		::Handle hText  = te.hText;
		
		// Find the last character in the buffer
		char c = ( *hText )[ teLength - 1 ];
		
		// If the last char is a carriage return, add an extra line
		if ( c == '\r' )
		{
			++nLines;
		}
		
		return nLines;
	}
	
	static short GetTextEditingHeight( TEHandle hTE )
	{
		const TERec& te = **hTE;
		
		return CountLinesForEditing( te ) * te.lineHeight;
	}
	
	static bool Update_TE_From_Model( TEHandle hTE, ConsoleParameters& params )
	{
		bool text_modified = false;
		
		if ( params.itsValidLength < params.itsText.length() )
		{
			text_modified = true;
			
			N::SetHandleSize( hTE[0]->hText, params.itsText.length() );
			
			TERec& te = **hTE;
			
			te.teLength = params.itsText.length();
			
			std::replace_copy( params.itsText.begin() + params.itsValidLength,
			                   params.itsText.end(),
			                   *te.hText + params.itsValidLength,
			                   '\n',
			                   '\r' );
			
			params.itsValidLength = te.teLength;
		}
		else if ( params.itsValidLength < hTE[0]->teLength )
		{
			// Text was merely truncated
			
			text_modified = true;
			
			TERec& te = **hTE;
			
			te.teLength = params.itsValidLength;
			
			N::SetHandleSize( te.hText, params.itsValidLength );
		}
		
		return text_modified;
	}
	
	void Console_Scroller::Draw( const Rect& bounds )
	{
		using Nucleus::operator!=;
		
		ScrollerBase::Draw( bounds );
		
		TEHandle hTE = itsSubview.Get();
		
		ASSERT( hTE != NULL );
		
		const FSTree* key = GetKey();
		
		ScrollerParameters& params = GetScrollerParams( key );
		
		ConsoleParameters& editParams = gConsoleParametersMap[ key ];
		
		const bool text_modified = Update_TE_From_Model( hTE, editParams );
		
		const short viewWidth  = bounds.right - bounds.left;
		const short viewHeight = bounds.bottom - bounds.top;
		
		const bool bounds_changed = bounds != hTE[0]->viewRect;
		
		if ( bounds_changed )
		{
			//params.itsLastViewBounds = bounds;
			
			params.itsClientWidth = viewWidth;
			
			TERec& te = **hTE;
			
			te.viewRect = bounds;
			
			te.destRect = N::OffsetRect( te.viewRect,
			                             -params.itsHOffset,
			                             -params.itsVOffset );
			
			short rows = (bounds.bottom - bounds.top) / te.lineHeight;
			short cols = (bounds.right - bounds.left) / ::CharWidth( 'M' );
			
			editParams.itsTextDimensions = N::SetPt( cols, rows );
		}
		
		if ( bounds_changed || text_modified )
		{
			N::TECalText( hTE );
			
			params.itsClientHeight = GetTextEditingHeight( hTE );
			
			if ( text_modified )
			{
				const short max_voffset = std::max( params.itsClientHeight - viewHeight, 0 );
				
				if ( params.itsVOffset == max_voffset )
				{
					// do nothing
				}
				else if ( params.itsVOffset > max_voffset  ||  editParams.itIsAtBottom )
				{
					params.itsVOffset = max_voffset;
					
					editParams.itHasChangedAttributes = true;
				}
			}
		}
		
		if ( editParams.itHasChangedAttributes )
		{
			TERec& te = **hTE;
			
			// Propagate changes made to 'x' and 'y' files
			te.destRect = N::OffsetRect( te.viewRect,
			                             -params.itsHOffset,
			                             -params.itsVOffset );
			
			te.selStart = editParams.itsSelection.start;
			te.selEnd   = editParams.itsSelection.end;
			
			editParams.itHasChangedAttributes = false;
		}
		
		Subview().Draw( bounds );
	}
	
	bool Console_Scroller::UserCommand( Ped::MenuItemCode code )
	{
		const FSTree* key = GetKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ key ];
		
		switch ( code )
		{
			// Edit
			case 'past':  // kHICommandPaste
			case 'pste':
			case 'clea':
				if ( params.itsSelection.start < params.itsStartOfInput )
				{
					::SysBeep( 30 );
					
					return true;
				}
				
				break;
			
			default:
				break;
		}
		
		return ScrollerBase::UserCommand( code );
	}
	
	bool Console_Scroller::SetCursor( const EventRecord&  event,
	                                  RgnHandle           mouseRgn )
	{
		N::SetCursor( N::GetCursor( N::iBeamCursor ) );
		
		return true;
	}
	
	
	void Console::Install()
	{
		ASSERT( itsTE == NULL );
		
		N::CGrafPtr thePort = N::GetQDGlobalsThePort();
		
		Rect bounds = N::GetPortBounds( thePort );  // will be fixed in next Draw()
		
		itsTE = N::TENew( bounds );
		
		N::TEAutoView( true, itsTE );  // enable auto-scrolling
		
		InstallCustomTEClickLoop( itsTE );
		
		if ( N::GetWindowPort( N::FrontWindow() ) == thePort )
		{
			N::TEActivate( itsTE );
		}
	}
	
	void Console::Uninstall()
	{
		itsTE.reset();
	}
	
	
	void Console::On_UserSelect()
	{
		ConsoleParameters& params = gConsoleParametersMap[ itsKey ];
		
		params.itsSelection = GetCurrentSelection();
		
		UpdateScrollOffsets();
	}
	
	void Console::On_UserEdit()
	{
		UpdateText();
		
		UpdateClientHeight();
		
		On_UserSelect();
	}
	
	void Console::On_EnterKey()
	{
		ConsoleParameters& params = gConsoleParametersMap[ itsKey ];
		
		const std::string& s = params.itsText;
		
		std::string command( s.begin() + params.itsSelection.start,
		                     s.begin() + params.itsSelection.end );
		
		//command += '\n';
		
		RunShellCommand( command );
	}
	
	void Console::UpdateText()
	{
		Ped::TextSelection current = GetCurrentSelection();
		
		if ( current.start != current.end )
		{
			// Destructive operations don't leave a selection
			return;
		}
		
		ASSERT( itsTE != NULL );
		
		std::size_t length = itsTE[0]->teLength;
		
		ConsoleParameters& params = gConsoleParametersMap[ itsKey ];
		
		const Ped::TextSelection& previous = params.itsSelection;
		
		unsigned start = std::min( current.start, previous.start );
		
		start = std::min< unsigned >( params.itsText.length(), start );
		
		params.itsText.resize( length );
		
		const TERec& te = **itsTE;
		
		Handle h = te.hText;
		
		std::replace_copy( *h + start,
		                   *h + length,
		                   params.itsText.begin() + start,
		                   '\r',
		                   '\n' );
		
		params.itsValidLength = length;
	}
	
	void Console::UpdateClientHeight()
	{
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = GetScrollerParams( itsKey );
		
		//const TERec& te = **itsTE;
		
		params.itsClientHeight = GetTextEditingHeight( itsTE );
	}
	
	void Console::UpdateScrollOffsets()
	{
		ASSERT( itsTE != NULL );
		
		ScrollerParameters& params = GetScrollerParams( itsKey );
		
		const TERec& te = **itsTE;
		
		params.itsHOffset = te.viewRect.left - te.destRect.left;
		params.itsVOffset = te.viewRect.top  - te.destRect.top;
		
		gConsoleParametersMap[ itsKey ].itIsAtBottom = IsAtBottom( params );
	}
	
	static void SendSignalToProcessGroupForKey( int signo, const FSTree* key )
	{
		ConsoleParameters& params = gConsoleParametersMap[ key ];
		
		const boost::shared_ptr< IOHandle >& handle = params.itsTerminal;
		
		TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *handle ) );
		
		SendSignalToProcessGroup( signo, *terminal.GetProcessGroup().lock() );
	}
	
	bool Console::KeyDown( const EventRecord& event )
	{
		char c   =  event.message & charCodeMask;
		char key = (event.message & keyCodeMask) >> 8;
		
		ConsoleParameters& params = gConsoleParametersMap[ itsKey ];
		
		Ped::TextSelection& selection = params.itsSelection;
		
		if ( Update_TE_From_Model( itsTE, params )  &&  params.itHasChangedAttributes )
		{
			TERec& te = **itsTE;
			
			te.selStart = selection.start;
			te.selEnd   = selection.end;
		}
		
		if ( c == kEnterCharCode  &&  key >= 0x30 )
		{
			On_EnterKey();
			
			return true;
		}
		
		if ( params.itsSelection.start < params.itsStartOfInput )
		{
			Select( 32767, 32767 );
		}
		
		const UInt32 kEitherControlKey = controlKey | rightControlKey;
		
		if ( event.modifiers & kEitherControlKey  &&  c < 0x20 )
		{
			char cntrl = c | 0x40;
			
			switch ( cntrl )
			{
				case 'A':
					Select( params.itsStartOfInput,
					        params.itsStartOfInput );
					break;
				
				case 'E':
					Select( params.itsText.size(),
					        params.itsText.size() );
					break;
				
				case 'C':
					SendSignalToProcessGroupForKey( SIGINT, itsKey );
					break;
				
				case 'Z':
					SendSignalToProcessGroupForKey( SIGTSTP, itsKey );
					break;
				
				case 'D':
					if ( params.itsText.size() - params.itsStartOfInput <= 0 )
					{
						params.itHasReceivedEOF = true;
					}
					else
					{
						N::SysBeep();
					}
					
					break;
			}
			
			return true;
		}
		
		if ( c == kBackspaceCharCode )
		{
			if ( params.itsSelection.end == params.itsStartOfInput )
			{
				// Eat the event -- don't backspace over the prompt.
				return true;
			}
			
			if ( event.modifiers & cmdKey )
			{
				// Don't delete the prompt.
				Select( params.itsStartOfInput, params.itsSelection.end );
			}
		}
		else if ( c == kReturnCharCode )
		{
			Select( 32767, 32767 );
		}
		else if ( c == kLeftArrowCharCode  &&  params.itsSelection.start == params.itsStartOfInput )
		{
			const bool shift = event.modifiers & (shiftKey | rightShiftKey);
			
			const short cursor = shift ? params.itsSelection.start
			                           : params.itsSelection.end;
			
			if ( cursor == params.itsStartOfInput )
			{
				// Don't retreat cursor past prompt.
				return true;
			}
		}
		
		if ( TextEdit::KeyDown( event ) )
		{
			if ( params.itsSelection.start < params.itsStartOfInput )
			{
				// Fudge 
				Select( params.itsStartOfInput,
				        params.itsStartOfInput );
			}
			
			return true;
		}
		
		return false;
	}
	
	boost::shared_ptr< Ped::Quasimode >
	//
	Console::EnterShiftSpaceQuasimode( const EventRecord& event )
	{
		const bool backward = event.modifiers & shiftKey;
		
		boost::shared_ptr< Ped::Quasimode > mode( new Ped::IncrementalSearchQuasimode( *this, backward ) );
		
		return mode;
	}
	
	static void DrawQuasimodeFrame( Rect frame )
	{
		N::FrameRect( frame );
	}
	
	static void DrawQuasimodeFrame()
	{
		DrawQuasimodeFrame( N::GetPortBounds( N::GetQDGlobalsThePort() ) );
	}
	
	static const RGBColor gRGBBlack = {     0,     0,     0 };
	static const RGBColor gRGBWhite = { 65535, 65535, 65535 };
	
	void Console::BeginQuasimode()
	{
		DrawQuasimodeFrame();
	}
	
	void Console::EndQuasimode()
	{
		N::RGBForeColor( gRGBWhite );
		
		DrawQuasimodeFrame();
		
		N::RGBForeColor( gRGBBlack );
	}
	
	Ped::TextSelection Console::GetPriorSelection() const
	{
		return itsSelectionPriorToSearch;
	}
	
	void Console::SetPriorSelection( const Ped::TextSelection& selection )
	{
		itsSelectionPriorToSearch = selection;
	}
	
	
	bool Console::Wrapped() const
	{
		ConsoleParametersMap::const_iterator it = gConsoleParametersMap.find( itsKey );
		
		if ( it != gConsoleParametersMap.end() )
		{
			return it->second.itIsWrapped;
		}
		
		return true;
	}
	
	boost::shared_ptr< Ped::View > ConsoleFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new Console_Scroller( delegate ) );
	}
	
	
	void FSTree_new_console::DestroyDelegate( const FSTree* delegate )
	{
		RemoveScrollerParams( delegate );
		
		gConsoleParametersMap.erase( delegate );
	}
	
	
	static void TextEditText_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->ParentRef().get();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		params.itsValidLength = std::min< size_t >( params.itsText.length(), length );
		
		params.itsText.resize( length );
		
		InvalidateWindowForView( view );
	}
	
	class ConsoleTextFileHandle : public VirtualFileHandle
	{
		public:
			ConsoleTextFileHandle( const FSTreePtr& file, OpenFlags flags ) : VirtualFileHandle( file, flags )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			const FSTree* ViewKey() const;
			
			std::string& String() const  { return gConsoleParametersMap[ ViewKey() ].itsText; }
			
			ssize_t SysRead( char* buffer, std::size_t byteCount );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t length )  { TextEditText_SetEOF( GetFile().get(), length ); }
	};
	
	boost::shared_ptr< IOHandle > ConsoleTextFileHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new ConsoleTextFileHandle( GetFile(), GetFlags() ) );
	}
	
	const FSTree* ConsoleTextFileHandle::ViewKey() const
	{
		return GetFile()->ParentRef().get();
	}
	
	ssize_t ConsoleTextFileHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		std::string& s = params.itsText;
		
		ASSERT( GetFileMark() <= s.size() );
		
		byteCount = std::min( byteCount, s.size() - GetFileMark() );
		
		std::copy( s.begin() + GetFileMark(),
		           s.begin() + GetFileMark() + byteCount,
		           buffer );
		
		return Advance( byteCount );
	}
	
	ssize_t ConsoleTextFileHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		std::string& s = String();
		
		if ( GetFileMark() + byteCount > s.size() )
		{
			s.resize( GetFileMark() + byteCount );
		}
		
		std::copy( buffer,
		           buffer + byteCount,
		           s.begin() + GetFileMark() );
		
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		params.itsValidLength = std::min< size_t >( params.itsValidLength, GetFileMark() );
		
		InvalidateWindowForView( view );
		
		return Advance( byteCount );
	}
	
	
	static FSTreePtr MakeConsoleProxy( unsigned id )
	{
		FSTreePtr parent = ResolvePathname( "/dev/con" );
		
		std::string name = NN::Convert< std::string >( id );
		
		return FSTreePtr( new FSTree( parent, name ) );
	}
	
	class ConsoleTTYHandle : public TTYHandle
	{
		private:
			FSTreePtr  itsTTYFile;
			unsigned   itsID;
			
			const FSTree* ViewKey() const;
		
		public:
			ConsoleTTYHandle( const FSTreePtr& file, unsigned id )
			:
				TTYHandle( 0 ),
				itsTTYFile( file ),
				itsID( id )
			{
			}
			
			~ConsoleTTYHandle()
			{
				GetDynamicGroup< ConsoleTTYHandle >().erase( itsID );
			}
			
			void Attach( const boost::shared_ptr< IOHandle >& terminal );
			
			FSTreePtr GetFile() const  { return MakeConsoleProxy( itsID ); }
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			void IOCtl( unsigned long request, int* argp );
	};
	
	void ConsoleTTYHandle::Attach( const boost::shared_ptr< IOHandle >& terminal )
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		params.itsTerminal = terminal;
	}
	
	const FSTree* ConsoleTTYHandle::ViewKey() const
	{
		return itsTTYFile->ParentRef().get();
	}
	
	unsigned int ConsoleTTYHandle::SysPoll()
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		std::string& s = params.itsText;
		
		const bool readable = params.itsStartOfInput < s.size()  &&  *s.rbegin() == '\n'  ||  params.itHasReceivedEOF;
		
		int readability = readable ? kPollRead : 0;
		
		return readability | kPollWrite;
	}
	
	ssize_t ConsoleTTYHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		std::string& s = params.itsText;
		
		while ( !params.itHasReceivedEOF  &&  ( params.itsStartOfInput == s.size()  ||  *s.rbegin() != '\n' ) )
		{
			TryAgainLater();
		}
		
		if ( params.itHasReceivedEOF )
		{
			params.itHasReceivedEOF = false;
			
			return 0;
		}
		
		ASSERT( params.itsStartOfInput < s.size() );
		
		byteCount = std::min( byteCount, s.size() - params.itsStartOfInput );
		
		std::copy( s.begin() + params.itsStartOfInput,
		           s.begin() + params.itsStartOfInput + byteCount,
		           buffer );
		
		params.itsStartOfInput += byteCount;
		
		return byteCount;
	}
	
	ssize_t ConsoleTTYHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		std::string& s = params.itsText;
		
		if ( s.size() + byteCount > 30000 )
		{
			s.erase( s.begin(), s.begin() + params.itsStartOfInput );
			
			params.itsStartOfInput = 0;
			params.itsValidLength  = 0;
		}
		
		ASSERT( params.itsStartOfInput <= s.size() );
		
		params.itsValidLength = std::min( params.itsValidLength, params.itsStartOfInput );
		
		s.insert( params.itsStartOfInput, buffer, byteCount );
		
		if ( params.itsSelection.start >= params.itsStartOfInput )
		{
			params.itsSelection.start += byteCount;
			params.itsSelection.end   += byteCount;
		}
		else if ( params.itsSelection.end <= params.itsStartOfInput )
		{
			// preserve selection
		}
		else
		{
			params.itsSelection.start =
			params.itsSelection.end   = s.length();
		}
		
		params.itsStartOfInput += byteCount;
		
		params.itHasChangedAttributes = true;
		
		InvalidateWindowForView( view );
		
		return byteCount;
	}
	
	void ConsoleTTYHandle::IOCtl( unsigned long request, int* argp )
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		Point* result = (Point*) argp;
		
		switch ( request )
		{
			case TIOCGWINSZ:
				if ( result != NULL )
				{
					const Rect& bounds = GetScrollerParams( view ).itsLastViewBounds;
					
					result[0] = params.itsTextDimensions;
					result[1] = N::SetPt( bounds.right - bounds.left, bounds.bottom - bounds.top );
				}
				
				break;
			
			default:
				TTYHandle::IOCtl( request, argp );
				break;
		};
	}
	
	
	class FSTree_Console_tty : public FSTree
	{
		public:
			FSTree_Console_tty( const FSTreePtr&    parent,
			                    const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return ParentRef().get(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			void Rename( const FSTreePtr& destination ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	void FSTree_Console_tty::Rename( const FSTreePtr& destination ) const
	{
		destination->Attach( shared_from_this() );
	}
	
	boost::shared_ptr< IOHandle >
	//
	FSTree_Console_tty::Open( OpenFlags flags ) const
	{
		static unsigned gLastID = 0;
		
		unsigned id = ++gLastID;
		
		boost::shared_ptr< IOHandle > result( new ConsoleTTYHandle( shared_from_this(), id ) );
		
		GetDynamicGroup< ConsoleTTYHandle >()[ id ] = result;
		
		return result;
	}
	
	
	class FSTree_Console_text : public FSTree
	{
		public:
			FSTree_Console_text( const FSTreePtr&    parent,
			                     const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			std::string& String() const  { return gConsoleParametersMap[ ParentRef().get() ].itsText; }
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const  { return String().size(); }
			
			void SetEOF( off_t length ) const  { TextEditText_SetEOF( this, length ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	boost::shared_ptr< IOHandle > FSTree_Console_text::Open( OpenFlags flags ) const
	{
		IOHandle* result = new ConsoleTextFileHandle( shared_from_this(), flags );
		
		return boost::shared_ptr< IOHandle >( result );
	}
	
	
	struct Console_wrapped
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			const FSTree* view = that->ParentRef().get();
			
			const bool wrapped = gConsoleParametersMap[ view ].itIsWrapped;
			
			return wrapped ? "1" : "0";
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end )
		{
			const FSTree* view = that->ParentRef().get();
			
			const bool wrapped = begin[ 0 ] != '0';
			
			gConsoleParametersMap[ view ].itIsWrapped = wrapped;
			
			InvalidateWindowForView( view );
		}
	};
	
	struct Console_Width_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			return NN::Convert< std::string >( GetScrollerParams( view ).itsClientWidth );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end )
		{
			const FSTree* view = GetViewKey( that );
			
			// *end == '\n'
			
			GetScrollerParams( view ).itsClientWidth = std::atoi( begin );
			
			InvalidateWindowForView( view );
		}
	};
	
	struct Console_Height_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			return NN::Convert< std::string >( GetScrollerParams( view ).itsClientHeight );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end )
		{
			const FSTree* view = GetViewKey( that );
			
			// *end == '\n'
			
			GetScrollerParams( view ).itsClientHeight = std::atoi( begin );
			
			InvalidateWindowForView( view );
		}
	};
	
	struct Console_HOffset_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			return NN::Convert< std::string >( GetScrollerParams( view ).itsHOffset );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end )
		{
			const FSTree* view = GetViewKey( that );
			
			// *end == '\n'
			
			GetScrollerParams( view ).itsHOffset = std::atoi( begin );
			
			gConsoleParametersMap[ view ].itHasChangedAttributes = true;
			
			InvalidateWindowForView( view );
		}
	};
	
	struct Console_VOffset_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			return NN::Convert< std::string >( GetScrollerParams( view ).itsVOffset );
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end )
		{
			const FSTree* view = GetViewKey( that );
			
			// *end == '\n'
			
			GetScrollerParams( view ).itsVOffset = std::atoi( begin );
			
			gConsoleParametersMap[ view ].itHasChangedAttributes = true;
			
			InvalidateWindowForView( view );
		}
	};
	
	struct Console_Selection_Property
	{
		static std::string Read( const FSTree* that, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			const Ped::TextSelection& selection = gConsoleParametersMap[ view ].itsSelection;
			
			std::string result = NN::Convert< std::string >( selection.start );
			
			if ( selection.end != selection.start )
			{
				result += '-';
				
				result += NN::Convert< std::string >( selection.end );
			}
			
			return result;
		}
		
		static void Write( const FSTree* that, const char* begin, const char* end )
		{
			const FSTree* view = GetViewKey( that );
			
			ConsoleParameters& params = gConsoleParametersMap[ view ];
			
			std::size_t length = params.itsText.length();
			
			int start;
			int s_end;
			
			if ( end - begin == 1  &&  begin[0] == '-' )
			{
				// A single hyphen means to select the end of the text.
				
				start =
				s_end = length;
			}
			else
			{
				start = std::atoi( begin );
				
				const char* hyphen = std::find( begin, end, '-' );
				
				// If no hyphen is present, select at the given offset.
				// If no number follows the hyphen, use the text length.
				// Otherwise, convert the number and use it.
				
				s_end = hyphen     == end ? start
				      : hyphen + 1 == end ? length
				      :                     std::atoi( hyphen + 1 );
				
				// The selection must not be inverted or exceed the text range.
				
				if ( 0 > start  ||  start > s_end  ||  s_end > length )
				{
					p7::throw_errno( EINVAL );
				}
			}
			
			params.itsSelection.start = start;
			params.itsSelection.end   = s_end;
			
			params.itHasChangedAttributes = true;
			
			InvalidateWindowForView( view );
		}
	};
	
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Read,
		                                       &Property::Write ) );
	}
	
	const FSTree_Premapped::Mapping Console_view_Mappings[] =
	{
		{ "tty", &Basic_Factory< FSTree_Console_tty > },
		
		{ "text", &Basic_Factory< FSTree_Console_text > },
		
		{ "selection", &Property_Factory< Console_Selection_Property > },
		
		//{ "wrapped", &Property_Factory< Console_wrapped > },
		
		// unlocked-text
		
		{ "width",  &Property_Factory< Console_Width_Property  > },
		{ "height", &Property_Factory< Console_Height_Property > },
		
		{ "x", &Property_Factory< Console_HOffset_Property > },
		{ "y", &Property_Factory< Console_VOffset_Property > },
		
		{ NULL, NULL }
	};
	
}

