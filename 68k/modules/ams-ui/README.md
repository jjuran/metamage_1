ams-ui:  Advanced Mac Substitute User Interface
===============================================

Depends on:

  * [ams-qd][]

Window Manager
--------------

  * calls WDEFs
  * _ZoomWindow

Control Manager
---------------

  * calls CDEFs
  * TODO: _DragControl (docs are unclear on what this actually does)
  * _UpdateControls shim calls DrawControls()
  * _Draw1Control
  * _GetCVariant (reads high byte of contrlDefProc)

Menu Manager
------------

  * patches _SysBeep to flash menubar if sound off
  * calls MDEFs
  * TODO: call MBDF
  * _AddResMenu (TODO: de-dupe names, special-case 'FONT')
  * menu item icons (large, half-scale, 'SICN')
  * TODO: implement _OpenDeskAcc along with DA support
  * TODO: hierarchical menus (they appear in menubar instead)

Icon Manager
------------

  * _PlotIcon
  * _IconDispatch
    * PlotIconID
    * IconIDToRgn
    * PlotSICNHandle

TextEdit
--------

  * TODO: multi-line selection
  * _TEAutoView stub
  * _TEStyleNew just calls TENew()
  * _TEDispatch
    * TEStyleInsert just calls TEInsert()
    * TEGetHeight (assumes fixed line height)

Dialog Manager
--------------

  * _{Could,Free}{Dialog,Alert}() are no-ops
  * _HideDItem
  * _ShowDItem
  * _FindDItem
  * _UpdateDialog just calls DrawDialog()

List Manager
------------

  * calls LDEFs
  * patches _Pack0
    * LActivate
    * LAddRow
    * LClick
    * LDelRow
    * LDispose
    * LSetDrawingMode
    * LGetSelect
    * LLastClick
    * LNew
    * LScroll
    * LSetCell
    * LSize
    * LUpdate

Standard File
-------------

  * SFPutFile
  * SFGetFile
  * SFPPutFile
    * TODO: dlgHook
  * SFPGetFile
    * TODO: dlgHook, fileFilter
  * TODO: calls 5 through 8 from System 7
  * TODO: reuse List Manager for scrolling list?

[ams-qd]:  <../ams-qd/#readme>
