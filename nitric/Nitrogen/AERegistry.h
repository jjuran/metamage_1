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
	
	
	static const AEKeyword keyAEAngle           = AEKeyword( ::keyAEAngle           );
	static const AEKeyword keyAEArcAngle        = AEKeyword( ::keyAEArcAngle        );
	
	static const AEKeyword keyAEBaseAddr        = AEKeyword( ::keyAEBaseAddr        );
	static const AEKeyword keyAEBestType        = AEKeyword( ::keyAEBestType        );
	static const AEKeyword keyAEBgndColor       = AEKeyword( ::keyAEBgndColor       );
	static const AEKeyword keyAEBgndPattern     = AEKeyword( ::keyAEBgndPattern     );
	static const AEKeyword keyAEBounds          = AEKeyword( ::keyAEBounds          );
	static const AEKeyword keyAECellList        = AEKeyword( ::keyAECellList        );
	static const AEKeyword keyAEClassID         = AEKeyword( ::keyAEClassID         );
	static const AEKeyword keyAEColor           = AEKeyword( ::keyAEColor           );
	static const AEKeyword keyAEColorTable      = AEKeyword( ::keyAEColorTable      );
	static const AEKeyword keyAECurveHeight     = AEKeyword( ::keyAECurveHeight     );
	static const AEKeyword keyAECurveWidth      = AEKeyword( ::keyAECurveWidth      );
	static const AEKeyword keyAEDashStyle       = AEKeyword( ::keyAEDashStyle       );
	static const AEKeyword keyAEData            = AEKeyword( ::keyAEData            );
	static const AEKeyword keyAEDefaultType     = AEKeyword( ::keyAEDefaultType     );
	static const AEKeyword keyAEDefinitionRect  = AEKeyword( ::keyAEDefinitionRect  );
	static const AEKeyword keyAEDescType        = AEKeyword( ::keyAEDescType        );
	static const AEKeyword keyAEDestination     = AEKeyword( ::keyAEDestination     );
	static const AEKeyword keyAEDoAntiAlias     = AEKeyword( ::keyAEDoAntiAlias     );
	static const AEKeyword keyAEDoDithered      = AEKeyword( ::keyAEDoDithered      );
	static const AEKeyword keyAEDoRotate        = AEKeyword( ::keyAEDoRotate        );
	
	static const AEKeyword keyAEDoScale         = AEKeyword( ::keyAEDoScale         );
	static const AEKeyword keyAEDoTranslate     = AEKeyword( ::keyAEDoTranslate     );
	static const AEKeyword keyAEEditionFileLoc  = AEKeyword( ::keyAEEditionFileLoc  );
	static const AEKeyword keyAEElements        = AEKeyword( ::keyAEElements        );
	static const AEKeyword keyAEEndPoint        = AEKeyword( ::keyAEEndPoint        );
	static const AEKeyword keyAEEventClass      = AEKeyword( ::keyAEEventClass      );
	static const AEKeyword keyAEEventID         = AEKeyword( ::keyAEEventID         );
	static const AEKeyword keyAEFile            = AEKeyword( ::keyAEFile            );
	static const AEKeyword keyAEFileType        = AEKeyword( ::keyAEFileType        );
	static const AEKeyword keyAEFillColor       = AEKeyword( ::keyAEFillColor       );
	static const AEKeyword keyAEFillPattern     = AEKeyword( ::keyAEFillPattern     );
	static const AEKeyword keyAEFlipHorizontal  = AEKeyword( ::keyAEFlipHorizontal  );
	static const AEKeyword keyAEFlipVertical    = AEKeyword( ::keyAEFlipVertical    );
	static const AEKeyword keyAEFont            = AEKeyword( ::keyAEFont            );
	static const AEKeyword keyAEFormula         = AEKeyword( ::keyAEFormula         );
	static const AEKeyword keyAEGraphicObjects  = AEKeyword( ::keyAEGraphicObjects  );
	static const AEKeyword keyAEID              = AEKeyword( ::keyAEID              );
	static const AEKeyword keyAEImageQuality    = AEKeyword( ::keyAEImageQuality    );
	static const AEKeyword keyAEInsertHere      = AEKeyword( ::keyAEInsertHere      );
	static const AEKeyword keyAEKeyForms        = AEKeyword( ::keyAEKeyForms        );
	
	static const AEKeyword keyAEKeyword         = AEKeyword( ::keyAEKeyword         );
	static const AEKeyword keyAELevel           = AEKeyword( ::keyAELevel           );
	static const AEKeyword keyAELineArrow       = AEKeyword( ::keyAELineArrow       );
	static const AEKeyword keyAEName            = AEKeyword( ::keyAEName            );
	static const AEKeyword keyAENewElementLoc   = AEKeyword( ::keyAENewElementLoc   );
	static const AEKeyword keyAEObject          = AEKeyword( ::keyAEObject          );
	static const AEKeyword keyAEObjectClass     = AEKeyword( ::keyAEObjectClass     );
	static const AEKeyword keyAEOffStyles       = AEKeyword( ::keyAEOffStyles       );
	static const AEKeyword keyAEOnStyles        = AEKeyword( ::keyAEOnStyles        );
	static const AEKeyword keyAEParameters      = AEKeyword( ::keyAEParameters      );
	static const AEKeyword keyAEParamFlags      = AEKeyword( ::keyAEParamFlags      );
	static const AEKeyword keyAEPenColor        = AEKeyword( ::keyAEPenColor        );
	static const AEKeyword keyAEPenPattern      = AEKeyword( ::keyAEPenPattern      );
	static const AEKeyword keyAEPenWidth        = AEKeyword( ::keyAEPenWidth        );
	static const AEKeyword keyAEPixelDepth      = AEKeyword( ::keyAEPixelDepth      );
	static const AEKeyword keyAEPixMapMinus     = AEKeyword( ::keyAEPixMapMinus     );
	static const AEKeyword keyAEPMTable         = AEKeyword( ::keyAEPMTable         );
	static const AEKeyword keyAEPointList       = AEKeyword( ::keyAEPointList       );
	static const AEKeyword keyAEPointSize       = AEKeyword( ::keyAEPointSize       );
	static const AEKeyword keyAEPosition        = AEKeyword( ::keyAEPosition        );
	
	static const AEKeyword keyAEPropData        = AEKeyword( ::keyAEPropData        );
	static const AEKeyword keyAEProperties      = AEKeyword( ::keyAEProperties      );
	static const AEKeyword keyAEProperty        = AEKeyword( ::keyAEProperty        );
	static const AEKeyword keyAEPropFlags       = AEKeyword( ::keyAEPropFlags       );
	static const AEKeyword keyAEPropID          = AEKeyword( ::keyAEPropID          );
	static const AEKeyword keyAEProtection      = AEKeyword( ::keyAEProtection      );
	static const AEKeyword keyAERenderAs        = AEKeyword( ::keyAERenderAs        );
	static const AEKeyword keyAERequestedType   = AEKeyword( ::keyAERequestedType   );
	static const AEKeyword keyAEResult          = AEKeyword( ::keyAEResult          );
	static const AEKeyword keyAEResultInfo      = AEKeyword( ::keyAEResultInfo      );
	static const AEKeyword keyAERotation        = AEKeyword( ::keyAERotation        );
	static const AEKeyword keyAERotPoint        = AEKeyword( ::keyAERotPoint        );
	static const AEKeyword keyAERowList         = AEKeyword( ::keyAERowList         );
	static const AEKeyword keyAESaveOptions     = AEKeyword( ::keyAESaveOptions     );
	static const AEKeyword keyAEScale           = AEKeyword( ::keyAEScale           );
	static const AEKeyword keyAEScriptTag       = AEKeyword( ::keyAEScriptTag       );
	static const AEKeyword keyAEShowWhere       = AEKeyword( ::keyAEShowWhere       );
	static const AEKeyword keyAEStartAngle      = AEKeyword( ::keyAEStartAngle      );
	static const AEKeyword keyAEStartPoint      = AEKeyword( ::keyAEStartPoint      );
	static const AEKeyword keyAEStyles          = AEKeyword( ::keyAEStyles          );
	
	static const AEKeyword keyAESuiteID         = AEKeyword( ::keyAESuiteID         );
	static const AEKeyword keyAEText            = AEKeyword( ::keyAEText            );
	static const AEKeyword keyAETextColor       = AEKeyword( ::keyAETextColor       );
	static const AEKeyword keyAETextFont        = AEKeyword( ::keyAETextFont        );
	static const AEKeyword keyAETextPointSize   = AEKeyword( ::keyAETextPointSize   );
	static const AEKeyword keyAETextStyles      = AEKeyword( ::keyAETextStyles      );
	static const AEKeyword keyAETextLineHeight  = AEKeyword( ::keyAETextLineHeight  );
	static const AEKeyword keyAETextLineAscent  = AEKeyword( ::keyAETextLineAscent  );
	static const AEKeyword keyAETheText         = AEKeyword( ::keyAETheText         );
	static const AEKeyword keyAETransferMode    = AEKeyword( ::keyAETransferMode    );
	static const AEKeyword keyAETranslation     = AEKeyword( ::keyAETranslation     );
	static const AEKeyword keyAETryAsStructGraf = AEKeyword( ::keyAETryAsStructGraf );
	static const AEKeyword keyAEUniformStyles   = AEKeyword( ::keyAEUniformStyles   );
	static const AEKeyword keyAEUpdateOn        = AEKeyword( ::keyAEUpdateOn        );
	static const AEKeyword keyAEUserTerm        = AEKeyword( ::keyAEUserTerm        );
	static const AEKeyword keyAEWindow          = AEKeyword( ::keyAEWindow          );
	static const AEKeyword keyAEWritingCode     = AEKeyword( ::keyAEWritingCode     );
	
	static const AEKeyword keyMiscellaneous     = AEKeyword( ::keyMiscellaneous     );
	static const AEKeyword keySelection         = AEKeyword( ::keySelection         );
	static const AEKeyword keyWindow            = AEKeyword( ::keyWindow            );
	
	static const AEKeyword keyWhen              = AEKeyword( ::keyWhen              );
	static const AEKeyword keyWhere             = AEKeyword( ::keyWhere             );
	static const AEKeyword keyModifiers         = AEKeyword( ::keyModifiers         );
	static const AEKeyword keyKey               = AEKeyword( ::keyKey               );
	static const AEKeyword keyKeyCode           = AEKeyword( ::keyKeyCode           );
	static const AEKeyword keyKeyboard          = AEKeyword( ::keyKeyboard          );
	static const AEKeyword keyDriveNumber       = AEKeyword( ::keyDriveNumber       );
	static const AEKeyword keyErrorCode         = AEKeyword( ::keyErrorCode         );
	static const AEKeyword keyHighLevelClass    = AEKeyword( ::keyHighLevelClass    );
	static const AEKeyword keyHighLevelID       = AEKeyword( ::keyHighLevelID       );
	
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
	
	
	static const DescType typeAEText             = DescType( ::typeAEText             );
	static const DescType typeArc                = DescType( ::typeArc                );
	static const DescType typeBest               = DescType( ::typeBest               );
	static const DescType typeCell               = DescType( ::typeCell               );
	static const DescType typeClassInfo          = DescType( ::typeClassInfo          );
	static const DescType typeColorTable         = DescType( ::typeColorTable         );
	static const DescType typeColumn             = DescType( ::typeColumn             );
	static const DescType typeDashStyle          = DescType( ::typeDashStyle          );
	static const DescType typeData               = DescType( ::typeData               );
	static const DescType typeDrawingArea        = DescType( ::typeDrawingArea        );
	static const DescType typeElemInfo           = DescType( ::typeElemInfo           );
	static const DescType typeEnumeration        = DescType( ::typeEnumeration        );
	static const DescType typeEPS                = DescType( ::typeEPS                );
	static const DescType typeEventInfo          = DescType( ::typeEventInfo          );
	static const DescType typeFinderWindow       = DescType( ::typeFinderWindow       );
	static const DescType typeFixedPoint         = DescType( ::typeFixedPoint         );
	static const DescType typeFixedRectangle     = DescType( ::typeFixedRectangle     );
	static const DescType typeGraphicLine        = DescType( ::typeGraphicLine        );
	static const DescType typeGraphicText        = DescType( ::typeGraphicText        );
	static const DescType typeGroupedGraphic     = DescType( ::typeGroupedGraphic     );
	static const DescType typeInsertionLoc       = DescType( ::typeInsertionLoc       );
	static const DescType typeIntlText           = DescType( ::typeIntlText           );
	static const DescType typeIntlWritingCode    = DescType( ::typeIntlWritingCode    );
	static const DescType typeLongDateTime       = DescType( ::typeLongDateTime       );
	static const DescType typeLongFixed          = DescType( ::typeLongFixed          );
	static const DescType typeLongFixedPoint     = DescType( ::typeLongFixedPoint     );
	static const DescType typeLongFixedRectangle = DescType( ::typeLongFixedRectangle );
	static const DescType typeLongPoint          = DescType( ::typeLongPoint          );
	static const DescType typeLongRectangle      = DescType( ::typeLongRectangle      );
	static const DescType typeMachineLoc         = DescType( ::typeMachineLoc         );
	static const DescType typeOval               = DescType( ::typeOval               );
	static const DescType typeParamInfo          = DescType( ::typeParamInfo          );
	static const DescType typePict               = DescType( ::typePict               );
	static const DescType typePixelMap           = DescType( ::typePixelMap           );
	static const DescType typePixMapMinus        = DescType( ::typePixMapMinus        );
	static const DescType typePolygon            = DescType( ::typePolygon            );
	static const DescType typePropInfo           = DescType( ::typePropInfo           );
	static const DescType typePtr                = DescType( ::typePtr                );
	static const DescType typeQDPoint            = DescType( ::typeQDPoint            );
	static const DescType typeQDRegion           = DescType( ::typeQDRegion           );
	static const DescType typeRectangle          = DescType( ::typeRectangle          );
	static const DescType typeRGB16              = DescType( ::typeRGB16              );
	static const DescType typeRGB96              = DescType( ::typeRGB96              );
	static const DescType typeRGBColor           = DescType( ::typeRGBColor           );
	static const DescType typeRotation           = DescType( ::typeRotation           );
	static const DescType typeRoundedRectangle   = DescType( ::typeRoundedRectangle   );
	static const DescType typeRow                = DescType( ::typeRow                );
	static const DescType typeScrapStyles        = DescType( ::typeScrapStyles        );
	static const DescType typeScript             = DescType( ::typeScript             );
	static const DescType typeStyledText         = DescType( ::typeStyledText         );
	static const DescType typeSuiteInfo          = DescType( ::typeSuiteInfo          );
	static const DescType typeTable              = DescType( ::typeTable              );
	static const DescType typeTextStyles         = DescType( ::typeTextStyles         );
	static const DescType typeTIFF               = DescType( ::typeTIFF               );
	static const DescType typeVersion            = DescType( ::typeVersion            );
	
	// kAEMenuClass .. kAEPromise
	// keyMenuID .. keyLocalWhere
	// typeHIMenu, typeHIWindow
	// kBySmallIcon .. kByVersionView
	// kAEInfo .. kAESharing
	// kAEZoomIn, kAEZoomOut
	// kTextServiceClass .. kGetSelectedText
	// keyAETSMDocumentRefcon .. keyAETextServiceMacEncoding
	
	static const DescType typeTextRange           = DescType( ::typeTextRange           );
	static const DescType typeComponentInstance   = DescType( ::typeComponentInstance   );
	static const DescType typeOffsetArray         = DescType( ::typeOffsetArray         );
	static const DescType typeTextRangeArray      = DescType( ::typeTextRangeArray      );
	static const DescType typeLowLevelEventRecord = DescType( ::typeLowLevelEventRecord );
	static const DescType typeEventRef            = DescType( ::typeEventRef            );
	static const DescType typeText                = DescType( ::typeText                );
	
	// kTSMOutsideOfBody .. kTSMInsideOfActiveInputArea
	// kNextBody, kPreviousBody
	
	// kCaretPosition .. kSelectedText
	// keyAEHiliteRange .. keyAEDragging
	
	static const DescType typeUnicodeText       = DescType( ::typeUnicodeText       );
	static const DescType typeStyledUnicodeText = DescType( ::typeStyledUnicodeText );
	static const DescType typeEncodedString     = DescType( ::typeEncodedString     );
	static const DescType typeCString           = DescType( ::typeCString           );
	static const DescType typePString           = DescType( ::typePString           );
	
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
	
	using ::VersRec;
	
	inline std::size_t SizeOf_VersRec( const VersRec& version )
	{
		UInt8 shortVersionLength = version.shortVersion[ 0 ];
		
		// The long version string immediately follows the short version string.
		const UInt8* longVersion = version.shortVersion + 1 + shortVersionLength;
		UInt8 longVersionLength  = longVersion[ 0 ];
		
		return sizeof (::NumVersion)
		     + sizeof (SInt16)
		     + 1 + shortVersionLength
		     + 1 + longVersionLength;
	}
	
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
