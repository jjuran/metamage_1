// Nitrogen/AERegistry.hh
// ----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_AEREGISTRY_HH
#define NITROGEN_AEREGISTRY_HH

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif
#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_APPLEEVENTS_HH
#include "Nitrogen/AppleEvents.hh"
#endif

namespace Nitrogen
{
	
	// enum...
	
	static const AEKeyForm formUniqueID = AEKeyForm( ::formUniqueID );
	
	// kAEAsk                                                 
	// kAEBefore                                              
	// kAEBeginning                                           
	// kAEBeginTransaction                                    
	// kAEBold                                                
	// kAECentered                                            
	// kAEChangeView                                          
	// kAEClone                                               
	static const AEEventID kAEClose = AEEventID( ::kAEClose );
	// kAECondensed                                           
	static const AEEventID kAECopy = AEEventID( ::kAECopy );
	static const AEEventClass kAECoreSuite = AEEventClass( ::kAECoreSuite );
	static const AEEventID kAECountElements = AEEventID( ::kAECountElements );
	static const AEEventID kAECreateElement = AEEventID( ::kAECreateElement );
	static const AEEventID kAECreatePublisher = AEEventID( ::kAECreatePublisher );
	static const AEEventID kAECut = AEEventID( ::kAECut );
	static const AEEventID kAEDelete = AEEventID( ::kAEDelete );
	static const AEEventID kAEDoObjectsExist = AEEventID( ::kAEDoObjectsExist );
	static const AEEventID kAEDoScript = AEEventID( ::kAEDoScript );
	// kAEDrag                                                
	// kAEDuplicateSelection                                  
	// kAEEditGraphic                                         
	// kAEEmptyTrash                                          
	// kAEEnd                                                 
	// kAEEndTransaction                                      
	// kAEExpanded                                            
	// kAEFast                                                
	// kAEFinderEvents                                        
	// kAEFormulaProtect                                      
	// kAEFullyJustified                                      
	// kAEGetClassInfo                                        
	static const AEEventID kAEGetData = AEEventID( ::kAEGetData );
	static const AEEventID kAEGetDataSize = AEEventID( ::kAEGetDataSize );
	// kAEGetEventInfo                                        
	// kAEGetInfoSelection                                    
	// kAEGetPrivilegeSelection                               
	// kAEGetSuiteInfo                                        
	// kAEGrow                                                
	// kAEHidden                                              
	// kAEHiQuality                                           
	// kAEImageGraphic                                        
	// kAEIsUniform                                           
	// kAEItalic                                              
	// kAELeftJustified                                       
	// kAELowercase                                           
	// kAEMakeObjectsVisible                                  
	static const AEEventClass kAEMiscStandards = AEEventClass( ::kAEMiscStandards );
	// kAEModifiable                                          
	// kAEMove                                                
	// kAENo                                                  
	// kAENoArrow                                             
	// kAENonmodifiable                                       
	static const AEEventID kAEOpen = AEEventID( ::kAEOpen );
	// kAEOpenSelection                                       
	// kAEOutline                                             
	// kAEPageSetup                                           
	static const AEEventID kAEPaste = AEEventID( ::kAEPaste );
	// kAEPlain                                               
	static const AEEventID kAEPrint = AEEventID( ::kAEPrint );
	// kAEPrintSelection                                      
	// kAEPrintWindow                                         
	// kAEPutAwaySelection                                    
	// kAEQDAddOver                                           
	// kAEQDAddPin                                            
	// kAEQDAdMax                                             
	// kAEQDAdMin                                             
	// kAEQDBic                                               
	// kAEQDBlend                                             
	// kAEQDCopy                                              
	// kAEQDNotBic                                            
	// kAEQDNotCopy                                           
	// kAEQDNotOr                                             
	// kAEQDNotXor                                            
	// kAEQDOr                                                
	// kAEQDSubOver                                           
	// kAEQDSubPin                                            
	static const AEEventClass kAEQDSupplementalSuite = AEEventClass( ::kAEQDSupplementalSuite );
	// kAEQDXor                                               
	static const AEEventClass kAEQuickdrawSuite = AEEventClass( ::kAEQuickdrawSuite );
	// kAEQuitAll                                             
	// kAERedo                                                
	// kAERegular                                             
	static const AEEventID kAEReopenApplication = AEEventID( ::kAEReopenApplication );
	// kAEReplace                                             
	static const AEEventClass kAERequiredSuite = AEEventClass( ::kAERequiredSuite );
	// kAERestart                                             
	// kAERevealSelection                                     
	// kAERevert                                              
	// kAERightJustified                                      
	// kAESave                                                
	static const AEEventID kAESelect = AEEventID( ::kAESelect );
	static const AEEventID kAESetData = AEEventID( ::kAESetData );
	// kAESetPosition                                         
	// kAEShadow                                              
	// kAEShowClipboard                                       
	// kAEShutDown                                            
	// kAESleep                                               
	// kAESmallCaps                                           
	// kAESpecialClassProperties                              
	// kAEStrikethrough                                       
	// kAESubscript                                           
	// kAESuperscript                                         
	static const AEEventClass kAETableSuite = AEEventClass( ::kAETableSuite );
	static const AEEventClass kAETextSuite = AEEventClass( ::kAETextSuite );
	// kAETransactionTerminated                               
	// kAEUnderline                                           
	// kAEUndo                                                
	// kAEWholeWordEquals                                     
	// kAEYes                                                 
	// kAEZoom                                                
	
