ams-seg:  Advanced Mac Substitute Launch Facilities
===================================================

Depends on:

  * [ams-rsrc][]

Scrap Manager
-------------

  * load/unload works if the Clipboard File already exists
    * unload writes the file if the volume allows it (e.g. Preferences)

Shutdown Manager
----------------

  * calls user-installed shutdown handlers
  * calls UnloadScrap()

Segment Loader
--------------

  * _Launch
    * TODO: send goodBye to open drivers
    * TODO: save scrap onto the stack
    * TODO: reinitialize the unit table
    * TODO: clear system globals
  * _LoadSeg
  * _UnloadSeg shim (no-op)
  * _ExitToShell
    * if FinderName non-empty, launches that application
    * otherwise, calls ShutDwnPower()

[ams-rsrc]:  <../ams-rsrc/#readme>
