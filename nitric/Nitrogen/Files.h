// Files.h

#ifndef NITROGEN_FILES_H
#define NITROGEN_FILES_H

#ifndef __FILES__
#include <Files.h>
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_CFSTRING_H
#include "Nitrogen/CFString.h"
#endif
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_INDEXUNTILFAILURECONTAINER_H
#include "Nitrogen/IndexUntilFailureContainer.h"
#endif
#ifndef NITROGEN_INDEXTYPE_H
#include "Nitrogen/IndexType.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   void RegisterFileManagerErrors();

   class FSDirIDTag {};
   typedef IDType< FSDirIDTag, UInt32, 0 > FSDirID;
   
   class FSNodeFlagsTag {};
   typedef FlagType< FSNodeFlagsTag, UInt16, 0 > FSNodeFlags;
   
   class FSNodeIDTag {};
   typedef IDType< FSNodeIDTag, UInt32, 0 > FSNodeID;
   
   class FSVolumeRefNumTag {};
   typedef IDType< FSVolumeRefNumTag, ::FSVolumeRefNum, 0 > FSVolumeRefNum;
   
   class FSSharingFlagsTag {};
   typedef FlagType< FSSharingFlagsTag, UInt8, 0 > FSSharingFlags;
   
   class FSUserPrivilegesTag {};
   typedef FlagType< FSUserPrivilegesTag, UInt8, 0 > FSUserPrivileges;
   typedef FSUserPrivileges FSIOACUser;

   class FSIteratorFlagsTag {};
   typedef FlagType< FSIteratorFlagsTag, ::FSIteratorFlags, 0 > FSIteratorFlags;
   
   class FSForkRefNumTag {};
   typedef IDType< FSForkRefNumTag, SInt16, 0 > FSForkRefNum;
    
   class FSIOPermssnTag {};
   typedef IDType< FSIOPermssnTag, SInt8, fsCurPerm > FSIOPermssn;
   typedef FSIOPermssn FSIOPermissions;
    
   class FSIOPosModeTag {};
   typedef SelectorType< FSIOPosModeTag, UInt16, fsAtMark > FSIOPosMode;
   typedef FSIOPosMode FSIOPositioningMode;

   class FSAllocationFlagsTag {};
   typedef FlagType< FSAllocationFlagsTag, ::FSAllocationFlags, 0 > FSAllocationFlags;
  
   class FSForkIteratorTag {};
   typedef IDType< FSForkIteratorTag, SInt16, 0 > FSForkIterator;

   class FSVolumeInfoFlagsTag {};
   typedef FlagType< FSVolumeInfoFlagsTag, UInt16, 0 > FSVolumeInfoFlags;
   typedef FSVolumeInfoFlags FSIOVAtrb;
   typedef FSVolumeInfoFlags FSIOVolumeAttributes;
   
   struct FSVolumeIndex_Specifics
     {
      typedef ::ItemCount UnderlyingType;
      static const UnderlyingType defaultValue = 0;
     };

   typedef IndexType< FSVolumeIndex_Specifics > FSVolumeIndex;
   
   class FSFileSystemIDTag {};
   typedef IDType< FSFileSystemIDTag, UInt16, 0 > FSFileSystemID;
   typedef FSFileSystemID IOFSID;
   
   class DriverReferenceNumberTag {};
   typedef IDType< DriverReferenceNumberTag, SInt16, 0 > DriverReferenceNumber;
   typedef DriverReferenceNumber DRefNum;
   typedef DriverReferenceNumber DriverRefNum;

   class HFSCatalogNodeIDTag {};
   typedef IDType< HFSCatalogNodeIDTag, UInt32, 0 > HFSCatalogNodeID;


   template <> struct Converter< Owned<CFStringRef>, HFSUniStr255 >: public std::unary_function< HFSUniStr255, Owned<CFStringRef> >
     {
      Owned<CFStringRef> operator()( const HFSUniStr255& in ) const
        {
         return CFStringCreateWithCharacters( in.unicode, in.length );
        }
     };
   
   template <> struct Converter< HFSUniStr255, CFStringRef >: public std::unary_function< CFStringRef, HFSUniStr255 >
     {
      HFSUniStr255 operator()( CFStringRef in ) const
        {
         CFIndex length = Nitrogen::CFStringGetLength( in );
         if ( length > 255 )
            throw StringTooLong();
         HFSUniStr255 result;
         result.length = Convert<UInt16>( length );
         Nitrogen::CFStringGetCharacters( in, CFRangeMake( 0, length ), result.unicode );
         return result;
        }
     };

   template <> struct Converter< UniString, HFSUniStr255 >: public std::unary_function< HFSUniStr255, UniString >
     {
      UniString operator()( const HFSUniStr255& in ) const
        {
         return UniString( in.unicode, in.unicode+in.length );
        }
     };
  
   template <> struct Converter< HFSUniStr255, UniString >: public std::unary_function< UniString, HFSUniStr255 >
     {
      HFSUniStr255 operator()( const UniString& in ) const
        {
         if ( in.size() > 255 )
            throw StringTooLong();
         HFSUniStr255 result;
         result.length = Convert<UInt16>( in.size() );
         std::copy( in.begin(), in.end(), result.unicode );
         return result;
        }
     };
   
   FSRef FSpMakeFSRef( const FSSpec& );

   template <> struct Converter< FSRef, FSSpec >: public std::unary_function< FSSpec, FSRef >
     {
      FSRef operator()( const FSSpec& spec ) const
        {
         return FSpMakeFSRef( spec );
        }
     };

   FSRef FSMakeFSRefUnicode( const FSRef&   parentRef,
                             UniCharCount   nameLength,
                             const UniChar *name,
                             TextEncoding   textEncodingHint );

   inline FSRef FSMakeFSRefUnicode( const FSRef&        parentRef,
                                    const HFSUniStr255& name,
                                    TextEncoding        textEncodingHint )
     {
      return FSMakeFSRefUnicode( parentRef, name.length, name.unicode, textEncodingHint );
     }

   FSRef FSMakeFSRefUnicode( const FSRef&     parentRef,
                             const UniString& name,
                             TextEncoding     textEncodingHint );
   
   bool FSCompareFSRefs( const FSRef& ref1, const FSRef& ref2 );

   inline bool operator==( const FSRef& ref1, const FSRef& ref2 )   { return FSCompareFSRefs( ref1, ref2 ); }
   inline bool operator!=( const FSRef& ref1, const FSRef& ref2 )   { return !( ref1 == ref2 ); }


   struct FSRefSpec
     {
      FSRef fsRef;
      FSSpec fsSpec;
      
      operator const FSRef&() const   { return fsRef; }
      operator const FSSpec&() const  { return fsSpec; }
     };

   struct FSRefSpecDirID: public FSRefSpec
     {
      FSDirID dirID;

      operator FSDirID() const          { return dirID; }
     };

   class FileSystemDisposer: private DefaultDestructionOSStatusPolicy
     {
      public:
         void operator()( const FSRef& ) const;
         void operator()( const FSSpec& ) const;
         void operator()( const FSRefSpec& r ) const          { operator()( r.fsRef ); }
     };
   
   template <> struct OwnedDefaults< FSRef >           { typedef FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< FSSpec >          { typedef FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< FSDirID >         { typedef FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< FSRefSpec >       { typedef FileSystemDisposer Disposer; };
   template <> struct OwnedDefaults< FSRefSpecDirID >  { typedef FileSystemDisposer Disposer; };
   
   template <> struct LivelinessTraits< FSRef,          FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
   template <> struct LivelinessTraits< FSRefSpec,      FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };
   template <> struct LivelinessTraits< FSRefSpecDirID, FileSystemDisposer >   { typedef SeizedValuesAreLive LivelinessTest; };


   typedef Owned<FSRefSpec> FSCreateFileUnicode_Result;
   
   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   UniCharCount         nameLength,
                                                   const UniChar *      name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&   parentRef,
                                                   UniCharCount   nameLength,
                                                   const UniChar *name );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&         parentRef,
                                                   const UniString&     name,
                                                   FSCatalogInfoBitmap  whichInfo,
                                                   const FSCatalogInfo& catalogInfo );

   FSCreateFileUnicode_Result FSCreateFileUnicode( const FSRef&     parentRef,
                                                   const UniString& name );
   
   typedef Owned<FSRefSpecDirID> FSCreateDirectoryUnicode_Result;
   
   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             UniCharCount         nameLength,
                                                             const UniChar *      name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo );

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&   parentRef,
                                                             UniCharCount   nameLength,
                                                             const UniChar *name );

   inline FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&        parentRef,
                                                                    const HFSUniStr255& name )
     {
      return FSCreateDirectoryUnicode( parentRef, name.length, name.unicode );
     }
   
   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&         parentRef,
                                                             const UniString&     name,
                                                             FSCatalogInfoBitmap  whichInfo,
                                                             const FSCatalogInfo& catalogInfo );

   FSCreateDirectoryUnicode_Result FSCreateDirectoryUnicode( const FSRef&     parentRef,
                                                             const UniString& name );

   void FSDeleteObject( Owned<FSRef> );

   void FSMoveObject( Owned<FSRef>&, const FSRef& destDirectory );

   void FSExchangeObjects( const FSRef& ref, const FSRef& destRef );
   
   void FSRenameUnicode( Owned<FSRef>&  ref,
                         UniCharCount   nameLength,
                         const UniChar *name,
                         TextEncoding   textEncodingHint );

   void FSRenameUnicode( Owned<FSRef>&    ref,
                         const UniString& name,
                         TextEncoding     textEncodingHint );

   struct FSGetCatalogInfo_Result
     {
      FSCatalogInfo catalogInfo;
      HFSUniStr255  outName;
      FSSpec        fsSpec;
      FSRef         parentRef;
     };
   
   struct ForkSizes
     {
      UInt64 logicalSize;
      UInt64 physicalSize;
     };
   
   union FinderInfo
     {
      FileInfo   fileInfo;
      FolderInfo folderInfo;
      UInt8      finderInfo[16];
      
      FinderInfo()                                     { fileInfo = FileInfo(); }
      FinderInfo( const FileInfo& info )               { fileInfo = info; }
      FinderInfo( const FolderInfo& info )             { folderInfo = info; }
         
      FinderInfo& operator=( const FileInfo& info )    { fileInfo = info;   return *this; } 
      FinderInfo& operator=( const FolderInfo& info )  { folderInfo = info; return *this; }
      
      operator const FileInfo&() const                 { return fileInfo; }
      operator const FolderInfo&() const               { return folderInfo; }
      
      operator FileInfo&()                             { return fileInfo; }
      operator FolderInfo&()                           { return folderInfo; }
     };

   union ExtFinderInfo
     {
      ExtendedFileInfo   extFileInfo;
      ExtendedFolderInfo extFolderInfo;
      UInt8              extFinderInfo[16];

      ExtFinderInfo()                                             { extFileInfo = ExtendedFileInfo(); }
      ExtFinderInfo( const ExtendedFileInfo& info )               { extFileInfo = info; }
      ExtFinderInfo( const ExtendedFolderInfo& info )             { extFolderInfo = info; }
         
      ExtFinderInfo& operator=( const ExtendedFileInfo& info )    { extFileInfo = info;   return *this; } 
      ExtFinderInfo& operator=( const ExtendedFolderInfo& info )  { extFolderInfo = info; return *this; }
      
      operator const ExtendedFileInfo&() const                    { return extFileInfo; }
      operator const ExtendedFolderInfo&() const                  { return extFolderInfo; }
      
      operator ExtendedFileInfo&()                                { return extFileInfo; }
      operator ExtendedFolderInfo&()                              { return extFolderInfo; }
     };
   
   struct Permissions
     {
      UInt32 permissions[4];
     };
   
   template < class DesiredType, class MemberType, MemberType FSCatalogInfo::*member >
   struct Basic_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSCatalogInfo& info )                { return info.*member; }
      static void Set( FSCatalogInfo& info, const Type& value )   { info.*member = value; }
     };

   template < class DesiredType, class MemberType, MemberType FSCatalogInfo::*member >
   struct Const_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSCatalogInfo& info )                { return info.*member; }
     };

   
   template < UInt64 FSCatalogInfo::*logical, UInt64 FSCatalogInfo::*physical >
   struct ForkSizes_FSCatalogInfoBit_Traits
     {
      typedef ForkSizes Type;
      
      static Type Get( const FSCatalogInfo& info )
        {
         ForkSizes result;
         result.logicalSize  = info.*logical;
         result.physicalSize = info.*physical;
         return result;
        }
     };

   template < class DesiredType,
              class MemberType,
              std::size_t size,
              MemberType (FSCatalogInfo::*infoMember)[size],
              MemberType (DesiredType::*desiredMember)[size] >
   struct Array_FSCatalogInfoBit_Traits
     {
      typedef DesiredType Type;
      
      static Type Get( const FSCatalogInfo& info )
        {
         DesiredType result;
         for ( std::size_t i = 0; i < size; ++i )
            (result.*desiredMember)[i] = (info.*infoMember)[i];
         return result;
        }
      
      static void Set( FSCatalogInfo& info, const Type& value )
        {
         for ( std::size_t i = 0; i < size; ++i )
            (info.*infoMember)[i] = (value.*desiredMember)[i];
        }
     };
   
   
   template < ::FSCatalogInfoBitmap bit > struct FSCatalogInfoBit_Traits;
   
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoTextEncoding > : Basic_FSCatalogInfoBit_Traits< TextEncoding,     ::TextEncoding,   &FSCatalogInfo::textEncodingHint > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoNodeFlags    > : Basic_FSCatalogInfoBit_Traits< FSNodeFlags,      ::UInt16,         &FSCatalogInfo::nodeFlags        > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoVolume       > : Const_FSCatalogInfoBit_Traits< FSVolumeRefNum,   ::FSVolumeRefNum, &FSCatalogInfo::volume           > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoParentDirID  > : Const_FSCatalogInfoBit_Traits< FSDirID,          ::UInt32,         &FSCatalogInfo::parentDirID      > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoNodeID       > : Const_FSCatalogInfoBit_Traits< FSNodeID,         ::UInt32,         &FSCatalogInfo::nodeID           > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoCreateDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::createDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoContentMod   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::contentModDate   > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoAttrMod      > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::attributeModDate > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoAccessDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::accessDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoBackupDate   > : Basic_FSCatalogInfoBit_Traits< UTCDateTime,      ::UTCDateTime,    &FSCatalogInfo::backupDate       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoValence      > : Const_FSCatalogInfoBit_Traits< UInt32,           ::UInt32,         &FSCatalogInfo::valence          > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoSharingFlags > : Const_FSCatalogInfoBit_Traits< FSSharingFlags,   ::UInt8,          &FSCatalogInfo::sharingFlags     > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoUserPrivs    > : Const_FSCatalogInfoBit_Traits< FSUserPrivileges, ::UInt8,          &FSCatalogInfo::userPrivileges   > {};

   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoDataSizes > : ForkSizes_FSCatalogInfoBit_Traits< &FSCatalogInfo::dataLogicalSize, &FSCatalogInfo::dataPhysicalSize > {}; 
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoRsrcSizes > : ForkSizes_FSCatalogInfoBit_Traits< &FSCatalogInfo::rsrcLogicalSize, &FSCatalogInfo::rsrcPhysicalSize > {};

   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoPermissions  > : Array_FSCatalogInfoBit_Traits< Permissions,   ::UInt32, 4, &FSCatalogInfo::permissions,   &Permissions::permissions     > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoFinderInfo   > : Array_FSCatalogInfoBit_Traits< FinderInfo,    ::UInt8, 16, &FSCatalogInfo::finderInfo,    &FinderInfo::finderInfo       > {};
   template <> struct FSCatalogInfoBit_Traits< kFSCatInfoFinderXInfo  > : Array_FSCatalogInfoBit_Traits< ExtFinderInfo, ::UInt8, 16, &FSCatalogInfo::extFinderInfo, &ExtFinderInfo::extFinderInfo > {};

   void FSGetCatalogInfo( const FSRef&        ref,
                          FSCatalogInfoBitmap whichInfo,
                          FSCatalogInfo *     catalogInfo,
                          HFSUniStr255 *      outName,
                          FSSpec *            fsSpec,
                          FSRef *             parentRef );
   
   FSGetCatalogInfo_Result FSGetCatalogInfo( const FSRef& ref, FSCatalogInfoBitmap whichInfo );
   
   template< ::FSCatalogInfoBitmap bit >
   struct FSGetCatalogInfo_Traits
     {
      typedef typename FSCatalogInfoBit_Traits<bit>::Type Result;
     };
   
   template< ::FSCatalogInfoBitmap bit >
   typename FSGetCatalogInfo_Traits<bit>::Result FSGetCatalogInfo( const FSRef& ref )
     {
      FSCatalogInfo info;
      FSGetCatalogInfo( ref, bit, &info, 0, 0, 0 );
      return FSCatalogInfoBit_Traits<bit>::Get( info );
     }

   void FSSetCatalogInfo( const FSRef&         ref,
                          FSCatalogInfoBitmap  whichInfo,
                          const FSCatalogInfo& catalogInfo );
   
   
   template <> struct Converter< FSSpec, FSRef >: public std::unary_function< FSRef, FSSpec >
     {
      FSSpec operator()( const FSRef& ref ) const
        {
         FSSpec result;
         FSGetCatalogInfo( ref, 0, 0, 0, &result, 0 );
         return result;
        }
     };


   template< ::FSCatalogInfoBitmap bit >
   struct FSSetCatalogInfo_Traits
     {
      typedef const typename FSCatalogInfoBit_Traits<bit>::Type& ParameterType;
     };

   template< ::FSCatalogInfoBitmap bit >
   void FSSetCatalogInfo( const FSRef& ref, typename FSSetCatalogInfo_Traits<bit>::ParameterType value )
     {
      FSCatalogInfo info; 
      FSCatalogInfoBit_Traits<bit>::Set( info, value );
      FSSetCatalogInfo( ref, bit, info );
     }

   template <> struct Disposer<FSIterator>: public std::unary_function< FSIterator, void >,
                                            private DefaultDestructionOSStatusPolicy
     {
      void operator()( FSIterator iterator ) const
        {
         OnlyOnce<RegisterFileManagerErrors>();
         DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::FSCloseIterator( iterator ) );
        }
     };

   Owned<FSIterator> FSOpenIterator( const FSRef& container, FSIteratorFlags iteratorFlags );
    
   void FSCloseIterator( Owned<FSIterator> );