	// EventRecord Classes and EventIDs ...
	
	
	// kAEMenuClass .. kAEPromise
	// keyMenuID .. keyLocalWhere
	// typeHIMenu, typeHIWindow
	// kBySmallIcon .. kByVersionView
	// kAEInfo .. kAESharing
	// kAEZoomIn, kAEZoomOut
	// kTextServiceClass .. kGetSelectedText
	// keyAETSMDocumentRefcon .. keyAETextServiceMacEncoding
	
	// kTSMOutsideOfBody .. kTSMInsideOfActiveInputArea
	// kNextBody, kPreviousBody
	
	// kCaretPosition .. kSelectedText
	// keyAEHiliteRange .. keyAEDragging
	
	// typeMeters .. typeDegreesK
	// kFAServerApp .. kFAIndexParam
	
	// kAEInternetSuite, kAEISWebStarSuite
	// kAEISGetURL, KAEISHandleCGI
	// cURL .. cFTPItem
	// kAEISHTTPSearchArgs .. kAEISFullRequest
	// pScheme .. pFTPKind
	// eScheme .. eurlUnknown
	// kConnSuite .. eVideoOut
	// cKeystroke .. eF15Key
	
	template <> struct AEKeyword_Traits< keyAEObjectClass > : Type_AEKeyword_Traits< AEObjectClass > {};
	
   template<> struct DescType_Traits< typeFixedPoint                >: Nucleus::PodFlattener< FixedPoint                > {};
   template<> struct DescType_Traits< typeFixedRectangle            >: Nucleus::PodFlattener< FixedRect                 > {};
   template<> struct DescType_Traits< typeIntlWritingCode           >: Nucleus::PodFlattener< ScriptCode                > {};
   template<> struct DescType_Traits< typePtr                       >: Nucleus::PodFlattener< void *                    > {};
   template<> struct DescType_Traits< typeQDPoint                   >: Nucleus::PodFlattener< Point                     > {};
   template<> struct DescType_Traits< typeRectangle                 >: Nucleus::PodFlattener< Rect                      > {};
   template<> struct DescType_Traits< typeRGBColor                  >: Nucleus::PodFlattener< RGBColor                  > {};
   template<> struct DescType_Traits< typeScript                    >: Nucleus::PodFlattener< ScriptCode                > {};
   template<> struct DescType_Traits< typeTextRange                 >: Nucleus::PodFlattener< TextRange                 > {};
   template<> struct DescType_Traits< typeComponentInstance         >: Nucleus::PodFlattener< ComponentInstance         > {};
   template<> struct DescType_Traits< typeEventRef                  >: Nucleus::PodFlattener< EventRef                  > {};

   template<> struct DescType_Traits< typeUnicodeText >: UnicodeFlattener< UniChar > {};
	
   using ::OffsetArray;

   inline std::size_t SizeOf_OffsetArray( const OffsetArray& array )
     {
      return sizeof( array ) - sizeof( array.fOffset ) + array.fNumOfOffsets * sizeof( long );
     }
   
   using ::TextRangeArray;

   inline std::size_t SizeOf_TextRangeArray( const TextRangeArray& array )
     {
      return sizeof( array ) - sizeof( array.fRange ) + array.fNumOfRanges * sizeof( TextRange );
     }
   
   template<> struct DescType_Traits< typeVersion        >: Nucleus::VariableLengthPodFlattener< VersRec,        SizeOf_VersRec        > {};
   template<> struct DescType_Traits< typeOffsetArray    >: Nucleus::VariableLengthPodFlattener< OffsetArray,    SizeOf_OffsetArray    > {};
   template<> struct DescType_Traits< typeTextRangeArray >: Nucleus::VariableLengthPodFlattener< TextRangeArray, SizeOf_TextRangeArray > {};
  }

#endif
