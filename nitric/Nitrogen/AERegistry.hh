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

// Nitrogen
#ifndef MAC_APPLEEVENTS_UTILITIES_SIZEOFOFFSETARRAY_HH
#include "Mac/AppleEvents/Utilities/sizeof_OffsetArray.hh"
#endif
#ifndef MAC_APPLEEVENTS_UTILITIES_SIZEOFTEXTRANGEARRAY_HH
#include "Mac/AppleEvents/Utilities/sizeof_TextRangeArray.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_SIZEOFVERSREC_HH
#include "Mac/Toolbox/Utilities/SizeOf_VersRec.hh"
#endif

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	// enum...
	
	// kAEAsk                                                 
	// kAEBefore                                              
	// kAEBeginning                                           
	// kAEBeginTransaction                                    
	// kAEBold                                                
	// kAECentered                                            
	// kAEChangeView                                          
	// kAECondensed                                           
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
	// kAEGetInfoSelection                                    
	// kAEGetPrivilegeSelection                               
	// kAEGrow                                                
	// kAEHidden                                              
	// kAEHiQuality                                           
	// kAEImageGraphic                                        
	// kAEIsUniform                                           
	// kAEItalic                                              
	// kAELeftJustified                                       
	// kAELowercase                                           
	// kAEMakeObjectsVisible                                  
	// kAEModifiable                                          
	// kAENo                                                  
	// kAENoArrow                                             
	// kAENonmodifiable                                       
	// kAEOpenSelection                                       
	// kAEOutline                                             
	// kAEPageSetup                                           
	// kAEPlain                                               
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
	// kAEQDXor                                               
	// kAEQuitAll                                             
	// kAERedo                                                
	// kAERegular                                             
	// kAEReplace                                             
	// kAERestart                                             
	// kAERevealSelection                                     
	// kAERevert                                              
	// kAERightJustified                                      
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
	
	template <> struct AEKeyword_Traits< Mac::keyAEObjectClass > : Type_AEKeyword_Traits< Mac::AEObjectClass > {};
	
   template<> struct DescType_Traits< Mac::typeFixedPoint                >: nucleus::POD_scribe< FixedPoint                > {};
   template<> struct DescType_Traits< Mac::typeFixedRectangle            >: nucleus::POD_scribe< FixedRect                 > {};
   template<> struct DescType_Traits< Mac::typeIntlWritingCode           >: nucleus::POD_scribe< ScriptCode                > {};
   template<> struct DescType_Traits< Mac::typePtr                       >: nucleus::POD_scribe< void *                    > {};
   template<> struct DescType_Traits< Mac::typeQDPoint                   >: nucleus::POD_scribe< Point                     > {};
   template<> struct DescType_Traits< Mac::typeRectangle                 >: nucleus::POD_scribe< Rect                      > {};
   template<> struct DescType_Traits< Mac::typeRGBColor                  >: nucleus::POD_scribe< RGBColor                  > {};
   template<> struct DescType_Traits< Mac::typeScript                    >: nucleus::POD_scribe< ScriptCode                > {};
   template<> struct DescType_Traits< Mac::typeTextRange                 >: nucleus::POD_scribe< TextRange                 > {};
   template<> struct DescType_Traits< Mac::typeComponentInstance         >: nucleus::POD_scribe< ComponentInstance         > {};
   template<> struct DescType_Traits< Mac::typeEventRef                  >: nucleus::POD_scribe< EventRef                  > {};

   template<> struct DescType_Traits< Mac::typeVersion        >: nucleus::variable_length_POD_scribe< VersRec,        Mac::SizeOf_VersRec   > {};
   template<> struct DescType_Traits< Mac::typeOffsetArray    >: nucleus::variable_length_POD_scribe< OffsetArray,    Mac::sizeof_OffsetArray    > {};
   template<> struct DescType_Traits< Mac::typeTextRangeArray >: nucleus::variable_length_POD_scribe< TextRangeArray, Mac::sizeof_TextRangeArray > {};
  }

#endif