#if 0
   template < class Specifics >
   class FSIteratedContainer: private Specifics
     {
      private:
          FSRef root;
         
         // not implemented:
            FSIteratedContainer& operator=( const FSIteratedContainer& );

      public:
         typedef typename Specifics::value_type value_type;
         typedef typename Specifics::size_type size_type;
         typedef typename Specifics::difference_type difference_type;

         typedef value_type& reference;
         typedef const value_type& const_reference;
         
         class const_iterator
           {
            friend class FSIteratedContainer;
 
            public:
               typedef IndexUntilFailureContainer::difference_type difference_type;
               typedef IndexUntilFailureContainer::value_type value_type;
               typedef const value_type *pointer;
               typedef const value_type& reference;
               typedef std::input_iterator_tag iterator_category;
           
            private:
               Shared<FSIterator> fsIterator;
               value_type value;
               
               void GetValue()
                 {
                  FSGetCatalogInfoBulk_Result infoResult;
                  do
                     infoResult = Specifics::GetValue( fsIterator, &value );
                  while ( infoResult.actualObjects == 0 && !infoResult.noMoreItems );
                  if ( infoResult.noMoreItems )
                     fsIterator = Shared<FSIterator>();
                 }
               
               const_iterator( const FSRef& root, FSIteratorFlags flags )
                 : fsIterator( FSOpenIterator( root, flags ) )
                 {
                  GetValue();
                 }
               
            public:
               const_iterator()                          {}
               
               const_iterator& operator++()              { GetValue(); return *this; }
               const_iterator operator++(int)            { GetValue(); return *this; }
               
               reference operator*() const               { return value; }
               pointer operator->() const                { return &value; }
               
               friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.fsIterator == b.fsIterator; }
               friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
           };
      
         FSIteratedContainer( const Specifics& base )
           : Specifics( base )
           {}
         
         const_iterator begin() const                    { return const_iterator( *this, Specifics::begin_position() ); }
         const_iterator end() const                      { return const_iterator(); }
     };
