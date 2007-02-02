// AERegistry.h

#ifndef NITROGEN_AEREGISTRY_H
#define NITROGEN_AEREGISTRY_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEREGISTRY__
#include FRAMEWORK_HEADER(AE,AERegistry.h)
#endif
#ifndef __CARBONEVENTS__
#include FRAMEWORK_HEADER(HIToolbox,CarbonEvents.h)
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_APPLEEVENTS_H
#include "Nitrogen/AppleEvents.h"
#endif

namespace Nitrogen
{
	
	static const AEObjectClass cAEList             = AEObjectClass( ::cAEList             );
	static const AEObjectClass cApplication        = AEObjectClass( ::cApplication        );
	static const AEObjectClass cArc                = AEObjectClass( ::cArc                );
	static const AEObjectClass cBoolean            = AEObjectClass( ::cBoolean            );
	static const AEObjectClass cCell               = AEObjectClass( ::cCell               );
	static const AEObjectClass cChar               = AEObjectClass( ::cChar               );
	static const AEObjectClass cColorTable         = AEObjectClass( ::cColorTable         );
	static const AEObjectClass cColumn             = AEObjectClass( ::cColumn             );
	static const AEObjectClass cDocument           = AEObjectClass( ::cDocument           );
	static const AEObjectClass cDrawingArea        = AEObjectClass( ::cDrawingArea        );
	static const AEObjectClass cEnumeration        = AEObjectClass( ::cEnumeration        );
	static const AEObjectClass cFile               = AEObjectClass( ::cFile               );
	static const AEObjectClass cFixed              = AEObjectClass( ::cFixed              );
	static const AEObjectClass cFixedPoint         = AEObjectClass( ::cFixedPoint         );
	static const AEObjectClass cFixedRectangle     = AEObjectClass( ::cFixedRectangle     );
	static const AEObjectClass cGraphicLine        = AEObjectClass( ::cGraphicLine        );
	static const AEObjectClass cGraphicObject      = AEObjectClass( ::cGraphicObject      );
	static const AEObjectClass cGraphicShape       = AEObjectClass( ::cGraphicShape       );
	static const AEObjectClass cGraphicText        = AEObjectClass( ::cGraphicText        );
	static const AEObjectClass cGroupedGraphic     = AEObjectClass( ::cGroupedGraphic     );


	static const AEObjectClass cInsertionLoc       = AEObjectClass( ::cInsertionLoc       );
	static const AEObjectClass cInsertionPoint     = AEObjectClass( ::cInsertionPoint     );
	static const AEObjectClass cIntlText           = AEObjectClass( ::cIntlText           );
	static const AEObjectClass cIntlWritingCode    = AEObjectClass( ::cIntlWritingCode    );
	static const AEObjectClass cItem               = AEObjectClass( ::cItem               );
	static const AEObjectClass cLine               = AEObjectClass( ::cLine               );
	static const AEObjectClass cLongDateTime       = AEObjectClass( ::cLongDateTime       );
	static const AEObjectClass cLongFixed          = AEObjectClass( ::cLongFixed          );
	static const AEObjectClass cLongFixedPoint     = AEObjectClass( ::cLongFixedPoint     );
	static const AEObjectClass cLongFixedRectangle = AEObjectClass( ::cLongFixedRectangle );
	static const AEObjectClass cLongInteger        = AEObjectClass( ::cLongInteger        );
	static const AEObjectClass cLongPoint          = AEObjectClass( ::cLongPoint          );
	static const AEObjectClass cLongRectangle      = AEObjectClass( ::cLongRectangle      );
	static const AEObjectClass cMachineLoc         = AEObjectClass( ::cMachineLoc         );
	static const AEObjectClass cMenu               = AEObjectClass( ::cMenu               );
	static const AEObjectClass cMenuItem           = AEObjectClass( ::cMenuItem           );
	static const AEObjectClass cObject             = AEObjectClass( ::cObject             );
	static const AEObjectClass cObjectSpecifier    = AEObjectClass( ::cObjectSpecifier    );
	static const AEObjectClass cOpenableObject     = AEObjectClass( ::cOpenableObject     );
	static const AEObjectClass cOval               = AEObjectClass( ::cOval               );


