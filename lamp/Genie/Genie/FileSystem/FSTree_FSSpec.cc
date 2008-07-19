/*	================
 *	FSTree_FSSpec.cc
 *	================
 */

#include "Genie/FileSystem/FSTree_FSSpec.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <map>
#include <vector>

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// MoreFiles
#include "MoreFilesExtras.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/Aliases.h"
#include "Nitrogen/Files.h"

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen Extras / Iteration
#include "Iteration/FSContents.h"

// Arcana / MD5
#include "MD5.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/FSSpecForkUser.hh"
#include "Genie/FileSystem/FSTree_Dev.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Proc.hh"
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
#include "Genie/FileSystem/FSTree_sys.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/MacFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using namespace io::path_descent_operators;
	
	
	static std::string MacFromUnixName( const std::string& name )
	{
		//ASSERT( name != "."  );
		//ASSERT( name != ".." );
		
		std::string result;
		
		result.resize( name.size() );
		
		std::replace_copy( name.begin(),
		                   name.end(),
		                   result.begin(),
		                   ':',
		                   '/' );
		
		return result;
	}
	
	static std::string UnixFromMacName( const std::string& name )
	{
		std::string result;
		
		result.resize( name.size() );
		
		std::replace_copy( name.begin(),
		                   name.end(),
		                   result.begin(),
		                   '/',
		                   ':' );
		
		return result;
	}
	
	
	static char base32_encode( unsigned x )
	{
		return ( x < 10 ? '0' : 'a' - 10 ) + x;
	}
	
	static FSSpec FSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		std::size_t dot = unixName.find_last_of( "." );
		
		const bool has_dot = dot != unixName.npos;
		
		const unsigned n_delimiters = 1;
		
		const unsigned hash_length = 2;
		
		std::size_t base_length = has_dot ? dot : unixName.size();
		
		std::size_t replaced_length = unixName.size() - 31 + n_delimiters + hash_length;
		
		if ( replaced_length >= base_length )
		{
			p7::throw_errno( ENAMETOOLONG );  // extension is too long
		}
		
		std::size_t shortened_base_length = base_length - replaced_length;
		
		MD5::Result hash = MD5::Digest( unixName.data(), unixName.size() );
		
		std::string macName( unixName.begin(), unixName.begin() + shortened_base_length );
		
		macName += '¥';
		
		macName += base32_encode( hash.data[0] >> 3 );
		macName += base32_encode( hash.data[1] >> 3 );
		
		if ( has_dot )
		{
			macName.append( unixName.begin() + dot, unixName.end() );
		}
		
		std::replace( macName.begin(), macName.end(), ':', '/' );
		
		FSSpec result = parent / macName;
		
		return result;
	}
	
	static FSSpec FSSpecForLongUnixName( const N::FSDirSpec&  parent,
	                                     const std::string&   unixName,
	                                     bool                 forCreation )
	{
		FSSpec result = FSSpecForLongUnixName( parent, unixName );
		
		if ( io::item_exists( result ) == forCreation )
		{
			p7::throw_errno( forCreation ? ENAMETOOLONG : ENOENT );
		}
		
		return result;
	}
	
	inline FSSpec NewFSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		return FSSpecForLongUnixName( parent, unixName, true );
	}
	
	inline FSSpec OldFSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		return FSSpecForLongUnixName( parent, unixName, false );
	}
	
	static bool ItemWithLongNameExists( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec item = FSSpecForLongUnixName( parent, unixName );
		
		return io::item_exists( item );
	}
	
	static void CreateFileWithLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		N::FileSignature sig = PickFileSignatureForName( unixName );
		
		FSSpec file = NewFSSpecForLongUnixName( parent, unixName );
		
		N::FSpCreate( file, sig );
		
		N::FSpDTSetComment( file, unixName );
	}
	
	static void CreateDirectoryWithLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec dir = NewFSSpecForLongUnixName( parent, unixName );
		
		N::FSpDirCreate( dir );
		
		N::FSpDTSetComment( dir, unixName );
	}
	
	
	static std::string GetUnixName( const FSSpec& item )
	{
		if ( item.name[0] == 31 )
		{
			try
			{
				std::string comment = N::FSpDTGetComment( item );
				
				if ( comment.size() > 31 )
				{
					// throws ENOENT if the encoded name doesn't exist
					(void) OldFSSpecForLongUnixName( io::get_preceding_directory( item ), comment );
					
					// Assume it's a Unix name.  FIXME:  Need better heuristics
					return comment;
				}
			}
			catch ( ... )
			{
			}
		}
		
		return UnixFromMacName( io::get_filename_string( item ) );
	}
	
	
	static N::FSVolumeRefNum DetermineVRefNum( ConstStr255Param   name,
	                                           N::FSVolumeRefNum  vRefNum = N::FSVolumeRefNum() )
	{
		::FSVolumeRefNum actualVRefNum;
		N::ThrowOSStatus( ::DetermineVRefNum( name, vRefNum, &actualVRefNum ) );
		
		return N::FSVolumeRefNum( actualVRefNum );
	}
	
	static N::FSVolumeRefNum DetermineVRefNum( const std::string&  name,
	                                           N::FSVolumeRefNum   vRefNum = N::FSVolumeRefNum() )
	{
		return DetermineVRefNum( N::Str27( name ), vRefNum );
	}
	
	
	struct Volume_KeyName_Traits
	{
		typedef N::FSVolumeRefNum Key;
		
		static FSSpec FSSpecFromKey( const Key& key )
		{
			return N::FSMakeFSSpec( key, N::fsRtDirID, "\p" );
		}
		
		static std::string NameFromKey( const Key& key )
		{
			return UnixFromMacName( io::get_filename_string( FSSpecFromKey( key ) ) );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			Key key = DetermineVRefNum( MacFromUnixName( name ) + ":" );
			
			return key;
		}
	};
	
	
	class FSTree_HFS : public FSTree_Mappable
	{
		public:
			FSTree_HFS() : FSTree_Mappable( FSTreePtr() )
			{
				// we override Parent()
			}
			
			bool IsLink() const;
			
			void Stat( struct ::stat& sb ) const;
			
			void ChangeMode( mode_t mode ) const;
			
			void Delete() const;
			
			std::string ReadLink() const;
			FSTreePtr ResolveLink() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			MainEntry GetMainEntry() const;
			
			FSTreePtr Lookup_Regular( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
		
		private:
			virtual void CreateFile() const = 0;
	};
	
	class FSTree_FSSpec : public FSTree_HFS
	{
		private:
			FSSpec itsFileSpec;
		
		public:
			FSTree_FSSpec( const FSSpec& file ) : itsFileSpec( file )
			{
			}
			
			FSTree_FSSpec( const N::FSDirSpec& dir ) : itsFileSpec( NN::Convert< FSSpec >( dir ) )
			{
			}
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			
			std::string Name() const;
			
			FSTreePtr Parent() const;
			
			FSSpec GetFSSpec( bool forCreation ) const;
			
			void CreateFile() const;
			
			void CreateDirectory( mode_t mode ) const;
	};
	
	class FSTree_ConflictingName : public FSTree_FSSpec
	{
		public:
			FSTree_ConflictingName( const FSSpec& file ) : FSTree_FSSpec( file )
			{
			}
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			
			FSSpec GetFSSpec( bool forCreation ) const;
			
			void CreateFile() const;
			
			void CreateDirectory( mode_t mode ) const;
	};
	
	class FSTree_LongName : public FSTree_HFS
	{
		private:
			N::FSDirSpec  itsParent;
			std::string   itsUnixName;
		
		public:
			FSTree_LongName( const N::FSDirSpec&  parent,
			                 const std::string&   name ) : itsParent  ( parent ),
			                                               itsUnixName( name   )
			{
			}
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			
			std::string Name() const;
			
			FSTreePtr Parent() const;
			
			FSSpec GetFSSpec( bool forCreation = false ) const;
			
			void CreateFile() const;
			
			void CreateDirectory( mode_t mode ) const;
	};
	
	
	class FSTree_Volumes_Link;
	
	
	struct Volumes_Details : public Volume_KeyName_Traits
	{
		typedef N::Volume_Container Sequence;
		
		typedef FSTree_Volumes_Link ChildNode;
		
		static std::string Name()  { return "Volumes"; }
		
		static FSTreePtr Parent()  { return FSRoot(); }
		
		static Sequence ItemSequence()  { return N::Volumes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key )
		{
			return true;  // DetermineVRefNum() only returns valid keys
		}
		
		FSTreePtr GetChildNode( const FSTreePtr& parent, const Key& key ) const;
	};
	
	class FSTree_Volumes : public FSTree_Sequence< Volumes_Details >
	{
		private:
			typedef FSTree_Sequence< Volumes_Details > Base;
		
		public:
			FSTree_Volumes( const FSTreePtr&    parent,
			                const std::string&  name ) : Base( parent, name )
			{
			}
			
			void Stat( struct ::stat& sb ) const;
	};
	
	
	static N::FSDirSpec FindJDirectory()
	{
		FSSpec result;
		
		// Try current directory first
		if ( io::directory_exists( result = N::FSDirSpec() / "j" ) )
		{
			return N::FSDirSpec( result );
		}
		
		// Then root, or bust
		result = io::system_root< N::FSDirSpec >() / "j";
		
		return N::FSDirSpec( result );
	}
	
	static N::FSDirSpec UsersDirectory()
	{
		N::FSDirSpec root = io::system_root< N::FSDirSpec >();
		
		FSSpec users = root / "Users";
		
		return N::FSDirSpec( users );
	}
	
	
	class FSTree_J_Symlink : public FSTree
	{
		public:
			FSTree_J_Symlink( const FSTreePtr& parent, const std::string& ) : FSTree( parent )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return NN::Convert< std::string >( NN::Convert< FSSpec >( FindJDirectory() ).name ); }
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_FSSpec( io::get_preceding_directory( FindJDirectory() ) ) ); }
			
			std::string ReadLink() const  { return "/"; }
			
			FSTreePtr ResolveLink() const  { return FSRoot(); }
	};
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item )
	{
		if ( io::directory_exists( item )  &&  N::FSDirSpec( item ) == FindJDirectory() )
		{
			return GetSingleton< FSTree_J_Symlink >();
		}
		
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	
	static const FSTreePtr& MakeFSRoot()
	{
		FSTree_FSSpec* tree = NULL;
		
		static FSTreePtr result = FSTreePtr( tree = new FSTree_FSSpec( FindJDirectory() ) );
		
		if ( tree != NULL )
		{
			FSTreePtr users( new FSTree_FSSpec( io::system_root< N::FSDirSpec >() / "Users" ) );
			
			tree->Map( users );
			
			tree->Map( GetSingleton< FSTree_Volumes >( result, "Volumes" ) );
			
			tree->Map( GetDevFSTree () );
			tree->Map( GetProcFSTree() );
			tree->Map( GetSingleton< FSTree_sys >( result, "sys" ) );
		}
		
		return result;
	}
	
	FSTreePtr FSRoot()
	{
		static const FSTreePtr& root = MakeFSRoot();
		
		return root;
	}
	
	
	static FSNode MakeFSNode_FSSpec( const FSSpec& item )
	{
		std::string name = GetUnixName( item );
		
		const bool isLong = name.size() > 31;
		
		FSTree* ptr = isLong ? static_cast< FSTree* >( new FSTree_LongName( io::get_preceding_directory( item ), name ) )
		                     : static_cast< FSTree* >( new FSTree_FSSpec( item ) );
		
		FSTreePtr tree( ptr );
		
		return FSNode( name, tree );
	}
	
	
	bool FSTree_FSSpec::Exists() const
	{
		return io::item_exists( itsFileSpec );
	}
	
	bool FSTree_FSSpec::IsFile() const
	{
		return io::file_exists( itsFileSpec );
	}
	
	bool FSTree_FSSpec::IsDirectory() const
	{
		return io::directory_exists( itsFileSpec );
	}
	
	bool FSTree_ConflictingName::Exists() const
	{
		return false;
	}
	
	bool FSTree_ConflictingName::IsFile() const
	{
		return false;
	}
	
	bool FSTree_ConflictingName::IsDirectory() const
	{
		return false;
	}
	
	bool FSTree_LongName::Exists() const
	{
		return ItemWithLongNameExists( itsParent, itsUnixName );
	}
	
	bool FSTree_LongName::IsFile() const
	{
		return Exists() && io::file_exists( GetFSSpec() );
	}
	
	bool FSTree_LongName::IsDirectory() const
	{
		return Exists() && io::directory_exists( GetFSSpec() );
	}
	
	bool FSTree_HFS::IsLink() const
	{
		if ( !Exists() )
		{
			return false;
		}
		
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( GetFSSpec(), paramBlock );
		
		const HFileInfo& hFileInfo = paramBlock.hFileInfo;
		
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		bool isAlias = !isDir  &&  hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
		
		return isAlias;
	}
	
	static bool IsRootDirectory( const FSSpec& fileSpec )
	{
		return io::directory_exists( fileSpec )  &&  N::FSDirSpec( fileSpec ) == FindJDirectory();
	}
	
	std::string FSTree_FSSpec::Name() const
	{
		if ( itsFileSpec.parID == fsRtParID )
		{
			return "mnt";
		}
		
		if ( IsRootDirectory( itsFileSpec ) )
		{
			return "";
		}
		
		return io::get_filename_string( itsFileSpec );
	}
	
	FSTreePtr FSTree_FSSpec::Parent() const
	{
		if ( itsFileSpec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( itsFileSpec.vRefNum ) );
		}
		
		if ( io::directory_exists( itsFileSpec ) )
		{
			N::FSDirSpec dir( itsFileSpec );
			
			if ( dir == FindJDirectory() )
			{
				return FSRoot();
			}
			
			try
			{
				if ( dir == UsersDirectory() )
				{
					return FSRoot();
				}
			}
			catch ( ... )
			{
			}
		}
		
		return FSTreePtr( new FSTree_FSSpec( io::get_preceding_directory( itsFileSpec ) ) );
	}
	
	std::string FSTree_LongName::Name() const
	{
		return itsUnixName;
	}
	
	FSTreePtr FSTree_LongName::Parent() const
	{
		return FSTreePtr( new FSTree_FSSpec( itsParent ) );
	}
	
	FSSpec FSTree_FSSpec::GetFSSpec( bool forCreation ) const
	{
		return itsFileSpec;
	}
	
	FSSpec FSTree_ConflictingName::GetFSSpec( bool forCreation ) const
	{
		p7::throw_errno( forCreation ? EEXIST : ENOENT );
		
		return FSSpec();
	}
	
	FSSpec FSTree_LongName::GetFSSpec( bool forCreation ) const
	{
		return FSSpecForLongUnixName( itsParent, itsUnixName, forCreation );
	}
	
	void FSTree_HFS::Stat( struct ::stat& sb ) const
	{
		StatFile( GetFSSpec(), &sb, false );
	}
	
	void FSTree_HFS::ChangeMode( mode_t mode ) const
	{
		ChangeFileMode( GetFSSpec(), mode );
	}
	
	void FSTree_HFS::Delete() const
	{
		N::FSpDelete( GetFSSpec() );
	}
	
	
	std::string FSTree_HFS::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		return ResolveLink()->Pathname();
	}
	
	FSTreePtr FSTree_HFS::ResolveLink() const
	{
		if ( !Exists() )
		{
			return shared_from_this();
		}
		
		FSSpec target = N::ResolveAliasFile( GetFSSpec(), false );
		
		return FSTreePtr( new FSTree_FSSpec( target ) );
	}
	
	void FSTree_FSSpec::CreateFile() const
	{
		// No need to convert name -- for examination only
		std::string name = io::get_filename_string( itsFileSpec );
		
		N::FileSignature sig = PickFileSignatureForName( name );
		
		N::FSpCreate( itsFileSpec, sig );
	}
	
	void FSTree_ConflictingName::CreateFile() const
	{
		p7::throw_errno( EEXIST );
	}
	
	void FSTree_LongName::CreateFile() const
	{
		CreateFileWithLongName( itsParent, itsUnixName );
	}
	
	boost::shared_ptr< IOHandle > FSTree_HFS::Open( OpenFlags flags, mode_t /*mode*/ ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating )
		{
			if ( !Exists() )
			{
				CreateFile();
			}
			else if ( excluding )
			{
				p7::throw_errno( EEXIST );
			}
		}
		
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle > FSTree_HFS::Open( OpenFlags flags ) const
	{
		FSSpec target = N::ResolveAliasFile( GetFSSpec(), true );
		
		return DataForkUser().OpenFileHandle( target, flags );
	}
	
	MainEntry FSTree_HFS::GetMainEntry() const
	{
		return GetMainEntryFromFile( GetFSSpec() );
	}
	
	void FSTree_FSSpec::CreateDirectory( mode_t /*mode*/ ) const
	{
		N::FSpDirCreate( itsFileSpec );
	}
	
	void FSTree_ConflictingName::CreateDirectory( mode_t /*mode*/ ) const
	{
		p7::throw_errno( EEXIST );
	}
	
	void FSTree_LongName::CreateDirectory( mode_t /*mode*/ ) const
	{
		CreateDirectoryWithLongName( itsParent, itsUnixName );
	}
	
	FSTreePtr FSTree_HFS::Lookup_Regular( const std::string& name ) const
	{
		FSSpec target = N::ResolveAliasFile( GetFSSpec(), true );
		
		if ( io::file_exists( target )  &&  name == "rsrc" )
		{
			return GetRsrcForkFSTree( target );
		}
		
		if ( name.size() > 31 )
		{
			return FSTreePtr( new FSTree_LongName( N::FSDirSpec( target ), name ) );
		}
		
		std::string macName = MacFromUnixName( name );
		
		FSSpec item = target / macName;
		
		// The requested name and the returned name may differ in letter case.
		bool matchedCase = std::equal( macName.begin(), macName.end(), item.name + 1 );
		
		return matchedCase ? FSTreeFromFSSpec( item )
		                   : FSTreePtr( new FSTree_ConflictingName( item ) );
	}
	
	void FSTree_HFS::IterateIntoCache( FSTreeCache& cache ) const
	{
		N::FSDirSpec dir( GetFSSpec() );
		
		N::FSSpecContents_Container contents = N::FSContents( dir );
		
		std::transform( contents.begin(),
		                contents.end(),
		                //cache.begin() + 2,
		                std::back_inserter( cache ),
		                std::ptr_fun( MakeFSNode_FSSpec ) );
	}
	
	
	class FSTree_Volumes_Link : public FSTree
	{
		private:
			typedef N::FSVolumeRefNum Key;
			
			Key itsKey;
		
		public:
			FSTree_Volumes_Link( const FSTreePtr&  parent,
			                     const Key&        key ) : FSTree( parent ),
			                                               itsKey( key    )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return io::get_filename_string( N::FSMakeFSSpec( itsKey, N::fsRtDirID, "\p" ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return FSTreePtr( new FSTree_FSSpec( NN::Make< N::FSDirSpec >( itsKey, N::fsRtDirID ) ) ); }
	};
	
	
	FSTreePtr Volumes_Details::GetChildNode( const FSTreePtr& parent, const Key& key ) const
	{
		return MakeFSTree( new FSTree_Volumes_Link( parent, key ) );
	}
	
	void FSTree_Volumes::Stat( struct ::stat& sb ) const
	{
		Base::Stat( sb );
		
		sb.st_ino = fsRtParID;
	}
	
}