#endif
   
   struct FSGetCatalogInfoBulk_Result
     {
      ItemCount actualObjects;
      bool      noMoreItems;
     };
   
   class FSContainterChanged {};
   
   FSGetCatalogInfoBulk_Result FSGetCatalogInfoBulk( FSIterator          iterator,
                                                     ItemCount           maximumObjects,
                                                     FSCatalogInfoBitmap whichInfo,
                                                     FSCatalogInfo *     catalogInfos,
                                                     FSRef *             refs = 0,
                                                     FSSpec *            specs = 0,
                                                     HFSUniStr255 *      names = 0 );
   
   void FSGetCatalogInfoBulk( FSIterator                  iterator,
                              FSCatalogInfoBitmap         whichInfo,
                              std::vector<FSCatalogInfo> *catalogInfos,
                              std::vector<FSRef>         *refs = 0,
                              std::vector<FSSpec>        *specs = 0,
                              std::vector<HFSUniStr255>  *names = 0 );
                              
#if 0  
   template < ::FSCatalogInfoBitmap whichInfo >
   class FSCatalogInfo_ContainerSpecifics
     {
      public:
         typedef FSCatalogInfo value_type;
         typedef FSCatalogIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef ErrFSNoMoreItems EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetCatalogInfo( position, whichInfo );
           }
     };

   template < ::FSCatalogInfoBitmap whichInfo > class FSCatalogInfo_Container;
   template < ::FSCatalogInfoBitmap whichInfo > FSCatalogInfo_Container<whichInfo> FSCatalogInfos();
   
   template < ::FSCatalogInfoBitmap whichInfo >
   class FSCatalogInfo_Container: public IndexUntilFailureContainer< FSCatalogInfo_ContainerSpecifics<whichInfo> >
     {
      friend FSCatalogInfo_Container<whichInfo> FSCatalogInfos<whichInfo>();

      private:
         FSCatalogInfo_Container()
           : IndexUntilFailureContainer< FSCatalogInfo_ContainerSpecifics<whichInfo> >( FSCatalogInfo_ContainerSpecifics<whichInfo>() )
           {}
     };
   
   template < ::FSCatalogInfoBitmap whichInfo >
   FSCatalogInfo_Container<whichInfo> FSCatalogInfos()
     {
      return FSCatalogInfo_Container<whichInfo>();
     }
   
   
   template < ::FSCatalogInfoBitmap whichInfo >
   CatalogInfo_Container<whichInfo> CatalogInfos( const FSRef& container );
   CatalogRef_Container  CatalogRefs( const FSRef& container );
   CatalogSpec_Container CatalogSpecs( const FSRef& container );
   CatalogName_Container CatalogNames( const FSRef& container );
