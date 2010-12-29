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
	
   template<> struct DescType_Traits< Mac::typeVersion        >: nucleus::variable_length_POD_scribe< VersRec,        Mac::SizeOf_VersRec   > {};
   template<> struct DescType_Traits< Mac::typeOffsetArray    >: nucleus::variable_length_POD_scribe< OffsetArray,    Mac::sizeof_OffsetArray    > {};
   template<> struct DescType_Traits< Mac::typeTextRangeArray >: nucleus::variable_length_POD_scribe< TextRangeArray, Mac::sizeof_TextRangeArray > {};
  }

#endif