	static const AEObjectClass cParagraph          = AEObjectClass( ::cParagraph          );
	static const AEObjectClass cPICT               = AEObjectClass( ::cPICT               );
	static const AEObjectClass cPixel              = AEObjectClass( ::cPixel              );
	static const AEObjectClass cPixelMap           = AEObjectClass( ::cPixelMap           );
	static const AEObjectClass cPolygon            = AEObjectClass( ::cPolygon            );
	static const AEObjectClass cProperty           = AEObjectClass( ::cProperty           );
	static const AEObjectClass cQDPoint            = AEObjectClass( ::cQDPoint            );
	static const AEObjectClass cQDRectangle        = AEObjectClass( ::cQDRectangle        );
	static const AEObjectClass cRectangle          = AEObjectClass( ::cRectangle          );
	static const AEObjectClass cRGBColor           = AEObjectClass( ::cRGBColor           );
	static const AEObjectClass cRotation           = AEObjectClass( ::cRotation           );
	static const AEObjectClass cRoundedRectangle   = AEObjectClass( ::cRoundedRectangle   );
	static const AEObjectClass cRow                = AEObjectClass( ::cRow                );
	static const AEObjectClass cSelection          = AEObjectClass( ::cSelection          );
	static const AEObjectClass cShortInteger       = AEObjectClass( ::cShortInteger       );
	static const AEObjectClass cTable              = AEObjectClass( ::cTable              );
	static const AEObjectClass cText               = AEObjectClass( ::cText               );
	static const AEObjectClass cTextFlow           = AEObjectClass( ::cTextFlow           );
	static const AEObjectClass cTextStyles         = AEObjectClass( ::cTextStyles         );
	static const AEObjectClass cType               = AEObjectClass( ::cType               );
	
	static const AEObjectClass cVersion            = AEObjectClass( ::cVersion            );
	static const AEObjectClass cWindow             = AEObjectClass( ::cWindow             );
	static const AEObjectClass cWord               = AEObjectClass( ::cWord               );
	
	// enum...
	
	static const AEKeyForm formUniqueID = AEKeyForm( ::formUniqueID );
	