#endif

   typedef FSGetCatalogInfoBulk_Result FSCatalogSearch_Result;

   FSCatalogSearch_Result FSCatalogSearch( FSIterator            iterator,
                                           const FSSearchParams& searchCriteria,
                                           ItemCount             maximumObjects,
                                           FSCatalogInfoBitmap   whichInfo,
                                           FSCatalogInfo *       catalogInfos,
                                           FSRef *               refs,
                                           FSSpec *              specs,
                                           HFSUniStr255 *        names );

   class FSForkRef
     {
      private:
         FSRef file;
         UniString name;
      
      public:
         FSForkRef()
           : file(),
             name()
           {}
         
         FSForkRef( const FSRef& theFile, const UniString& theName )
           : file( theFile ),
             name( theName )
           {}
         
         FSForkRef( const FSRef& f, UniCharCount nameLength, const UniChar *theName )
           : file( f ),
             name( theName, theName+nameLength )
           {}
         
         const FSRef& File() const            { return file; }
         const UniString& Name() const    { return name; }
     };
   
   inline bool operator==( const FSForkRef& a, const FSForkRef& b )
     {
      return a.Name() == b.Name() && a.File() == b.File();
     }
   
   inline bool operator!=( const FSForkRef& a, const FSForkRef& b )
     {
      return !( a == b );
     }
   
   template <> struct Disposer<FSForkRef>: public std::unary_function< FSIterator, void >,
                                           private DefaultDestructionOSStatusPolicy
     {
      void operator()( const FSForkRef& fork ) const
        {
         OnlyOnce<RegisterFileManagerErrors>();
         DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::FSDeleteFork( &fork.File(),
                                                                                      fork.Name().size(),
                                                                                      fork.Name().data() ) );
        }
     };

   Owned<FSForkRef> FSCreateFork( const FSRef&   ref,
                                  UniCharCount   forkNameLength,
                                  const UniChar *forkName );

   Owned<FSForkRef> FSCreateFork( const FSRef&     ref,
                                  const UniString& forkName );

   void FSDeleteFork( Owned<FSForkRef> );
   
   struct FSIterateForks_Result
     {
      HFSUniStr255 forkName;
      UInt64       forkSize;
      UInt64       forkPhysicalSize;
     };
   
   FSIterateForks_Result FSIterateForks( const FSRef&    ref,
                                         CatPositionRec& forkIterator );
   
   template <> struct Disposer<FSForkRefNum>: public std::unary_function< FSIterator, void >,
                                              private DefaultDestructionOSStatusPolicy
     {
      void operator()( FSForkRefNum fork ) const
        {
         OnlyOnce<RegisterFileManagerErrors>();
         DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::FSCloseFork( fork ) );
        }
     };
   
   Owned<FSForkRefNum> FSOpenFork( const FSRef&   ref,
                                   UniCharCount   forkNameLength,
                                   const UniChar *forkName,
                                   FSIOPermssn    permissions );

   Owned<FSForkRefNum> FSOpenFork( const FSRef&    ref,
                                   const UniString forkName,
                                   FSIOPermssn     permissions );

   Owned<FSForkRefNum> FSOpenFork( const FSForkRef& fork,
                                   FSIOPermssn      permissions );
   
   ByteCount FSReadFork( FSForkRefNum fork,
                         FSIOPosMode  positionMode,
                         SInt64       positionOffset,
                         ByteCount    requestCount,
                         void *       buffer );

   inline ByteCount FSReadFork( FSForkRefNum fork,
                                ByteCount    requestCount,
                                void *       buffer )
     {
      return FSReadFork( fork, fsAtMark, 0, requestCount, buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSReadFork( FSForkRefNum fork,
                         FSIOPosMode  positionMode,
                         SInt64       positionOffset,
                         Element      (&buffer)[count] )
     {
      return FSReadFork( fork, positionMode, positionOffset, count*sizeof(Element), buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSReadFork( FSForkRefNum fork,
                         Element      (&buffer)[count] )
     {
      return FSReadFork( fork, count*sizeof(Element), buffer );
     }

   ByteCount FSWriteFork( FSForkRefNum fork,
                          FSIOPosMode  positionMode,
                          SInt64       positionOffset,
                          ByteCount    requestCount,
                          const void * buffer );

   inline ByteCount FSWriteFork( FSForkRefNum fork,
                                 ByteCount    requestCount,
                                 const void * buffer )
     {
      return FSWriteFork( fork, fsAtMark, 0, requestCount, buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSWriteFork( FSForkRefNum  fork,
                          FSIOPosMode   positionMode,
                          SInt64        positionOffset,
                          const Element (&buffer)[count] )
     {
      return FSWriteFork( fork, positionMode, positionOffset, count*sizeof(Element), buffer );
     }

   template < class Element, std::size_t count >
   ByteCount FSWriteFork( FSForkRefNum  fork,
                          const Element (&buffer)[count] )
     {
      return FSWriteFork( fork, count*sizeof(Element), buffer );
     }
   
   SInt64 FSGetForkPosition( FSForkRefNum forkRefNum );
   
   void FSSetForkPosition( FSForkRefNum forkRefNum,
                           FSIOPosMode  positionMode,
                           SInt64       positionOffset );
   
   SInt64 FSGetForkSize( FSForkRefNum forkRefNum );

   void FSSetForkSize( FSForkRefNum forkRefNum,
                       FSIOPosMode  positionMode,
                       SInt64       positionOffset );
   
   UInt64 FSAllocateFork( FSForkRefNum      forkRefNum,
                          FSAllocationFlags flags,
                          FSIOPosMode       positionMode,
                          SInt64            positionOffset,
                          UInt64            requestCount );
   
   void FSFlushFork( FSForkRefNum forkRefNum );
   
   void FSCloseFork( Owned<FSForkRefNum> forkRefNum );
   
   struct FSGetForkCBInfo_Result
     {
      FSForkRefNum  actualRefNum;
      FSForkInfo    forkInfo;
      FSRef         ref;
      HFSUniStr255  outForkName;
     };
   
   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkRefNum desiredRefNum );

   FSGetForkCBInfo_Result FSGetForkCBInfo( FSVolumeRefNum  volume,
                                           FSForkIterator& iterator );

   FSGetForkCBInfo_Result FSGetForkCBInfo( FSForkIterator& iterator );


   template < class DesiredType, class MemberType, MemberType FSVolumeInfo::*member >
   struct Basic_FSVolumeInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSVolumeInfo& info )                { return info.*member; }
      static void Set( FSVolumeInfo& info, const Type& value )   { info.*member = value; }
     };

   template < class DesiredType, class MemberType, MemberType FSVolumeInfo::*member >
   struct Const_FSVolumeInfoBit_Traits
     {
      typedef DesiredType Type;
      static Type Get( const FSVolumeInfo& info )                { return info.*member; }
     };
   
   template < ::FSVolumeInfoBitmap bit > struct FSVolumeInfoBit_Traits;
   
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoCreateDate  > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::createDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoModDate     > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::modifyDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoBackupDate  > : Basic_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::backupDate     > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoCheckedDate > : Const_FSVolumeInfoBit_Traits< UTCDateTime,       ::UTCDateTime,   &FSVolumeInfo::checkedDate    > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFileCount   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::fileCount      > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDirCount    > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::folderCount    > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoNextAlloc   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::nextAllocation > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoRsrcClump   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::rsrcClumpSize  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDataClump   > : Const_FSVolumeInfoBit_Traits< UInt32,            ::UInt32,        &FSVolumeInfo::dataClumpSize  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoNextID      > : Const_FSVolumeInfoBit_Traits< HFSCatalogNodeID,  ::UInt32,        &FSVolumeInfo::nextCatalogID  > {};
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFlags       > : Basic_FSVolumeInfoBit_Traits< FSVolumeInfoFlags, ::UInt16,        &FSVolumeInfo::flags          > {};

   struct FSVolumeInfoSizes
     {
      UInt64 totalBytes;
      UInt64 freeBytes;
     };

   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoSizes >
     {
      typedef FSVolumeInfoSizes Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoSizes result;
         result.totalBytes = info.totalBytes;
         result.freeBytes = info.freeBytes;
         return result;
        }
     };

   struct FSVolumeInfoBlocks
     {
      UInt32 blockSize;
      UInt32 totalBlocks;
      UInt32 freeBlocks;
     };

   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoBlocks >
     {
      typedef FSVolumeInfoBlocks Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoBlocks result;
         result.blockSize = info.blockSize;
         result.totalBlocks = info.totalBlocks;
         result.freeBlocks = info.freeBlocks;
         return result;
        }
     };
   
   struct FSVolumeInfoFinderInfo
     {
      UInt8 finderInfo[32];
     };
   
   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFinderInfo >
     {
      typedef FSVolumeInfoFinderInfo Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoFinderInfo result;
         for ( int i = 0; i < sizeof( result.finderInfo ) / sizeof( result.finderInfo[0] ); ++i )
            result.finderInfo[i] = info.finderInfo[i];
         return result;
        }

      static void Set( FSVolumeInfo& info, const FSVolumeInfoFinderInfo& value )
        {
         for ( int i = 0; i < sizeof( info.finderInfo ) / sizeof( info.finderInfo[0] ); ++i )
            info.finderInfo[i] = value.finderInfo[i];
        }      
     };
   
   struct FSVolumeInfoFSInfo
     {
      FSFileSystemID filesystemID;
      UInt16 signature;
     };

   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoFSInfo >
     {
      typedef FSVolumeInfoFSInfo Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoFSInfo result;
         result.filesystemID = info.filesystemID;
         result.signature = info.signature;
         return result;
        }
     };
   
   struct FSVolumeInfoDriveInfo
     {
      UInt16 driveNumber;
      DriverReferenceNumber driverRefNum;
     };

   template <> struct FSVolumeInfoBit_Traits< kFSVolInfoDriveInfo >
     {
      typedef FSVolumeInfoDriveInfo Type;
      
      static Type Get( const FSVolumeInfo& info )
        {
         FSVolumeInfoDriveInfo result;
         result.driveNumber = info.driveNumber;
         result.driverRefNum = info.driverRefNum;
         return result;
        }
     };


   struct FSGetVolumeInfo_Result
     {
      FSVolumeRefNum actualVolume;
      FSVolumeInfo   info;
      HFSUniStr255   volumeName;
      FSRef          rootDirectory;
     };

   void FSGetVolumeInfo( FSVolumeRefNum     volume,
                         FSVolumeIndex      volumeIndex,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );

   void FSGetVolumeInfo( FSVolumeRefNum     volume,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );

   void FSGetVolumeInfo( FSVolumeIndex      volumeIndex,
                         FSVolumeRefNum *   actualVolume,
                         FSVolumeInfoBitmap whichInfo,
                         FSVolumeInfo *     info,
                         HFSUniStr255 *     volumeName,
                         FSRef *            rootDirectory );
   
   FSGetVolumeInfo_Result FSGetVolumeInfo( FSVolumeRefNum volume,
                                           FSVolumeInfoBitmap whichInfo );
   
   FSGetVolumeInfo_Result FSGetVolumeInfo( FSVolumeIndex volumeIndex,
                                           FSVolumeInfoBitmap whichInfo);

   template< ::FSVolumeInfoBitmap bit >
   struct FSGetVolumeInfo_Traits
     {
      typedef typename FSVolumeInfoBit_Traits<bit>::Type Result;
     };
   
   template< ::FSVolumeInfoBitmap bit >
   typename FSGetVolumeInfo_Traits<bit>::Result FSGetVolumeInfo( FSVolumeRefNum volume )
     {
      FSVolumeInfo info;
      FSGetVolumeInfo( volume, 0, bit, &info, 0, 0 );
      return FSVolumeInfoBit_Traits<bit>::Get( info );
     }
   
   template< ::FSVolumeInfoBitmap bit >
   typename FSGetVolumeInfo_Traits<bit>::Result FSGetVolumeInfo( FSVolumeIndex volume )
     {
      FSVolumeInfo info;
      FSGetVolumeInfo( volume, 0, bit, &info, 0, 0 );
      return FSVolumeInfoBit_Traits<bit>::Get( info );
     }
     
   FSVolumeRefNum FSGetVolumeRefNum( FSVolumeIndex volumeIndex );
   
   HFSUniStr255 FSGetVolumeName( FSVolumeRefNum volume );
   HFSUniStr255 FSGetVolumeName( FSVolumeIndex volumeIndex );
   
   FSRef FSGetVolumeRootDirectory( FSVolumeRefNum volume );
   FSRef FSGetVolumeRootDirectory( FSVolumeIndex volumeIndex );

   class Volume_ContainerSpecifics
     {
      public:
         typedef FSVolumeRefNum value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeRefNum( position );
           }
     };

   class Volume_Container: public IndexUntilFailureContainer< Volume_ContainerSpecifics >
     {
      friend Volume_Container Volumes();
      private:
         Volume_Container()
           : IndexUntilFailureContainer< Volume_ContainerSpecifics >( Volume_ContainerSpecifics() )
           {}
     };

   inline Volume_Container Volumes()
     {
      return Volume_Container();
     }

   typedef Volume_Container FSVolumeRefNum_Container;
   
   inline FSVolumeRefNum_Container VolumeRefNums()
     {
      return Volumes();
     }


   typedef FSGetVolumeInfo_Result VolumeInfo;

   template < ::FSVolumeInfoBitmap whichInfo >
   class VolumeInfo_ContainerSpecifics
     {
      public:
         typedef VolumeInfo value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeInfo( position, whichInfo );
           }
     };

   template < ::FSVolumeInfoBitmap whichInfo > class VolumeInfo_Container;
   template < ::FSVolumeInfoBitmap whichInfo > VolumeInfo_Container<whichInfo> VolumeInfos();
   
   template < ::FSVolumeInfoBitmap whichInfo >
   class VolumeInfo_Container: public IndexUntilFailureContainer< VolumeInfo_ContainerSpecifics<whichInfo> >
     {
      friend VolumeInfo_Container<whichInfo> VolumeInfos<whichInfo>();

      private:
         VolumeInfo_Container()
           : IndexUntilFailureContainer< VolumeInfo_ContainerSpecifics<whichInfo> >( VolumeInfo_ContainerSpecifics<whichInfo>() )
           {}
     };
   
   template < ::FSVolumeInfoBitmap whichInfo >
   VolumeInfo_Container<whichInfo> VolumeInfos()
     {
      return VolumeInfo_Container<whichInfo>();
     }

   class VolumeName_ContainerSpecifics
     {
      public:
         typedef HFSUniStr255 value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeName( position );
           }
     };

   class VolumeName_Container: public IndexUntilFailureContainer< VolumeName_ContainerSpecifics >
     {
      friend VolumeName_Container VolumeNames();
      private:
         VolumeName_Container()
           : IndexUntilFailureContainer< VolumeName_ContainerSpecifics >( VolumeName_ContainerSpecifics() )
           {}
     };

   inline VolumeName_Container VolumeNames()
     {
      return VolumeName_Container();
     }

   class VolumeRootDirectory_ContainerSpecifics
     {
      public:
         typedef FSRef value_type;
         typedef FSVolumeIndex size_type;
         typedef SInt64 difference_type;
      
         static size_type begin_position()   { return 1; }
         static size_type end_position()     { return 0; }
         
         typedef NSVErr EndOfEnumeration;
         
         value_type GetValue( size_type position )
           {
            return FSGetVolumeRootDirectory( position );
           }
     };

   class VolumeRootDirectory_Container: public IndexUntilFailureContainer< VolumeRootDirectory_ContainerSpecifics >
     {
      friend VolumeRootDirectory_Container VolumeRootDirectories();
      private:
         VolumeRootDirectory_Container()
           : IndexUntilFailureContainer< VolumeRootDirectory_ContainerSpecifics >( VolumeRootDirectory_ContainerSpecifics() )
           {}
     };

   inline VolumeRootDirectory_Container VolumeRootDirectories()
     {
      return VolumeRootDirectory_Container();
     }

   void FSSetVolumeInfo( FSVolumeRefNum volume,
                         FSVolumeInfoBitmap  whichInfo,
                         const FSVolumeInfo& volumeInfo );
   
   template< ::FSVolumeInfoBitmap bit >
   struct FSSetVolumeInfo_Traits
     {
      typedef const typename FSVolumeInfoBit_Traits<bit>::Type& ParameterType;
     };

   template< ::FSVolumeInfoBitmap bit >
   void FSSetVolumeInfo( FSVolumeRefNum volume, typename FSSetVolumeInfo_Traits<bit>::ParameterType value )
     {
      FSVolumeInfo info;
      FSVolumeInfoBit_Traits<bit>::Set( info, value );
      FSSetVolumeInfo( volume, bit, info );
     }
   
   
   HFSUniStr255 FSGetDataForkName();
   
   HFSUniStr255 FSGetResourceForkName();
   
   void FSRefMakePath( const FSRef& ref,
                       UInt8 *      path,
                       UInt32       maxPathSize );

   template < UInt32 size >
   void FSRefMakePath( const FSRef& ref,
                       UInt8 (&path)[size] )
     {
      FSRefMakePath( ref, path, size );
     }

   std::string FSRefMakePath( const FSRef& ref );
   
   struct FSPathMakeRef_Result
     {
      FSRef ref;
      bool isDirectory;
      
      operator const FSRef&() const  { return ref; }
     };
   
   FSPathMakeRef_Result FSPathMakeRef( const UInt8 *path );
   FSPathMakeRef_Result FSPathMakeRef( const std::string& path );
   
   template <> struct Converter< FSRef, std::string >: public std::unary_function< std::string, FSRef >
     {
      FSRef operator()( const std::string& path ) const
        {
         return FSPathMakeRef( path );
        }
     };

   template <> struct Converter< std::string, FSRef >: public std::unary_function< FSRef, std::string >
     {
      std::string operator()( const FSRef& ref ) const
        {
         return FSRefMakePath( ref );
        }
     };
  }

#endif
