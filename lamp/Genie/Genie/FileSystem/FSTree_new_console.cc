/*	======================
 *	FSTree_new_console.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_new_console.hh"

// POSIX
#include <sys/ttycom.h>

// Genie
#include "Genie/Devices.hh"
#include "Genie/Process.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/TextEdit.hh"
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
		std::size_t         itsStartOfInput;
		bool                itHasReceivedEOF;
		
		ConsoleParameters()
		:
			itsStartOfInput(),
			itHasReceivedEOF()
		{
		}
	};
	
	typedef std::map< const FSTree*, ConsoleParameters > ConsoleParametersMap;
	
	static ConsoleParametersMap gConsoleParametersMap;
	
	
	static bool Console_KeyDown( TextEdit& that, const EventRecord& event );
	
	class Console_Scroller : public TextEdit_Scroller
	{
		public:
			Console_Scroller( Key key )
			:
				TextEdit_Scroller( key, Console_KeyDown )
			{
			}
			
			bool UserCommand( Ped::MenuItemCode code );
	};
	
	bool Console_Scroller::UserCommand( Ped::MenuItemCode code )
	{
		const FSTree* key = GetKey();
		
		TextEditParameters& textParams = TextEditParameters::Get( key );
		
		ConsoleParameters& params = gConsoleParametersMap[ key ];
		
		switch ( code )
		{
			// Edit
			case 'past':  // kHICommandPaste
			case 'pste':
			case 'clea':
				if ( textParams.itsSelection.start < params.itsStartOfInput )
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
	
	
	static void Console_On_EnterKey( TextEditParameters& params )
	{
		const std::string& s = params.itsText;
		
		std::string command( s.begin() + params.itsSelection.start,
		                     s.begin() + params.itsSelection.end );
		
		//command += '\n';
		
		RunShellCommand( command );
	}
	
	static void SendSignalToProcessGroupForKey( int signo, const FSTree* key )
	{
		ConsoleParameters& params = gConsoleParametersMap[ key ];
		
		const boost::shared_ptr< IOHandle >& handle = params.itsTerminal;
		
		TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *handle ) );
		
		SendSignalToProcessGroup( signo, *terminal.GetProcessGroup().lock() );
	}
	
	static bool Try_Control_Character( TextEdit& that, const EventRecord& event )
	{
		const UInt32 kEitherControlKey = controlKey | rightControlKey;
		
		const char c = event.message & charCodeMask;
		
		if ( event.modifiers & kEitherControlKey  &&  c < 0x20 )
		{
			typedef const FSTree* Key;
			
			const Key key = that.GetKey();
			
			const TextEditParameters& params = TextEditParameters::Get( key );
			
			ConsoleParameters& consoleParams = gConsoleParametersMap[ key ];
			
			const char cntrl = c | 0x40;
			
			switch ( cntrl )
			{
				case 'A':
					that.Select( consoleParams.itsStartOfInput,
					             consoleParams.itsStartOfInput );
					break;
				
				case 'E':
					that.Select( params.itsText.size(),
					             params.itsText.size() );
					break;
				
				case 'C':
					SendSignalToProcessGroupForKey( SIGINT, key );
					break;
				
				case 'Z':
					SendSignalToProcessGroupForKey( SIGTSTP, key );
					break;
				
				case 'D':
					if ( params.itsText.size() - consoleParams.itsStartOfInput <= 0 )
					{
						consoleParams.itHasReceivedEOF = true;
					}
					else
					{
						N::SysBeep();
					}
					
					break;
			}
			
			return true;
		}
		
		return false;
	}
	
	static void Console_Postprocess_Key( TextEdit& that )
	{
		const FSTree* key = that.GetKey();
		
		TextEditParameters& params = TextEditParameters::Get( key );
		
		ConsoleParameters& consoleParams = gConsoleParametersMap[ key ];
		
		if ( params.itsSelection.start < consoleParams.itsStartOfInput )
		{
			// Fudge 
			that.Select( consoleParams.itsStartOfInput,
			             consoleParams.itsStartOfInput );
		}
	}
	
	static bool Console_KeyDown( TextEdit& that, const EventRecord& event )
	{
		typedef const FSTree* Key;
		
		const Key viewKey = that.GetKey();
		
		TextEditParameters& params = TextEditParameters::Get( viewKey );
		
		const ConsoleParameters& consoleParams = gConsoleParametersMap[ viewKey ];
		
		Ped::TextSelection& selection = params.itsSelection;
		
		if ( Update_TE_From_Model( that.Get(), params )  &&  params.itHasChangedAttributes )
		{
			if ( params.itsValidLength > 0 )
			{
				// Draw() does additional processing on update that we don't
				// want to skip.  This will minimally invalidate the text,
				// unless it's completely empty.
				--params.itsValidLength;
			}
			
			if ( params.itHasChangedAttributes )
			{
				TERec& te = **that.Get();
				
				te.selStart = selection.start;
				te.selEnd   = selection.end;
				
				// Don't reset itHasChangedAttributes, since we didn't update scroll offsets
			}
		}
		
		const char c   =  event.message & charCodeMask;
		const char key = (event.message & keyCodeMask) >> 8;
		
		if ( c == kEnterCharCode  &&  key >= 0x30 )
		{
			Console_On_EnterKey( params );
			
			return true;
		}
		
		if ( that.Preprocess_Key( event ) )
		{
			return true;
		}
		
		if ( params.itsSelection.start < consoleParams.itsStartOfInput )
		{
			that.Select( 32767, 32767 );
		}
		
		const UInt32 kEitherControlKey = controlKey | rightControlKey;
		
		if ( Try_Control_Character( that, event ) )
		{
			return true;
		}
		
		if ( c == kBackspaceCharCode )
		{
			if ( params.itsSelection.end == consoleParams.itsStartOfInput )
			{
				// Eat the event -- don't backspace over the prompt.
				return true;
			}
			
			if ( event.modifiers & cmdKey )
			{
				// Don't delete the prompt.
				that.Select( consoleParams.itsStartOfInput, params.itsSelection.end );
			}
		}
		else if ( c == kReturnCharCode )
		{
			that.Select( 32767, 32767 );
		}
		else if ( c == kLeftArrowCharCode  &&  params.itsSelection.start == consoleParams.itsStartOfInput )
		{
			const bool shift = event.modifiers & (shiftKey | rightShiftKey);
			
			const short cursor = shift ? params.itsSelection.start
			                           : params.itsSelection.end;
			
			if ( cursor == consoleParams.itsStartOfInput )
			{
				// Don't retreat cursor past prompt.
				return true;
			}
		}
		
		if ( that.Process_Key( event ) )
		{
			Console_Postprocess_Key( that );
			
			return true;
		}
		
		return false;
	}
	
	boost::shared_ptr< Ped::View > ConsoleFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new Console_Scroller( delegate ) );
	}
	
	
	void FSTree_new_console::DestroyDelegate( const FSTree* delegate )
	{
		RemoveScrollerParams( delegate );
		
		TextEditParameters::Erase( delegate );
		
		gConsoleParametersMap.erase( delegate );
	}
	
	
	static void TextEditText_SetEOF( const FSTree* text, off_t length )
	{
		const FSTree* view = text->ParentRef().get();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
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
			
			std::string& String() const  { return TextEditParameters::Get( ViewKey() ).itsText; }
			
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
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
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
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
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
		
		std::string& s = TextEditParameters::Get( view ).itsText;
		
		const bool readable = params.itsStartOfInput < s.size()  &&  *s.rbegin() == '\n'  ||  params.itHasReceivedEOF;
		
		int readability = readable ? kPollRead : 0;
		
		return readability | kPollWrite;
	}
	
	ssize_t ConsoleTTYHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		std::string& s = TextEditParameters::Get( view ).itsText;
		
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
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		ConsoleParameters& consoleParams = gConsoleParametersMap[ view ];
		
		std::string& s = params.itsText;
		
		if ( s.size() + byteCount > 30000 )
		{
			s.erase( s.begin(), s.begin() + consoleParams.itsStartOfInput );
			
			consoleParams.itsStartOfInput = 0;
			params       .itsValidLength  = 0;
		}
		
		ASSERT( consoleParams.itsStartOfInput <= s.size() );
		
		params.itsValidLength = std::min( params.itsValidLength, consoleParams.itsStartOfInput );
		
		s.insert( consoleParams.itsStartOfInput, buffer, byteCount );
		
		if ( params.itsSelection.start >= consoleParams.itsStartOfInput )
		{
			params.itsSelection.start += byteCount;
			params.itsSelection.end   += byteCount;
		}
		else if ( params.itsSelection.end <= consoleParams.itsStartOfInput )
		{
			// preserve selection
		}
		else
		{
			params.itsSelection.start =
			params.itsSelection.end   = s.length();
		}
		
		consoleParams.itsStartOfInput += byteCount;
		
		params.itHasChangedAttributes = true;
		
		InvalidateWindowForView( view );
		
		return byteCount;
	}
	
	void ConsoleTTYHandle::IOCtl( unsigned long request, int* argp )
	{
		const FSTree* view = ViewKey();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
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
			
			std::string& String() const  { return TextEditParameters::Get( ParentRef().get() ).itsText; }
			
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
	
	
	namespace
	{
		
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
	struct Console_View_Property : public View_Property< Scribe, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			TextEditParameters::Get( view ).itHasChangedAttributes = true;
			
			View_Property::Set( that, begin, end, binary );
		}
	};
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Get,
		                                       &Property::Set ) );
	}
	
	const FSTree_Premapped::Mapping Console_view_Mappings[] =
	{
		{ "tty", &Basic_Factory< FSTree_Console_tty > },
		
		{ "text", &Basic_Factory< FSTree_Console_text > },
		
		{ "selection", &Property_Factory< Selection_Property > },
		
		//{ "wrapped", &Property_Factory< View_Property< Boolean_Scribe, Wrapped > > },
		
		// unlocked-text
		
		{ "width",  &Property_Factory< View_Property< Integer_Scribe< int >, Width  > > },
		{ "height", &Property_Factory< View_Property< Integer_Scribe< int >, Height > > },
		
		{ "x", &Property_Factory< Console_View_Property< Integer_Scribe< int >, HOffset > > },
		{ "y", &Property_Factory< Console_View_Property< Integer_Scribe< int >, VOffset > > },
		
		{ NULL, NULL }
	};
	
}