	// kAEAsk                                                 
	// kAEBefore                                              
	// kAEBeginning                                           
	// kAEBeginsWith                                          
	// kAEBeginTransaction                                    
	// kAEBold                                                
	// kAECaseSensEquals                                      
	// kAECentered                                            
	// kAEChangeView                                          
	// kAEClone                                               
	static const AEEventID kAEClose = AEEventID( ::kAEClose );
	// kAECondensed                                           
	// kAEContains                                            
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
	// kAEEndsWith                                            
	// kAEEndTransaction                                      
	// kAEEquals                                              
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
	// kAEGreaterThan                                         
	// kAEGreaterThanEquals                                   
	// kAEGrow                                                
	// kAEHidden                                              
	// kAEHiQuality                                           
	// kAEImageGraphic                                        
	// kAEIsUniform                                           
	// kAEItalic                                              
	// kAELeftJustified                                       
	// kAELessThan                                            
	// kAELessThanEquals                                      
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
	
	
	static const AEPropertyID pArcAngle           = AEPropertyID( ::pArcAngle           );
	static const AEPropertyID pBackgroundColor    = AEPropertyID( ::pBackgroundColor    );
	static const AEPropertyID pBackgroundPattern  = AEPropertyID( ::pBackgroundPattern  );
	static const AEPropertyID pBestType           = AEPropertyID( ::pBestType           );
	static const AEPropertyID pBounds             = AEPropertyID( ::pBounds             );
	static const AEPropertyID pClass              = AEPropertyID( ::pClass              );
	static const AEPropertyID pClipboard          = AEPropertyID( ::pClipboard          );
	static const AEPropertyID pColor              = AEPropertyID( ::pColor              );
	static const AEPropertyID pColorTable         = AEPropertyID( ::pColorTable         );
	static const AEPropertyID pContents           = AEPropertyID( ::pContents           );
	static const AEPropertyID pCornerCurveHeight  = AEPropertyID( ::pCornerCurveHeight  );
	static const AEPropertyID pCornerCurveWidth   = AEPropertyID( ::pCornerCurveWidth   );
	static const AEPropertyID pDashStyle          = AEPropertyID( ::pDashStyle          );
	static const AEPropertyID pDefaultType        = AEPropertyID( ::pDefaultType        );
	static const AEPropertyID pDefinitionRect     = AEPropertyID( ::pDefinitionRect     );
	static const AEPropertyID pEnabled            = AEPropertyID( ::pEnabled            );
	static const AEPropertyID pEndPoint           = AEPropertyID( ::pEndPoint           );
	static const AEPropertyID pFillColor          = AEPropertyID( ::pFillColor          );
	static const AEPropertyID pFillPattern        = AEPropertyID( ::pFillPattern        );
	static const AEPropertyID pFont               = AEPropertyID( ::pFont               );
	static const AEPropertyID pFormula            = AEPropertyID( ::pFormula            );
	static const AEPropertyID pGraphicObjects     = AEPropertyID( ::pGraphicObjects     );
	static const AEPropertyID pHasCloseBox        = AEPropertyID( ::pHasCloseBox        );
	static const AEPropertyID pHasTitleBar        = AEPropertyID( ::pHasTitleBar        );
	static const AEPropertyID pID                 = AEPropertyID( ::pID                 );
	static const AEPropertyID pIndex              = AEPropertyID( ::pIndex              );
	static const AEPropertyID pInsertionLoc       = AEPropertyID( ::pInsertionLoc       );
	static const AEPropertyID pIsFloating         = AEPropertyID( ::pIsFloating         );
	static const AEPropertyID pIsFrontProcess     = AEPropertyID( ::pIsFrontProcess     );
	static const AEPropertyID pIsModal            = AEPropertyID( ::pIsModal            );
	static const AEPropertyID pIsModified         = AEPropertyID( ::pIsModified         );
	static const AEPropertyID pIsResizable        = AEPropertyID( ::pIsResizable        );
	static const AEPropertyID pIsStationeryPad    = AEPropertyID( ::pIsStationeryPad    );
	static const AEPropertyID pIsZoomable         = AEPropertyID( ::pIsZoomable         );
	static const AEPropertyID pIsZoomed           = AEPropertyID( ::pIsZoomed           );
	static const AEPropertyID pItemNumber         = AEPropertyID( ::pItemNumber         );
	static const AEPropertyID pJustification      = AEPropertyID( ::pJustification      );
	static const AEPropertyID pLineArrow          = AEPropertyID( ::pLineArrow          );
	static const AEPropertyID pMenuID             = AEPropertyID( ::pMenuID             );
	static const AEPropertyID pName               = AEPropertyID( ::pName               );
	static const AEPropertyID pNewElementLoc      = AEPropertyID( ::pNewElementLoc      );
	static const AEPropertyID pPenColor           = AEPropertyID( ::pPenColor           );
	static const AEPropertyID pPenPattern         = AEPropertyID( ::pPenPattern         );
	static const AEPropertyID pPenWidth           = AEPropertyID( ::pPenWidth           );
	static const AEPropertyID pPixelDepth         = AEPropertyID( ::pPixelDepth         );
	static const AEPropertyID pPointList          = AEPropertyID( ::pPointList          );
	static const AEPropertyID pPointSize          = AEPropertyID( ::pPointSize          );
	static const AEPropertyID pProtection         = AEPropertyID( ::pProtection         );
	static const AEPropertyID pRotation           = AEPropertyID( ::pRotation           );
	static const AEPropertyID pScale              = AEPropertyID( ::pScale              );
	static const AEPropertyID pScript             = AEPropertyID( ::pScript             );
	static const AEPropertyID pScriptTag          = AEPropertyID( ::pScriptTag          );
	static const AEPropertyID pSelected           = AEPropertyID( ::pSelected           );
	static const AEPropertyID pSelection          = AEPropertyID( ::pSelection          );
	static const AEPropertyID pStartAngle         = AEPropertyID( ::pStartAngle         );
	static const AEPropertyID pStartPoint         = AEPropertyID( ::pStartPoint         );
	static const AEPropertyID pTextColor          = AEPropertyID( ::pTextColor          );
	static const AEPropertyID pTextFont           = AEPropertyID( ::pTextFont           );
	static const AEPropertyID pTextItemDelimiters = AEPropertyID( ::pTextItemDelimiters );
	static const AEPropertyID pTextPointSize      = AEPropertyID( ::pTextPointSize      );
	static const AEPropertyID pTextStyles         = AEPropertyID( ::pTextStyles         );
	static const AEPropertyID pTransferMode       = AEPropertyID( ::pTransferMode       );
	static const AEPropertyID pTranslation        = AEPropertyID( ::pTranslation        );
	static const AEPropertyID pUniformStyles      = AEPropertyID( ::pUniformStyles      );
	static const AEPropertyID pUpdateOn           = AEPropertyID( ::pUpdateOn           );
	static const AEPropertyID pUserSelection      = AEPropertyID( ::pUserSelection      );
	static const AEPropertyID pVersion            = AEPropertyID( ::pVersion            );
	static const AEPropertyID pVisible            = AEPropertyID( ::pVisible            );
	
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
